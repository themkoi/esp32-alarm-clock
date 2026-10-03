#include "lightSensor.h"

#define AL_ADDR 0x10
SparkFun_Ambient_Light lightMeter(AL_ADDR);

void dimmingTask(void *pvParameters);
void oledWakeupTask(void *pvParameters);
void dimOledDisplay();
void dimLedDisplay(bool checkForOff);

unsigned long displayHoldUntil = 0;

bool maxBrightness = false;

bool dimmingTaskRunning = false;

bool LedDisplayOn = true;

bool inputDetected = false;

TaskHandle_t dimmingTaskHandle;
TaskHandle_t oledWakeupTaskHandle;

void lightTask(void *pvParameters)
{
    unsigned long previousMillisChart = 0;
    while (true)
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillisChart >= INTERVAL_CHARTS)
        {
            getLightLevel();
            for (int i = 0; i < CHART_READINGS - 1; i++)
            {
                lightArray[i] = lightArray[i + 1];
            }
            lightArray[CHART_READINGS - 1] = lightLevel;
            previousMillisChart = currentMillis;
        }
        vTaskDelay(pdMS_TO_TICKS(600000));
    }
}

void createLightTask()
{
    xTaskCreate(
        lightTask,
        "lightTask",
        2048,
        NULL,
        1,
        NULL
    );
}

void createDimmingTask()
{
    Serial.print("creating dimmingTask");
    xTaskCreate(
        dimmingTask,
        "DimTask",
        4096,
        NULL,
        1,
        &dimmingTaskHandle
    );

    xTaskCreate(
        oledWakeupTask,
        "InputOledTask",
        4096,
        NULL,
        3,
        &oledWakeupTaskHandle
    );
}

float lightLevel = 0.0;
int mmwaveState = 0;

void oledWakeupTask(void *pvParameters)
{
    unsigned long lastActionTime = 0;
    while (true)
    {
        if (useAllButtons() != None || useAllTouch().touched == true || inputDetected == true)
        {
            vTaskSuspend(dimmingTaskHandle);
            Serial.println("Button pressed");
            Serial.println("Setting max brightness");

            inputDetected = true;
            maxBrightness = true;
            lastActionTime = millis();

            oledMana.enable();

            Serial.print("lock mutex");
            showCurrentTime();

            if (checkForNight())
            {
                setLedIntensity(LED_BRIGHTNESS_MAX_NIGHT);
            }
            else
            {
                setLedIntensity(LED_BRIGHTNESS_MAX);
            }
            Serial.print("unlock mutex");

            if (oledMana.dimmed)
            {
                oledMana.fadeIn();
                delay(5);
            }
            inputDetected = false;

            vTaskDelay(pdMS_TO_TICKS(100));

            while (millis() - lastActionTime < DIM_DELAY)
            {
                vTaskDelay(pdMS_TO_TICKS(5));

                if (useAllButtons() != None || useAllTouch().touched == true || inputDetected == true)
                {
                    inputDetected = false;
                    lastActionTime = millis();

                    if (!oledMana.ScreenEnabled)
                    {
                        oledMana.enable();
                    }

                    vTaskDelay(pdMS_TO_TICKS(5));
                }
            }
            if (WiFi.isConnected() && WiFi.SSID() == SSID1)
            {
                mmwaveState = getMmwaveState();
            }
            displayHoldUntil = millis() + DISPLAY_HOLD_DELAY;

            getLightLevel();
            dimOledDisplay();
            oledMana.fadeOut();
            dimLedDisplay(false);
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
    unsigned long previousMillisDimming = 0;
    unsigned long intervalDimming = 1000;

    unsigned long previousMillisLight = 0;

    unsigned long previousMillisState = 0;
    unsigned long intervalState = 30000;

    while (true)
    {
        dimmingTaskRunning = true;
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillisLight >= intervalDimming)
        {
            getLightLevel();
            previousMillisLight = currentMillis;
        }

        if (currentMillis - previousMillisState >= intervalState && WiFi.isConnected() && WiFi.SSID() == SSID1)
        {
            mmwaveState = getMmwaveState();
            previousMillisState = currentMillis;
            delay(100);
        }

        if (currentMillis - previousMillisDimming >= intervalDimming)
        {
            dimLedDisplay(true);
            dimOledDisplay();
            maxBrightness = false;

            Serial.println("dimming OLED accordingly: " + String(lightLevel));
            previousMillisDimming = currentMillis;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool shouldTurnOffOled(int lux)
{
    if (millis() < displayHoldUntil)
    {
        return false;
    }

    static const int offThreshold = OLED_DISABLE_THRESHOLD;
    static const int onThreshold = OLED_DISABLE_THRESHOLD + 10;
    static bool isDisplayOff = false;

    if (isDisplayOff && lux >= onThreshold)
    {
        isDisplayOff = false;
    }
    else if (!isDisplayOff && lux < offThreshold)
    {
        isDisplayOff = true;
    }

    return isDisplayOff;
}

float previousLightLevel = 0.0;

void dimOledDisplay()
{
    if (shouldTurnOffOled(lightLevel) == true || (mmwaveState == 0 && WiFi.SSID() == SSID1 && mmwaveState != 3 && WiFi.isConnected() == true))
    {
        oledMana.disable();

        if (oledMana.dimmed == false)
        {
            oledMana.fadeOut();
            delay(50);
        }
        delay(50);
    }
    else
    {
        oledMana.enable();

        if (oledMana.dimmed == false)
        {
            oledMana.fadeOut();
        }
    }
}

static int ledLastBrightness = LED_BRIGHTNESS_MIN;

int mapLedWithHysteresis(uint16_t lightLevel)
{
    uint8_t maxBright = !checkForNight()
                            ? LED_BRIGHTNESS_MAX
                            : LED_BRIGHTNESS_MAX_NIGHT;

    lightLevel = constrain(
        lightLevel,
        LED_DIM_THRESHOLD,
        LED_MAP_MAX_LIGHT);

    float normalized =
        (float)(lightLevel - LED_DIM_THRESHOLD) /
        (float)(LED_MAP_MAX_LIGHT - LED_DIM_THRESHOLD);

    normalized =
        log10f(1.0f + normalized * 9.0f);

    uint8_t newBrightness =
        LED_BRIGHTNESS_MIN +
        normalized * (maxBright - LED_BRIGHTNESS_MIN);

    newBrightness = constrain(
        newBrightness,
        LED_BRIGHTNESS_MIN,
        maxBright);

    if (abs(newBrightness - ledLastBrightness) >= LED_HYSTERESIS)
    {
        ledLastBrightness = newBrightness;
    }

    return ledLastBrightness;
}

bool disableHysteresisState = false;

void dimLedDisplay(bool checkForOff)
{
    if (lightLevel < 5000)
    {
        if (xSemaphoreTake(LedMut, portMAX_DELAY))
        {
            if (disableHysteresisState)
            {
                if (lightLevel > LED_DISABLE_THRESHOLD + 6)
                    disableHysteresisState = false;
            }
            else
            {
                if (lightLevel <= LED_DISABLE_THRESHOLD - 0.5 && checkForNight())
                    disableHysteresisState = true;
            }

            if (disableHysteresisState && checkForOff == true)
            {
                if (millis() < displayHoldUntil)
                {
                    xSemaphoreGive(LedMut);
                    return;
                }
                
                if (lockI2C())
                {
                    LedDisplay.clear();
                    unlockI2C();
                }
                LedDisplayOn = false;
            }
            else if (lightLevel > LED_DIM_THRESHOLD)
            {
                LedDisplayOn = true;
                uint8_t brightness = mapLedWithHysteresis(lightLevel);
                setLedIntensity(brightness);
                Serial.println("Brightness of Led display " + String(brightness));
            }
            else
            {
                LedDisplayOn = true;
                setLedIntensity(0);
                Serial.println("Brightness of Led display 0");
            }

            xSemaphoreGive(LedMut);
        }
    }
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
    }
    else
    {
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
            break;
        }
        else
        {
            Serial.println("Failed to fetch data, retrying...");
            delay(1000);
        }
    }

    if (jsonString.length() == 0)
    {
        Serial.println("Failed to fetch data after multiple attempts.");
        return 3;
    }

    JsonDocument jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, jsonString);

    if (error)
    {
        Serial.print("deserializeJson() returned ");
        Serial.println(error.c_str());
        return 3;
    }

    bool state = jsonDoc["Detected"];

    if (state)
    {
        Serial.println("Detected mmWave");
        return 1;
    }

    Serial.println("Not Detected mmWave");
    return 0;
}

float getLightLevel()
{
    static uint32_t lastRead = 0;

    if (millis() - lastRead >= 400)
    {
        lastRead = millis();

        float currentLightLevel = lightMeter.readLight();

        if (!isnan(currentLightLevel))
        {
            // Apply exponential moving average filter using the defined macro
            lightLevel = (LIGHT_ALPHA * currentLightLevel) + ((1.0f - LIGHT_ALPHA) * lightLevel);
        }
    }

    return lightLevel;
}

void initLightSensor()
{
    if (lightMeter.begin(Wire))
        Serial.println("Ready to sense some light!");
    else
        Serial.println("Could not communicate with the sensor!");

    lightMeter.setGain(1);
    lightMeter.setIntegTime(400);

    Serial.println("Reading settings...");
    Serial.print("Gain: ");
    float gainVal = lightMeter.readGain();
    Serial.print(gainVal, 3);
    Serial.print(" Integration Time: ");
    int timeVal = lightMeter.readIntegTime();
    Serial.println(timeVal);
}

void disableLightSensor()
{
    lightMeter.enablePowSave();
    Serial.println("Placed light sensor into low-power mode.");
}

void enableLightSensor()
{
    lightMeter.disablePowSave();
    delay(10);
    Serial.println("Woke up light sensor and restored normal mode.");
}