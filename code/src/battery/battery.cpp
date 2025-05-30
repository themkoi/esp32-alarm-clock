#include "battery.h"

int batteryPercentage;

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
      10000,         // Stack size (words)
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

void manageBattery(void *parameter)
{
  static bool preparingForSleep = false;
  static bool previousPowerConnected = false;
  while (true)
  {
    batteryPercentage = getBatteryPercentage();

    Serial.println("WiFi status " + wifiStatusToString(WiFi.status()));

    eTaskState WiFiTaskState = eTaskGetState(wifiTask);
    checkPower();
    if (powerConnected == true)
    {
      vTaskResume(oledWakeupTaskHandle);
      vTaskResume(LedTask);
      esp_pm_config_t pm_config = {
          .max_freq_mhz = 80,
          .min_freq_mhz = 10,
          .light_sleep_enable = true,
      };
      esp_pm_configure(&pm_config);
      Serial.println("Set pm config");
      if (previousPowerConnected == false)
      {
        setTouchInterrupt(TOUCH_1_Seg_PIN,TOUCH_1_Seg_THRESHOLD);
        setTouchInterrupt(TOUCH_2_Seg_PIN,TOUCH_2_Seg_THRESHOLD);
        setTouchInterrupt(TOUCH_3_Seg_PIN,TOUCH_3_Seg_THRESHOLD);
        setTouchInterrupt(TOUCH_4_Seg_PIN,TOUCH_4_Seg_THRESHOLD);
        setTouchInterrupt(TOUCH_5_Seg_PIN,TOUCH_5_Seg_THRESHOLD);
      }

      previousPowerConnected = powerConnected;
      wentToSleep = false;
      preparingForSleep = false;
      Serial.println("Power connected");
      controlCharger();
      vTaskDelay(pdMS_TO_TICKS(500));
      if (!WiFi.isConnected() && WiFiTaskRunning == false)
      {
        esp_wifi_start();
        Serial.println("launching WiFi task");
        createWifiTask();
      }
    }
    else
    {
      digitalWrite(CHARGER_CONTROL_PIN, LOW);
      turnOffWifi();
      vTaskDelay(pdMS_TO_TICKS(500));
      previousPowerConnected = false;

      static unsigned long sleepStartTime = 0;

      if (preparingForSleep == false && wentToSleep == false)
      {
        Serial.println("Preparing to go to sleep in 10 seconds");
        sleepStartTime = millis();
        preparingForSleep = true;
        setTouchInterrupt(TOUCH_1_Seg_PIN,TOUCH_1_Seg_THRESHOLD_BAT);
        setTouchInterrupt(TOUCH_2_Seg_PIN,TOUCH_2_Seg_THRESHOLD_BAT);
        setTouchInterrupt(TOUCH_3_Seg_PIN,TOUCH_3_Seg_THRESHOLD_BAT);
        setTouchInterrupt(TOUCH_4_Seg_PIN,TOUCH_4_Seg_THRESHOLD_BAT);
        setTouchInterrupt(TOUCH_5_Seg_PIN,TOUCH_5_Seg_THRESHOLD_BAT);
      }

      if (preparingForSleep == true && wentToSleep == false)
      {
        if (millis() - sleepStartTime >= 10000)
        {
          if (powerConnected == true || inputDetected == true)
          {
            Serial.println("Power Connected or input detected, canceling sleep preparation");
            preparingForSleep = false;
          }
          else
          {
            Serial.println("Requesting Sleep");
            wentToSleep = true;
            syncESP32RTC();
            enableSleep();
          }
        }
      }
      checkPower();
      if (wentToSleep == true && powerConnected == false)
      {
        if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER)
        {
          Serial.println("Woke up from Timer");
          LedDisplay.clear();
          unsigned long startTime = millis();

          int sleepTime = TIMER_WAKUP_TIME;

          while (checkPower() == false && goToSleep == false)
          {
            vTaskDelay(pdMS_TO_TICKS(500));

            if (useAllButtons() != None || useAllTouch() != No_Seg)
            {
              setTouchInterrupt(TOUCH_1_Seg_PIN,TOUCH_1_Seg_THRESHOLD_BAT);
              setTouchInterrupt(TOUCH_2_Seg_PIN,TOUCH_2_Seg_THRESHOLD_BAT);
              setTouchInterrupt(TOUCH_3_Seg_PIN,TOUCH_3_Seg_THRESHOLD_BAT);
              setTouchInterrupt(TOUCH_4_Seg_PIN,TOUCH_4_Seg_THRESHOLD_BAT);
              setTouchInterrupt(TOUCH_5_Seg_PIN,TOUCH_5_Seg_THRESHOLD_BAT);
              esp_pm_config_t pm_config = {
                  .max_freq_mhz = 80,
                  .min_freq_mhz = 10,
                  .light_sleep_enable = true,
              };
              esp_pm_configure(&pm_config);
              inputDetected = true;
              manager.sendOledAction(OLED_ENABLE);
              vTaskResume(oledWakeupTaskHandle);
              vTaskResume(LedTask);
            }

            if (!checkPower() && !inputDetected && !ringing)
            {
              Serial.println("No input detected, going back to sleep...");
              LedDisplay.clear();
              vTaskDelay(pdMS_TO_TICKS(200));
              enableSleep();
              break;
            }
          }
        }

        if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TOUCHPAD && !goToSleep)
        {
          setTouchInterrupt(TOUCH_1_Seg_PIN,TOUCH_1_Seg_THRESHOLD_BAT);
          setTouchInterrupt(TOUCH_2_Seg_PIN,TOUCH_2_Seg_THRESHOLD_BAT);
          setTouchInterrupt(TOUCH_3_Seg_PIN,TOUCH_3_Seg_THRESHOLD_BAT);
          setTouchInterrupt(TOUCH_4_Seg_PIN,TOUCH_4_Seg_THRESHOLD_BAT);
          setTouchInterrupt(TOUCH_5_Seg_PIN,TOUCH_5_Seg_THRESHOLD_BAT);
          Serial.println("Woke up from touch button");
          vTaskDelay(pdMS_TO_TICKS(500));
          esp_pm_config_t pm_config = {
              .max_freq_mhz = 80,
              .min_freq_mhz = 10,
              .light_sleep_enable = true,
          };
          esp_pm_configure(&pm_config);
          inputDetected = true;
          manager.sendOledAction(OLED_ENABLE);
          vTaskResume(oledWakeupTaskHandle);
          vTaskResume(LedTask);

         while (checkPower() == false && goToSleep == false)
         {
             Serial.println("Touch while running uh " + String(inputDetected));
             vTaskDelay(pdMS_TO_TICKS(500));

           if (!checkPower() && !inputDetected && !ringing)
           {
             Serial.println("No input detected, going back to sleep...");
             LedDisplay.clear();
             vTaskDelay(pdMS_TO_TICKS(200));
             enableSleep();
             break;
           }
         }
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void controlCharger()
{
  float batteryVoltage = getBatteryVoltage();

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

  // Debugging: Always print battery voltage for monitoring
  Serial.print("Battery Voltage: ");
  Serial.print(batteryVoltage);
  Serial.print(" V - Charging: ");
  Serial.println(charging ? "ON" : "OFF");
}

// median

bool initializedSleep = false;

void initSleep()
{
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

  esp_sleep_enable_timer_wakeup(SLEEPING_TIME);

  touchSleepWakeUpEnable(TOUCH_1_Seg_PIN, TOUCH_1_Seg_THRESHOLD_SLEEP);
  touchSleepWakeUpEnable(TOUCH_2_Seg_PIN, TOUCH_2_Seg_THRESHOLD_SLEEP);
  touchSleepWakeUpEnable(TOUCH_3_Seg_PIN, TOUCH_3_Seg_THRESHOLD_SLEEP);
  touchSleepWakeUpEnable(TOUCH_4_Seg_PIN, TOUCH_4_Seg_THRESHOLD_SLEEP);
  touchSleepWakeUpEnable(TOUCH_5_Seg_PIN, TOUCH_5_Seg_THRESHOLD_SLEEP);

  setTouchInterrupt(TOUCH_1_Seg_PIN,TOUCH_1_Seg_THRESHOLD_SLEEP);
  setTouchInterrupt(TOUCH_2_Seg_PIN,TOUCH_2_Seg_THRESHOLD_SLEEP);
  setTouchInterrupt(TOUCH_3_Seg_PIN,TOUCH_3_Seg_THRESHOLD_SLEEP);
  setTouchInterrupt(TOUCH_4_Seg_PIN,TOUCH_4_Seg_THRESHOLD_SLEEP);
  setTouchInterrupt(TOUCH_5_Seg_PIN,TOUCH_5_Seg_THRESHOLD_SLEEP);
  delay(100);


  esp_sleep_enable_touchpad_wakeup();
  initializedSleep = true;
}

void enableSleep()
{
  goToSleep = true;
  listenToSleep();
}

void listenToSleep()
{
  if (goToSleep == true)
  {
    vTaskSuspend(oledWakeupTaskHandle);
    vTaskSuspend(LedTask);
    vTaskSuspend(dimmingTaskHandle);
    initSleep();
    Serial.println("Initialized sleep");
    Serial.println("Light Sleep");
    Serial.flush();
    Serial.println("Going to sleep");
    maxBrightness = false;
    inputDetected = false;
    delay(500);
    manager.sendOledAction(OLED_DISABLE);
    display.ssd1306_command(SSD1306_DISPLAYOFF); // Just to make sure because manager can take a bit before reacting if many write operations are ordered
    delay(500);
    LedDisplay.setBrightness(0);
    delay(100);
    LedDisplay.clear();
    esp_wifi_stop();
    initSleep();
    esp_err_t sleep_result = esp_light_sleep_start();
    Serial.println("Sleep aaa");

    // Check the result of the sleep request
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
    esp_wifi_start();
    goToSleep = false;
    initializedSleep = false;
    checkPower();
    delay(200);
  }
}

double readVoltage(byte pin)
{
  double reading = analogRead(VOLTAGE_DIVIDER_PIN);
  Serial.println("Raw adc reading" + String(reading));
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

  Serial.print("Battery voltage (median): ");
  Serial.println(batteryVoltage, 3); // 3 decimal places
  Serial.print("Raw Voltage Divider mV: ");
  Serial.println(miliVolts, 3);

  return batteryVoltage;
}

int getBatteryPercentage()
{
  int percentage = ((getBatteryVoltage() - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE)) * 100.00;
  percentage = min(percentage, 100);
  if (percentage < 0)
  {
    percentage = 0;
    return percentage;
  }
  else
    return percentage;
}