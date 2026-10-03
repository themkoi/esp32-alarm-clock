#include "battery.h"
#include "rtcMem/rtcMem.h"
#include "esp_pm.h"
#include "esp_wifi.h"

int batteryPercentage;
float batteryVoltage;

bool powerConnected = false;
bool charging = false;
bool wentToSleep = false;

bool batterySleepMode = false;
bool wokeUp = true;

TaskHandle_t batteryTaskHandle = NULL;

void enableSleep();
void initSleep();
void enableAllSensors();
void disableAllSensors();
void manageBattery(void *parameter);
void controlCharger();
void wakeUpAndRestoreState();

void enableAllSensors()
{
  enableColorSensor();
  enableLightSensor();
  enablePressureSensor();
  enableTempSensor();
  setTouchNormalPower();
  rM.gpioExpander.setDefaultPinStates();
}

void disableAllSensors()
{
  disableColorSensor();
  disableLightSensor();
  disablePressureSensor();
  disableTempSensor();
  setTouchLowPower();
  rM.gpioExpander.enterLowPowerState();
}

void wakeUpAndRestoreState()
{
  enableAllSensors();

  vTaskResume(TimeTask);
  vTaskResume(alarmTaskHandle);

  if (!ringing)
  {
    vTaskResume(menuTaskHandle);
  }

  oledMana.enable();
  enableLedDisplay();

  wokeUp = true;
}

void createBatteryTask()
{
  xTaskCreate(
      manageBattery,
      "Battery",
      4096,
      NULL,
      3,
      &batteryTaskHandle);
}

bool checkPower()
{
  int chargingState = rM.gpioExpander.digitalRead(MCP_5V);

  Serial.print("Charging State: ");
  Serial.println(chargingState);

  powerConnected = (chargingState == HIGH);

  return powerConnected;
}

void manageBattery(void *parameter)
{
  const unsigned long batCheInterval = 15000;
  const unsigned long batteryWaitTimeout = 3000;

  unsigned long lastRunBatChe = 0;
  unsigned long batterySettingsTime = 0;
  unsigned long wakeupTime = 0;
  unsigned long initialWakeupTime = 0;

  bool setPowerSettings = true;
  bool waitingForPower = false;
  bool waitForInput = false;

  powerConnected = checkPower();
  batteryVoltage = getBatteryVoltage();
  controlCharger();

  while (true)
  {
    ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));

    unsigned long now = millis();

    if (now - lastRunBatChe >= batCheInterval)
    {
      lastRunBatChe = now;

      batteryVoltage = getBatteryVoltage();

      if (powerConnected)
      {
        controlCharger();
      }
    }

    if (powerConnected)
    {
      if (!setPowerSettings)
      {
        Serial.println("Setting power settings (AC Mode)");

        batterySleepMode = false;
        waitingForPower = false;
        waitForInput = false;
        setPowerSettings = true;

        wakeUpAndRestoreState();
        vTaskResume(oledWakeupTaskHandle);
        vTaskResume(dimmingTaskHandle);

        esp_wifi_start();
      }

      if (!WiFi.isConnected() && !WifiTaskRunning)
      {
        esp_wifi_start();

        Serial.println("Launching WiFi task");

        createWifiTask();
      }

      continue;
    }

    if (setPowerSettings)
    {
      Serial.println("Setting battery settings (Low-Power DFS Mode)");

      turnOffWifi();
      esp_wifi_stop();
      esp_wifi_deinit();

      rM.gpioExpander.setPinState(
          MCP_CHARGER_CONTROL_PIN,
          false);

      batterySettingsTime = now;

      waitingForPower = true;
      setPowerSettings = false;
      batterySleepMode = false;
      waitForInput = false;

      maxBrightness = false;
      inputDetected = false;
    }

    if (waitingForPower)
    {
      bool input =
          (useAllButtons() != None) ||
          useAllTouch().touched ||
          inputDetected ||
          ringing;

      if (input)
      {
        waitForInput = true;
        inputDetected = false;

        wakeUpAndRestoreState();

        setLedIntensity(2);
        showCurrentTime();

        batterySettingsTime = now;
      }

      if (now - batterySettingsTime >= batteryWaitTimeout)
      {
        waitingForPower = false;
        batterySleepMode = true;

        Serial.println("Battery mode active");

        syncESP32RTC();

        bool pendingInput =
            (useAllButtons() != None) ||
            useAllTouch().touched ||
            inputDetected ||
            ringing;

        if (!pendingInput)
        {
          enableSleep();

          now = millis();
          wakeupTime = now;
          initialWakeupTime = now;
          waitForInput = false;

          continue;
        }

        wakeupTime = now;
        initialWakeupTime = now;
        waitForInput = true;
      }
    }

    if (!batterySleepMode)
    {
      continue;
    }

    now = millis();

    bool input =
        (useAllButtons() != None) ||
        useAllTouch().touched ||
        inputDetected ||
        ringing;

    if (input)
    {
      inputDetected = false;

      if (!waitForInput)
      {
        Serial.println("Input detected during battery mode");

        wakeUpAndRestoreState();

        waitForInput = true;

        setLedIntensity(2);
        showCurrentTime();
      }

      wakeupTime = now;
    }

    if (now - initialWakeupTime >= MAX_AWAKE_HARD_LIMIT)
    {
      Serial.println("Hard 5-minute awake limit reached");

      waitForInput = false;

      enableSleep();

      now = millis();
      wakeupTime = now;
      initialWakeupTime = now;

      continue;
    }

    if (!waitForInput)
    {
      if (now - wakeupTime >= TIMER_WAKUP_TIME)
      {
        Serial.println("No input with timer, going back to sleep");

        enableSleep();

        now = millis();
        wakeupTime = now;
        initialWakeupTime = now;

        continue;
      }
    }
    else
    {
      if (now - wakeupTime >= GPIO_WAKUP_TIME)
      {
        Serial.println("No input with active session, going back to sleep");

        waitForInput = false;

        enableSleep();

        now = millis();
        wakeupTime = now;
        initialWakeupTime = now;

        continue;
      }
    }
  }
}

void controlCharger()
{
  bool newChargingState = charging;

  if (charging && batteryVoltage >= BATT_TARGET_VOLTAGE)
  {
    newChargingState = false;

    Serial.println("Charging stopped (target voltage reached).");
  }
  else if (!charging &&
           batteryVoltage <= (BATT_TARGET_VOLTAGE - BATT_HYSTERESIS))
  {
    newChargingState = true;

    Serial.println("Charging started (voltage dropped).");
  }

  if (newChargingState != charging)
  {
    charging = newChargingState;

    if (charging)
    {
      rM.gpioExpander.setPinPullUp(
          MCP_CHARGER_CONTROL_PIN,
          true);
    }
    else
    {
      rM.gpioExpander.setPinPullUp(
          MCP_CHARGER_CONTROL_PIN,
          false);
    }

    rM.gpioExpander.setPinState(
        MCP_CHARGER_CONTROL_PIN,
        charging);
  }

  Serial.print("Battery Voltage: ");
  Serial.print(batteryVoltage);
  Serial.print(" V - Charging: ");
  Serial.println(charging ? "ON" : "OFF");
}

uint64_t pinToMask(uint8_t pin)
{
  return ((uint64_t)1 << pin);
}

void initSleep()
{
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

  esp_sleep_enable_ext1_wakeup(
      (1ULL << TOUCH_INTERRUPT) |
          (1ULL << MCP_INTERRUPT_PIN),
      ESP_EXT1_WAKEUP_ANY_LOW);

  long secondsToNextAlarm = getTimeUntilNextAlarm();

  if (secondsToNextAlarm > 0)
  {
    uint64_t sleepMicros =
        (uint64_t)secondsToNextAlarm * 1000000ULL;

    esp_sleep_enable_timer_wakeup(sleepMicros);

    Serial.printf(
        "Timer wakeup set for next alarm in %ld seconds.\n",
        secondsToNextAlarm);
  }
  else
  {
    Serial.println("No active alarms found. GPIO wake only.");
  }
}

void enableSleep()
{
  bool pendingInput =
      (useAllButtons() != None) ||
      useAllTouch().touched ||
      inputDetected ||
      ringing;

  if (pendingInput)
  {
    Serial.println("Sleep cancelled: input detected");
    wakeUpAndRestoreState();
    return;
  }
  if (!oledMana.dimmed)
  {
    oledMana.fadeOut();
  }

  oledMana.disable();
  disableLedDisplay();
  disableAllSensors();

  initSleep();

  wokeUp = false;

  vTaskSuspend(oledWakeupTaskHandle);
  vTaskSuspend(TimeTask);
  vTaskSuspend(dimmingTaskHandle);
  vTaskSuspend(alarmTaskHandle);
  vTaskSuspend(menuTaskHandle);

  pendingInput =
      (useAllButtons() != None) ||
      useAllTouch().touched ||
      inputDetected ||
      ringing;

  if (pendingInput)
  {
    wakeUpAndRestoreState();
    return;
  }

  Serial.println("Entering light sleep...");

  esp_err_t result = esp_light_sleep_start();

  wokeUp = true;

  if (result != ESP_OK)
  {
    Serial.printf(
        "Light sleep failed: %d\n",
        result);

    wakeUpAndRestoreState();
    return;
  }

  esp_sleep_wakeup_cause_t cause =
      esp_sleep_get_wakeup_cause();

  if (cause == ESP_SLEEP_WAKEUP_TIMER)
  {
    Serial.println("Light sleep wake: TIMER");
  }
  else if (cause == ESP_SLEEP_WAKEUP_EXT1)
  {
    uint64_t pins =
        esp_sleep_get_ext1_wakeup_status();

    Serial.printf(
        "Light sleep wake: EXT1 0x%llX\n",
        pins);
  }
  else
  {
    Serial.printf(
        "Light sleep wake: cause %d\n",
        cause);
  }

  wakeUpAndRestoreState();

  syncTimeLibWithRTC();
  checkAlarms();

  inputDetected = false;

  delay(200);
}

double readVoltage(byte pin)
{
  int reading = analogRead(pin);

  constexpr double c4 = -1.6000000000000e-14;
  constexpr double c3 = 1.1817100000000e-10;
  constexpr double c2 = -3.0121169100000e-07;
  constexpr double c1 = 1.1090192717940e-03;
  constexpr double c0 = 3.4143524634089e-02;

  double voltage =
      (((c4 * reading + c3) * reading + c2) * reading + c1) *
          reading +
      c0;

  return voltage * 1000.0;
}

float getBatteryVoltage()
{
  double milliVolts = readVoltage(VOLTAGE_DIVIDER_PIN);

  Serial.print("milliVolts = ");
  Serial.println(milliVolts);

  milliVolts += ADC_OFFSET;

  float batteryVoltage =
      milliVolts / ADC_VOLTAGE_DIVIDER;

  Serial.print("batteryVoltage = ");
  Serial.println(batteryVoltage, 6);

  return batteryVoltage;
}

int getBatteryPercentage()
{
  int percentage =
      ((batteryVoltage - MIN_VOLTAGE) /
       (MAX_VOLTAGE - MIN_VOLTAGE)) *
      100.0;

  percentage = min(percentage, 100);

  if (percentage < 0)
  {
    percentage = 0;
  }

  return percentage;
}
