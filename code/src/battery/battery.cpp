#include "battery.h"

int batteryPercentage;
float batteryVoltage;

bool powerConnected = false;
bool charging = false;

bool wentToSleep = false;

bool goToSleep = false;

void enableSleep();
void initSleep();

void manageBattery(void *parameter);

void controlCharger();

void createBatteryTask()
{
  xTaskCreatePinnedToCore(
      manageBattery, // Function to implement the task
      "Battery",     // Task name
      4096,          // Stack size (words)
      NULL,          // Task input parameter
      3,             // Priority (0 is lowest)
      NULL,          // Task handle
      0              // Core to run the task on (0 or 1)
  );
}

bool checkPower()
{
  int chargingState = digitalRead(POWER_STATE_PIN);
  Serial.print("Charging State: ");
  Serial.println(chargingState);
  if (chargingState == HIGH)
  {
    powerConnected = true;
    return true;
  }
  else
  {
    powerConnected = false;
    return false;
  }
}

bool batterySleepMode = false;
bool wokeUp = true;

void manageBattery(void *parameter)
{
  const unsigned long batCheInterval = 15000;
  const unsigned long batteryWaitTimeout = 30000;
  unsigned long lastRunBatChe = 0;
  unsigned long batterySettingsTime = 0;
  unsigned long wakeupTime = 0;

  bool setPowerSettings = true;
  bool waitingForPower = false;
  bool waitForInput = false;

  powerConnected = checkPower();
  batteryVoltage = getBatteryVoltage();
  controlCharger();

  while (true)
  {
    delay(pdMS_TO_TICKS(100));
    unsigned long now = millis();
    if (now - lastRunBatChe >= batCheInterval)
    {
      lastRunBatChe = now;
      powerConnected = checkPower();
      batteryVoltage = getBatteryVoltage();
      controlCharger();
    }

    if (powerConnected)
    {
      if (!setPowerSettings)
      {
        Serial.println("Setting power settings");
        vTaskResume(oledWakeupTaskHandle);
        vTaskResume(LedTask);
        touchSetCycles(0x500, 0x500);
        setTouchInterrupt(TOUCH_1_Seg_PIN, TOUCH_1_Seg_THRESHOLD);
        setTouchInterrupt(TOUCH_2_Seg_PIN, TOUCH_2_Seg_THRESHOLD);
        setTouchInterrupt(TOUCH_3_Seg_PIN, TOUCH_3_Seg_THRESHOLD);
        setTouchInterrupt(TOUCH_4_Seg_PIN, TOUCH_4_Seg_THRESHOLD);
        setTouchInterrupt(TOUCH_5_Seg_PIN, TOUCH_5_Seg_THRESHOLD);
        esp_pm_config_t pm_config = {
            .max_freq_mhz = 80,
            .min_freq_mhz = 10,
            .light_sleep_enable = true,
        };
        esp_pm_configure(&pm_config);
        setPowerSettings = true;
        batterySleepMode = false;
        waitingForPower = false;
        esp_wifi_start();
        wokeUp = true;
      }

      if (!WiFi.isConnected() && !WiFiTaskRunning)
      {
        esp_wifi_start();
        Serial.println("launching WiFi task");
        createWifiTask();
      }
    }
    else
    {
      if (setPowerSettings)
      {
        Serial.println("Setting battery settings");
        turnOffWifi();
        vTaskSuspend(oledWakeupTaskHandle);
        vTaskSuspend(LedTask);
        vTaskSuspend(dimmingTaskHandle);
        setTouchInterrupt(TOUCH_1_Seg_PIN, TOUCH_1_Seg_THRESHOLD_BAT);
        setTouchInterrupt(TOUCH_2_Seg_PIN, TOUCH_2_Seg_THRESHOLD_BAT);
        setTouchInterrupt(TOUCH_3_Seg_PIN, TOUCH_3_Seg_THRESHOLD_BAT);
        setTouchInterrupt(TOUCH_4_Seg_PIN, TOUCH_4_Seg_THRESHOLD_BAT);
        setTouchInterrupt(TOUCH_5_Seg_PIN, TOUCH_5_Seg_THRESHOLD_BAT);
        batterySettingsTime = now;
        waitingForPower = true;
        setPowerSettings = false;
        maxBrightness = false;
        inputDetected = false;
        esp_wifi_stop();
        esp_pm_config_t pm_config = {
            .max_freq_mhz = 40,
            .min_freq_mhz = 10,
            .light_sleep_enable = true,
        };
        esp_pm_configure(&pm_config);
      }

      if (waitingForPower == true)
      {
        if (useAllButtons() != None || useAllTouch() != No_Seg || inputDetected == true)
        {
          waitForInput = true;
          inputDetected = false;
          manager.sendOledAction(OLED_ENABLE);
          LedDisplay.setBrightness(2);
          LedDisplay.showNumberDecEx(hour() * 100 + minute(), 0b11100000, true);
          batterySettingsTime = now;
        }

        if (waitingForPower && (now - batterySettingsTime >= batteryWaitTimeout))
        {
          batterySleepMode = true;
          waitingForPower = false;
          Serial.println("Battery mode");
          syncESP32RTC();
          enableSleep();
        }
      }

      if (batterySleepMode)
      {
        esp_sleep_wakeup_cause_t wakeCause = esp_sleep_get_wakeup_cause();

        if (wakeCause == ESP_SLEEP_WAKEUP_TIMER)
        {
          if (!wokeUp)
          {
            Serial.println("Woke up, waiting for input...");
            wokeUp = true;
            waitForInput = false;
            wakeupTime = now;
          }

          if (useAllButtons() != None || useAllTouch() != No_Seg || inputDetected == true)
          {
            Serial.println("Input detected waiting more");
            waitForInput = true;
            inputDetected = false;
            manager.sendOledAction(OLED_ENABLE);
            LedDisplay.setBrightness(2);
            LedDisplay.showNumberDecEx(hour() * 100 + minute(), 0b11100000, true);
            wakeupTime = now;
          }

          if (waitForInput == false)
          {
            if (now - wakeupTime >= TIMER_WAKUP_TIME)
            {
              Serial.println("No input with timer going back to sleep");
              waitForInput = false;
              enableSleep();
            }
          }
          else
          {
            if (now - wakeupTime >= GPIO_WAKUP_TIME)
            {
              Serial.println("No input with input going back to sleep");
              waitForInput = false;
              enableSleep();
            }
          }
        }
        if (wakeCause == ESP_SLEEP_WAKEUP_TIMER)
        {
          if (!wokeUp)
          {
            Serial.println("Woke up, waiting for input...");
            wokeUp = true;
            wakeupTime = now;
            inputDetected = true;
          }

          if ((useAllButtons() != None || useAllTouch() != No_Seg || inputDetected == true))
          {
            Serial.println("Input detected waiting more");
            inputDetected = false;
            manager.sendOledAction(OLED_ENABLE);
            LedDisplay.setBrightness(2);
            LedDisplay.showNumberDecEx(hour() * 100 + minute(), 0b11100000, true);
            wakeupTime = now;
          }

          if (now - wakeupTime >= GPIO_WAKUP_TIME)
          {
            Serial.println("No input after input going back to sleep");
            waitForInput = false;
            enableSleep();
          }
        }
      }
    }
  }
}

void controlCharger()
{
  if (charging)
  {
    if (batteryVoltage >= BATT_TARGET_VOLTAGE)
    {
      charging = false;
      digitalWrite(CHARGER_CONTROL_PIN, LOW); // Stop charging
      Serial.println("Charging stopped (target voltage reached).");
    }
  }
  else
  {
    if (batteryVoltage <= (BATT_TARGET_VOLTAGE - BATT_HYSTERESIS))
    {
      charging = true;
      digitalWrite(CHARGER_CONTROL_PIN, HIGH); // Start charging
      Serial.println("Charging started (voltage dropped).");
    }
  }

  Serial.print("Battery Voltage: ");
  Serial.print(batteryVoltage);
  Serial.print(" V - Charging: ");
  Serial.println(charging ? "ON" : "OFF");
}

void initSleep()
{
  touchSleepWakeUpEnable(TOUCH_1_Seg_PIN, TOUCH_1_Seg_THRESHOLD_SLEEP);
  touchSleepWakeUpEnable(TOUCH_2_Seg_PIN, TOUCH_2_Seg_THRESHOLD_SLEEP);
  touchSleepWakeUpEnable(TOUCH_3_Seg_PIN, TOUCH_3_Seg_THRESHOLD_SLEEP);
  touchSleepWakeUpEnable(TOUCH_4_Seg_PIN, TOUCH_4_Seg_THRESHOLD_SLEEP);
  touchSleepWakeUpEnable(TOUCH_5_Seg_PIN, TOUCH_5_Seg_THRESHOLD_SLEEP);

  esp_sleep_enable_touchpad_wakeup();
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

  esp_sleep_enable_timer_wakeup(SLEEPING_TIME);
}

void enableSleep()
{

  delay(500);
  manager.sendOledAction(OLED_DISABLE);
  display.ssd1306_command(SSD1306_DISPLAYOFF); // Just to make sure because manager can take a bit before reacting if many write operations are ordered
  delay(500);
  LedDisplay.setBrightness(0);
  delay(100);
  LedDisplay.clear();
  initSleep();
  wokeUp = false;
  esp_err_t sleep_result = esp_light_sleep_start();

  if (sleep_result == ESP_OK)
  {
    Serial.println("Light sleep entered and woke up successfully.");
  }
  else if (sleep_result == ESP_ERR_SLEEP_REJECT)
  {
    Serial.println("Sleep request rejected: Wakeup source set before the sleep request.");
  }
  else if (sleep_result == ESP_ERR_SLEEP_TOO_SHORT_SLEEP_DURATION)
  {
    Serial.println("Sleep duration too short: Minimum sleep duration not met.");
  }
  else
  {
    Serial.printf("Unexpected sleep error: %d\n", sleep_result);
  }
  esp_pm_config_t pm_config = {
      .max_freq_mhz = 40,
      .min_freq_mhz = 10,
      .light_sleep_enable = true,
  };
  esp_pm_configure(&pm_config);
  syncTimeLibWithRTC();
  checkPower();
  delay(200);
}

double readVoltage(byte pin)
{
  double reading = analogRead(VOLTAGE_DIVIDER_PIN);
  if (reading < 1 || reading > 4095)
    return 0;
  double voltage = -0.000000000000016 * pow(reading, 4) + 0.000000000118171 * pow(reading, 3) - 0.000000301211691 * pow(reading, 2) + 0.001109019271794 * reading + 0.034143524634089;
  return voltage * 1000;
}

float getBatteryVoltage()
{
  double miliVolts = readVoltage(VOLTAGE_DIVIDER_PIN);
  miliVolts = miliVolts - ADC_OFFSET;
  float batteryVoltage = miliVolts / ADC_VOLTAGE_DIVIDER;

  return batteryVoltage;
}

int getBatteryPercentage()
{
  int percentage = ((batteryVoltage - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE)) * 100.00;
  percentage = min(percentage, 100);
  if (percentage < 0)
  {
    percentage = 0;
    return percentage;
  }
  else
    return percentage;
}