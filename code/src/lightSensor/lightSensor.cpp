#include "defines.h"

void dimmingTask(void *pvParameters);
void oledWakeupTask(void *pvParameters);
void dimOledDisplay();
void dimLedDisplay();

bool maxBrightness = false;

bool dimmingTaskRunning = false;

bool displayON = true;

bool inputDetected = false;

TaskHandle_t dimmingTaskHandle;
TaskHandle_t oledWakeupTaskHandle;

void createDimmingTask()
{
    for (int i = 0; i < CHART_READINGS; i++)
    {
        lightArray[i] = getLightLevel();
    }
    Serial.print("creating dimmingTask");
    xTaskCreatePinnedToCore(
        dimmingTask,        /* Task function. */
        "DimTask",          /* String with name of task. */
        4096,              /* Stack size in words. */
        NULL,               /* Parameter passed as input of the task */
        1,                  /* Priority of the task. */
        &dimmingTaskHandle, /* Task handle. */
        1                   /* Core where the task should run. */
    );

    xTaskCreatePinnedToCore(
        oledWakeupTask, /* Task function. */
        "InputOledTask",      /* String with name of task. */
        2048,          /* Stack size in words. */
        NULL,           /* Parameter passed as input of the task */
        3,              /* Priority of the task. */
        &oledWakeupTaskHandle,           /* Task handle. */
        1               /* Core where the task should run. */
    );
}

bool dimmed = false;

float lightLevel = 0.0;
int mmwaveState = 0;

void oledWakeupTask(void *pvParameters)
{
    unsigned long lastActionTime = 0;
    while (true)
    {
        if (useAllButtons() != None || useAllTouch() != No_Seg || inputDetected == true)
        {
            vTaskSuspend(dimmingTaskHandle);
            vTaskResume(LedTask);
            Serial.println("Button pressed");
            Serial.println("Setting max brightness");

            inputDetected = true;
            maxBrightness = true;
            lastActionTime = millis();

            manager.sendOledAction(OLED_ENABLE);

            if (manager.dimmed)
            {
                delay(5);
                manager.sendOledAction(OLED_FADE_IN);
            }
            inputDetected = false;

            vTaskDelay(pdMS_TO_TICKS(100));

            while (millis() - lastActionTime < DIM_DELAY)
            {
                vTaskDelay(pdMS_TO_TICKS(5));

                if (useAllButtons() != None || useAllTouch() != No_Seg || inputDetected == true)
                {
                    inputDetected = false;
                    lastActionTime = millis();


                    if (!manager.ScreenEnabled)
                    {
                        manager.sendOledAction(OLED_ENABLE);
                    }

                    vTaskDelay(pdMS_TO_TICKS(5));

                    if (useAllButtons() != None || useAllTouch() != No_Seg || inputDetected == true)
                    {
                        inputDetected = false;
                        lastActionTime = millis();
                    }
                }
            }

            dimOledDisplay();
            lightLevel = getLightLevel();
        }
        else
        {
            eTaskState dimmingTaskState = eTaskGetState(dimmingTaskHandle);
            if (dimmingTaskState == eSuspended)
            {
                vTaskResume(dimmingTaskHandle);
            }
            delay(10);
        }
    }
}

void dimmingTask(void *pvParameters)
{
    unsigned long previousMillisChart = 0;
    unsigned long previousMillisLight = 0;

    unsigned long previousMillisDimming = 0;
    unsigned long intervalDimming = 1000;

    unsigned long previousMillisState = 0;
    unsigned long intervalState = 30000;

    while (true)
    {
        dimmingTaskRunning = true;
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillisChart >= INTERVAL_CHARTS)
        {
            lightLevel = getLightLevel();
            for (int i = 0; i < CHART_READINGS - 1; i++)
            {
                lightArray[i] = lightArray[i + 1];
            }
            lightArray[CHART_READINGS - 1] = lightLevel;
            previousMillisChart = currentMillis;
        }

        if (currentMillis - previousMillisLight >= intervalDimming)
        {
            lightLevel = getLightLevel();
            previousMillisLight = currentMillis;
        }

        if (currentMillis - previousMillisState >= intervalState && WiFi.isConnected()&& WiFi.SSID() == SSID1)
        {
            mmwaveState = getMmwaveState();
            previousMillisState = currentMillis;
            delay(100);
        }

        if (currentMillis - previousMillisDimming >= intervalDimming)
        {
            dimOledDisplay();
            maxBrightness = false;

            Serial.println("dimming OLED accordingly: " + String(lightLevel));
            previousMillisDimming = currentMillis;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
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

    if (shouldTurnOffDisplay(lightLevel) == true || (mmwaveState == 0 && WiFi.SSID() == SSID1 && mmwaveState != 3 && WiFi.isConnected() == true))
    {
        manager.sendOledAction(OLED_DISABLE);

        if (manager.dimmed == false)
        {
            manager.sendOledAction(OLED_FADE_OUT);
            delay(50);
        }
        delay(50);
    }
    else
    {
        manager.sendOledAction(OLED_ENABLE);

        if (manager.dimmed == false)
        {
            manager.sendOledAction(OLED_FADE_OUT);
        }
    }
}

static int ledLastBrightness = LED_BRIGHTNESS_MIN;

int mapWithHysteresis(int lightLevel)
{
    if (lightLevel <= LED_DISABLE_THRESHOLD)
    {
        ledLastBrightness = LED_BRIGHTNESS_MIN;
        return ledLastBrightness;
    }

    // Manual linear mapping (same as map function)
    int newBrightness = (lightLevel - LED_DIM_THRESHOLD) * (LED_BRIGHTNESS_MAX - LED_BRIGHTNESS_MIN) /
                            (LED_MAX_BRIGHTNESS - LED_DIM_THRESHOLD) +
                        LED_BRIGHTNESS_MIN;

    // Constrain brightness within valid range
    newBrightness = constrain(newBrightness, LED_BRIGHTNESS_MIN, LED_BRIGHTNESS_MAX);

    if (newBrightness > ledLastBrightness + LED_HYSTERESIS)
    {
        ledLastBrightness = newBrightness;
    }
    else if (newBrightness < ledLastBrightness - LED_HYSTERESIS)
    {
        ledLastBrightness = newBrightness;
    }

    return ledLastBrightness;
}

bool disableHysteresisState = false;

void dimLedDisplay()
{
    int currentHour = hour();
    int currentMinute = minute();

    if (lightLevel < 5000)
    {
        if (disableHysteresisState)
        {
            if (lightLevel > LED_DISABLE_THRESHOLD + 3) // hysteresis upper limit
                disableHysteresisState = false;
        }
        else
        {
            if (lightLevel <= LED_DISABLE_THRESHOLD - 3 && checkForNight())
                disableHysteresisState = true;
        }

        if (disableHysteresisState)
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
    if (isWeatherAvailable == false)
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
    } else {
        return !currentWeatherData.isDay;
    }
}

int getMmwaveState()
{
    String url = String(LIGHT_IP) + "mmwave";
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

    bool state = jsonDoc["Detected"];

    if (state)
    {
        Serial.println("Detected");
        return 1;
    }
    
    Serial.println("Not Detected");
    return 0;
}
