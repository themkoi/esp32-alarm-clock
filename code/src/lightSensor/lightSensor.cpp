#include "defines.h"

void dimmingFunction(void *pvParameters);
void dimOledDisplay();
void dimLedDisplay();

bool maxBrightness = false;

bool dimmingTaskRunning = false;

bool displayON = true;

bool inputDetected = false;

TaskHandle_t dimmingTask;

void createDimmingTask()
{
    for (int i = 0; i < CHART_READINGS; i++)
    {
        lightArray[i] = getLightLevel();
    }
    Serial.print("creating dimmingTask");
    xTaskCreatePinnedToCore(
        dimmingFunction, /* Task function. */
        "DimTask",       /* String with name of task. */
        10000,           /* Stack size in words. */
        NULL,            /* Parameter passed as input of the task */
        1,               /* Priority of the task. */
        &dimmingTask,    /* Task handle. */
        1                /* Core where the task should run. */
    );
}

bool dimmed = false;

float lightLevel = 0.0;
int lightState = 0;

void dimmingFunction(void *pvParameters)
{
    unsigned long intervalLight = 60000;
    unsigned long previousMillisLight = 0;

    unsigned long previousMillisDimming = 0;
    unsigned long intervalDimming = 1000;

    unsigned long previousMillisState = 0;
    unsigned long intervalState = 30000;

    unsigned long lastActionTime = 0;
    
    while (true) // FreeRTOS task must always run
    {
        dimmingTaskRunning = true;

        while (buttons.checkInput() == false) // Only executes when a button is pressed
        {
            unsigned long currentMillis = millis();

            if (currentMillis - previousMillisLight >= intervalLight)
            {
                for (int i = 0; i < CHART_READINGS - 1; i++)
                {
                    lightArray[i] = lightArray[i + 1];
                }
                lightArray[CHART_READINGS - 1] = getLightLevel();
                previousMillisLight = currentMillis;
            }

            if (currentMillis - previousMillisState >= intervalState && WiFi.isConnected() && powerConnected && WiFi.SSID() == SSID1)
            {
                lightState = getLightState();
                previousMillisState = currentMillis;
                delay(100);
            }

            if (currentMillis - previousMillisDimming >= intervalDimming && powerConnected)
            {
                lightLevel = getLightLevel();
                dimOledDisplay();
                maxBrightness = false;

                Serial.println("Reading brightness and dimming OLED accordingly: " + String(lightLevel));
                previousMillisDimming = currentMillis;
            }
            
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        if (checkPower() == true)
        {
            Serial.println("Button pressed");
            Serial.println("Setting max brightness");

            maxBrightness = true;
            lastActionTime = millis();
            
            if (manager.dimmed)
            {
                manager.oledFadeIn();
                manager.oledEnable();
            }

            vTaskDelay(pdMS_TO_TICKS(100));

            while (millis() - lastActionTime < DIM_DELAY)
            {
                vTaskDelay(pdMS_TO_TICKS(100));
                
                if (buttons.checkInput() == true)
                {
                    lastActionTime = millis();
                    
                    if (manager.dimmed)
                    {
                        manager.oledFadeIn();
                    }

                    if (!manager.ScreenEnabled)
                    {
                        manager.oledEnable();
                    }

                    vTaskDelay(pdMS_TO_TICKS(100));

                    while (buttons.checkInput() == true)
                    {
                        lastActionTime = millis();
                    }
                }
            }

            dimOledDisplay();
            inputDetected = false;
        }
    }
}

bool shouldTurnOffDisplay(int lux)
{
    static const int offThreshold = OLED_DISABLE_THRESHOLD;     // Lux threshold to turn off
    static const int onThreshold = OLED_DISABLE_THRESHOLD + 10; // Lux threshold to turn back on
    static bool isDisplayOff = false;

    if (isDisplayOff && lux >= onThreshold)
    {
        isDisplayOff = false; // Turn on when lux exceeds onThreshold
    }
    else if (!isDisplayOff && lux < offThreshold)
    {
        isDisplayOff = true; // Turn off when lux drops below offThreshold
    }

    return isDisplayOff;
}

float previousLightLevel = 0.0;

void dimOledDisplay()
{
    int currentHour = hour();
    int currentMinute = minute();
    Serial.println("raw light level: " + String(getLightLevel()));
    Serial.println("smoothened light level: " + String(lightLevel));

    if (shouldTurnOffDisplay(getLightLevel()) == true || (lightState == 0 && WiFi.SSID() == SSID1 && lightState != 3 && WiFi.isConnected() == true))
    {
        manager.oledDisable();

        if (manager.dimmed == false)
        {
            manager.oledFadeOut();
            delay(50);
        }
        delay(50);
    }
    else
    {
        manager.oledEnable();

        if (manager.dimmed == false)
        {
            manager.oledFadeOut();
        }
    }

    Serial.println("display off");
}

static int ledLastBrightness = LED_BRIGHTNESS_MIN;

int mapWithHysteresis(int lightLevel) {
    // Handle case where light is below the disable threshold
    if (lightLevel <= LED_DISABLE_THRESHOLD) {
        ledLastBrightness = LED_BRIGHTNESS_MIN;
        return ledLastBrightness;
    }

    // Manual linear mapping (same as map function)
    int newBrightness = (lightLevel - LED_DIM_THRESHOLD) * (LED_BRIGHTNESS_MAX - LED_BRIGHTNESS_MIN) /
                        (LED_MAX_BRIGHTNESS - LED_DIM_THRESHOLD) + LED_BRIGHTNESS_MIN;

    // Constrain brightness within valid range
    newBrightness = constrain(newBrightness, LED_BRIGHTNESS_MIN, LED_BRIGHTNESS_MAX);

    // Hysteresis logic: separate conditions for increasing and decreasing brightness
    if (newBrightness > ledLastBrightness + LED_HYSTERESIS) {
        ledLastBrightness = newBrightness; // Allow increase when crossing upper hysteresis
    } 
    else if (newBrightness < ledLastBrightness - LED_HYSTERESIS) {
        ledLastBrightness = newBrightness; // Allow decrease when crossing lower hysteresis
    }

    return ledLastBrightness;
}


void dimLedDisplay()
{
    int currentHour = hour();
    int currentMinute = minute();
    lightLevel = getLightLevel();
    Serial.println("raw light level: " + String(getLightLevel()));
    Serial.println("smoothened light level: " + String(lightLevel));

    if (lightLevel < 5000)
    {

        if (lightLevel <= LED_DISABLE_THRESHOLD && (checkForNight() == true))
        {
            LedDisplay.clear();
            displayON = false;
        }
        else if (lightLevel > LED_DIM_THRESHOLD)
        {
            displayON = true;
            LedDisplay.setBrightness(mapWithHysteresis(lightLevel));
            Serial.println("Brightness of Led display " + String(map(constrain(lightLevel, 0, LED_DIM_THRESHOLD), 0, LED_DIM_THRESHOLD, 0, 7)));
        }
        else
        {
            displayON = true;
            LedDisplay.setBrightness(0);
            Serial.println("Brightness of Led display 0");
        }
    }
}

float getLightLevel()
{
    float currentLightLevel = lightMeter.readBrightnessInLux(); // Read the current light level from BH1750 sensor
    return currentLightLevel;
}

bool checkForNight()
{
    time_t currentTime = now();
    int weekdayIndex = weekday(currentTime) - 1;

    int currentHour = hour();

    if ((alarms[weekdayIndex].hours == 0 && alarms[weekdayIndex].minutes == 0) && alarms[weekdayIndex].enabled == false)
    {
        if (currentHour >= 23 || currentHour < 10)
        {
            return true;
        }
        else
            return false;
    }
    else
    {
        if (currentHour >= 23 || currentHour < alarms[weekdayIndex].hours)
        {
            return true;
        }
        else
            return false;
    }
}

int getLightState()
{
    String url = "http://192.168.88.74/gateways/4276/RGB/0";
    String jsonString;
    const int maxRetries = 3;

    for (int attempt = 0; attempt < maxRetries; ++attempt)
    {
        jsonString = getStringRequest(url);

        if (jsonString.length() > 0)
        {
            break; // Exit the loop if we get a valid response
        }
        else
        {
            Serial.println("Failed to fetch data, retrying...");
            delay(1000); // Wait 1 second before retrying
        }
    }

    if (jsonString.length() == 0)
    {
        Serial.println("Failed to fetch data after multiple attempts.");
        return 3; // Return an error code if all retries fail
    }

    JsonDocument jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, jsonString);

    if (error)
    {
        Serial.print("deserializeJson() returned ");
        Serial.println(error.c_str());
        return 3; // Return error code if deserialization fails
    }

    const char *state = jsonDoc["state"]; // Extract the state field

    if (state != nullptr && strcmp(state, "ON") == 0)
    {
        Serial.println("Light is ON");
        return 1;
    }

    Serial.println("Light is OFF");
    return 0;
}
