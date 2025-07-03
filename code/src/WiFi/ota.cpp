#include "ota.h"

void checkForRunOta()
{
    if (digitalRead(UP_PIN) == LOW || readOtaValue() == true)
    {
        setCpuFrequencyMhz(240); // stable 160,80,240
        Serial.println("button is pressed or ota enabled from last boot");
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
}