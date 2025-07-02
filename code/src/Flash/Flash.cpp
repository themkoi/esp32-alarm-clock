#include "Flash.h"

Preferences preferences;

void saveAlarms()
{
    File file = LittleFS.open("/alarms.dat", "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    uint8_t daysBuffer[7];
    uint8_t count = 0;

    for (int i = 0; i < MAX_ALARMS; ++i)
        if (alarms[i].exists)
            count++;

    if (file.write(count) != 1) {
        Serial.println("Failed to write count");
    }

    for (int i = 0; i < MAX_ALARMS; ++i)
    {
        if (alarms[i].exists)
        {
            if (file.write(alarms[i].enabled) != 1) Serial.println("Write error enabled");
            if (file.write((uint8_t *)&alarms[i].hours, sizeof(int16_t)) != sizeof(int16_t)) Serial.println("Write error hours");
            if (file.write((uint8_t *)&alarms[i].minutes, sizeof(int16_t)) != sizeof(int16_t)) Serial.println("Write error minutes");
            if (file.write(alarms[i].soundOn) != 1) Serial.println("Write error soundOn");
            if (file.write(alarms[i].lightOn) != 1) Serial.println("Write error lightOn");

            for (int d = 0; d < 7; d++)
                daysBuffer[d] = alarms[i].days[d] ? 1 : 0;
            if (file.write(daysBuffer, 7) != 7) Serial.println("Write error daysBuffer");
        }
    }

    file.flush();
    file.close();
    Serial.println("Alarms saved");
}

void readAlarms()
{
    File file = LittleFS.open("/alarms.dat", "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    if (file.available() < 1) {
        Serial.println("File empty");
        file.close();
        return;
    }

    uint8_t count = file.read();
    Serial.printf("Count read: %d\n", count);

    for (int i = 0; i < MAX_ALARMS; ++i)
        alarms[i].exists = false;

    for (int i = 0; i < count && i < MAX_ALARMS; ++i)
    {
        if (file.available() < 11) {
            Serial.println("Unexpected EOF");
            break;
        }

        alarms[i].exists = true;
        alarms[i].enabled = file.read();
        file.read((uint8_t *)&alarms[i].hours, sizeof(int16_t));
        file.read((uint8_t *)&alarms[i].minutes, sizeof(int16_t));
        alarms[i].soundOn = file.read();
        alarms[i].lightOn = file.read();

        uint8_t daysBuffer[7];
        file.read(daysBuffer, 7);
        for (int d = 0; d < 7; d++)
            alarms[i].days[d] = daysBuffer[d] != 0;
    }

    file.close();
    Serial.println("Alarms loaded");
}


void saveOtaValue(bool Value)
{
    preferences.begin("ota", false);

    preferences.putBool("ota_enabled", Value);
    preferences.end();
}

bool readOtaValue()
{
    preferences.begin("ota", false);
    bool OTAEnabled = preferences.getBool("ota_enabled", false);
    preferences.end();
    return OTAEnabled;
}
