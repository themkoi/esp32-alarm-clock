#include "hardware.h"

TM1637Display LedDisplay = TM1637Display(CLK, DIO);
LTR_F216A lightMeter;

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

void initOledDisplay();
void initLedDisplay();
void initLightSensor();
void initBuzzer();
void initButtons();
void initTempSensor();
void initTouch();
void mountLittlefs();

void initHardware()
{
  Serial.begin(115200);
  Serial.println("Initializing Hardware");
  setCpuFrequencyMhz(80); // stable 160,80,240 needs to be 80 for wifi
  esp_pm_config_t pm_config = {
      .max_freq_mhz = 80,
      .min_freq_mhz = 10,
      .light_sleep_enable = true,
  };
  esp_pm_configure(&pm_config);
  pinMode(VOLTAGE_DIVIDER_PIN, INPUT);
  pinMode(POWER_STATE_PIN, INPUT);
  pinMode(HALL_SWITCH, INPUT);
  pinMode(CHARGER_CONTROL_PIN, OUTPUT);
  initButtons();
  initTouch();
  initBuzzer();
  initOledDisplay();
  initLedDisplay();
  initLightSensor();
  initTempSensor();
  syncTimeLibWithRTC();
  mountLittlefs();
  Serial.println("Hardware initialized");
}

void mountLittlefs()
{
  if (!LittleFS.begin())
  {
    Serial.println("LittleFS mount failed, formatting...");
    if (!LittleFS.format())
    {
      Serial.println("LittleFS format failed");
    }
    if (!LittleFS.begin())
    {
      Serial.println("LittleFS mount failed after format continue and see what happens :3");
    }
  }
  else
  {
    Serial.println("LittleFS mounted successfully");
  }
}

bool readHallSwitch()
{
  int pinState = digitalRead(HALL_SWITCH); // Read the pin state
  return (pinState == LOW);                // Return true if LOW, false if HIGH
}

void initOledDisplay()
{
  manager.createTask();
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  manager.sendOledAction(OLED_DISPLAY);

  centerText("Oled Initialized", SCREEN_HEIGHT / 2);
  centerText((resetReasonToString(esp_reset_reason())), 10);

  manager.sendOledAction(OLED_DISPLAY);
  manager.sendOledAction(OLED_ENABLE);
  display.ssd1306_command(0x81);
  display.ssd1306_command(130);
  delay(100);
  display.ssd1306_command(0xD9);
  display.ssd1306_command(30);
  manager.sendOledAction(OLED_FADE_OUT);
  Serial.println("OLed display initialized");
}

void initLedDisplay()
{
  LedDisplay.setBrightness(0);
  LedDisplay.showNumberDecEx(8888, 0b11100000);
  Serial.println("Led display initialized");
}

void initLightSensor()
{
  lightMeter.begin();
  lightMeter.setActiveMode();
  lightMeter.setGain(0x04);
  lightMeter.configureMeasurement(0x00, 0x02);
}

void initTouch()
{
  touchSetCycles(0x500, 0x500);
  turnOnTouch();
}

void initButtons()
{
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(MENU_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
  turnOnButtons();
  Serial.println("Buttons initialized");
}

int melody[] = {
    NOTE_C7, NOTE_G7, NOTE_C8};

int noteDurations[] = {
    16, 11, 16};

void initBuzzer()
{
  pinMode(BUZZER_PIN, OUTPUT);
#ifdef START_SOUND
  Serial.println("Buzzer initialized");
  for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++)
  {
    int noteDuration = 1000 / noteDurations[i];
    tone(BUZZER_PIN, melody[i], noteDuration);

    noTone(BUZZER_PIN);
  }
#endif
}

void initTempSensor()
{
  if (!sht4.begin())
  {
    Serial.println("Couldn't find SHT4x");
  }
  Serial.println("Found SHT4x sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht4.readSerial(), HEX);

  // You can have 3 different precisions, higher precision takes longer
  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  switch (sht4.getPrecision())
  {
  case SHT4X_HIGH_PRECISION:
    Serial.println("High precision");
    break;
  case SHT4X_MED_PRECISION:
    Serial.println("Med precision");
    break;
  case SHT4X_LOW_PRECISION:
    Serial.println("Low precision");
    break;
  }

  // You can have 6 different heater settings
  // higher heat and longer times uses more power
  // and reads will take longer too!
  sht4.setHeater(SHT4X_NO_HEATER);
  switch (sht4.getHeater())
  {
  case SHT4X_NO_HEATER:
    Serial.println("No heater");
    break;
  case SHT4X_HIGH_HEATER_1S:
    Serial.println("High heat for 1 second");
    break;
  case SHT4X_HIGH_HEATER_100MS:
    Serial.println("High heat for 0.1 second");
    break;
  case SHT4X_MED_HEATER_1S:
    Serial.println("Medium heat for 1 second");
    break;
  case SHT4X_MED_HEATER_100MS:
    Serial.println("Medium heat for 0.1 second");
    break;
  case SHT4X_LOW_HEATER_1S:
    Serial.println("Low heat for 1 second");
    break;
  case SHT4X_LOW_HEATER_100MS:
    Serial.println("Low heat for 0.1 second");
    break;
  }
}