#include "defines.h"
#include <ArduinoOTA.h>

bool OTAEnabled = false;

timeval tv;

void setup()
{
  Serial.begin(115200);
  initHardware();
  Serial.println("Initializing Hardware");

  if (!LittleFS.begin())
  {
    Serial.println("LittleFS mount failed, formatting...");
    if (!LittleFS.format())
    {
      Serial.println("LittleFS format failed");
    }
    if (!LittleFS.begin())
    {
      Serial.println("LittleFS mount failed after format");
    }
  }
  Serial.println("LittleFS mounted successfully");

  WiFi.mode(WIFI_STA);
  initWifi();
  readOtaValue();

  syncTimeLibWithRTC();
  LedDisplay.showNumberDecEx(hour() * 100 + minute(), 0b11100000, true);

  int upinkButtonStates = digitalRead(BUTTON_UP_PIN);
  if (upinkButtonStates == LOW || OTAEnabled == true)
  {
    setCpuFrequencyMhz(240); // stable 160,80,240
    Serial.println("button is pressed, enabling OTA");
    OTAEnabled = true;
    createWifiTask();
    display.clearDisplay();
    centerText("Connecting To WiFi", 30);
    centerText("Starting OTA", 40);
    display.display();
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(100);
    }
    Serial.println("Enabled OTA");
    ArduinoOTA.begin();
    display.clearDisplay();
    centerText("OTA:Enabled!", 10);
    centerText("IP Address:", 20);
    centerText(WiFi.localIP().toString(), 30);
    display.display();
    String ipAddress = WiFi.localIP().toString();

    String lastTwoDigits = ipAddress.substring(ipAddress.length() - 2);

    int lastTwoDigitsInt = lastTwoDigits.toInt();

    LedDisplay.showNumberDec(lastTwoDigitsInt);

    saveOtaValue(false);

    while (true)
    {
      display.clearDisplay();
      centerText("OTA:Enabled!", 10);
      centerText("IP Address:", 20);
      centerText(WiFi.localIP().toString(), 30);
      centerText("Running", 40);
      manager.sendOledAction(OLED_DISPLAY);
      ArduinoOTA.handle();
      display.clearDisplay();
      centerText("OTA:Enabled!", 10);
      centerText("IP Address:", 20);
      centerText(WiFi.localIP().toString(), 30);
      manager.sendOledAction(OLED_DISPLAY);
    }
    manager.sendOledAction(OLED_DISABLE);
  }
  if (!OTAEnabled)
  {
    initialzeAlarmArray();
    readAlarms();
    createLedDisplayTask();
    createDimmingTask();
    createBatteryTask();
    createTempTask();
    createTimeTask();
    display.setFont(&DejaVu_LGC_Sans_Bold_10);
    setupScreensaver();
    createAlarmTask();
  }
  initMenus();
  delay(100);
}

void loop()
{
  delay(100000);
}
