#include "Flash.h"

Preferences preferences;

void saveAlarms() {
    File file = LittleFS.open("/alarms.dat", "w");
    if (!file) return;

    uint8_t daysBuffer[7];  // outside loop

    for (int i = 0; i < MAX_ALARMS; ++i) {
        file.write(alarms[i].exists);
        file.write(alarms[i].enabled);
        file.write((uint8_t *)&alarms[i].hours, sizeof(int16_t));
        file.write((uint8_t *)&alarms[i].minutes, sizeof(int16_t));
        file.write(alarms[i].soundOn);
        file.write(alarms[i].lightOn);

        for (int d = 0; d < 7; d++) daysBuffer[d] = alarms[i].days[d] ? 1 : 0;
        file.write(daysBuffer, 7);
        delay(1);
    }

    file.close();
}


void readAlarms() {
    File file = LittleFS.open("/alarms.dat", "r");
    if (!file) return;

    for (int i = 0; i < MAX_ALARMS; ++i) {
        if (file.available() < 12) break;

        alarms[i].exists = file.read();
        alarms[i].enabled = file.read();
        file.read((uint8_t *)&alarms[i].hours, sizeof(int16_t));
        file.read((uint8_t *)&alarms[i].minutes, sizeof(int16_t));
        alarms[i].soundOn = file.read();
        alarms[i].lightOn = file.read();

        uint8_t daysBuffer[7];
        file.read(daysBuffer, 7);
        for (int d = 0; d < 7; d++) alarms[i].days[d] = daysBuffer[d] != 0;
    }

    file.close();
}


void saveOtaValue(bool Value)
{
    preferences.begin("Ota", false);

    preferences.putBool("Ota Enabled", Value);
    preferences.end();
}

void readOtaValue()
{
    preferences.begin("Ota", false);

    OTAEnabled = preferences.getBool("Ota Enabled", false);
    preferences.end();
}