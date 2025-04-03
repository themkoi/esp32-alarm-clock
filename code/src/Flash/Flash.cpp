#include "Flash.h"

Preferences preferences;

void saveAlarms()
{
    preferences.begin("alarms", false);

    for (int i = 0; i < MAX_ALARMS; ++i)
    {
        preferences.putBool(("exists" + String(i)).c_str(), alarms[i].exists);
        preferences.putBool(("enabled" + String(i)).c_str(), alarms[i].enabled);
        preferences.putInt(("hours" + String(i)).c_str(), alarms[i].hours);
        preferences.putInt(("minutes" + String(i)).c_str(), alarms[i].minutes);
        preferences.putBool(("soundOn" + String(i)).c_str(), alarms[i].soundOn);
        
        // Save the days array
        for (int day = 0; day < 7; ++day) {
            preferences.putBool(("day" + String(i) + "_" + String(day)).c_str(), alarms[i].days[day]);
        }

        Serial.print("Saved Alarm ");
        Serial.print(i);
        Serial.print(": exists=");
        Serial.print(alarms[i].exists);
        Serial.print(", enabled=");
        Serial.print(alarms[i].enabled);
        Serial.print(", hours=");
        Serial.print(alarms[i].hours);
        Serial.print(", minutes=");
        Serial.print(alarms[i].minutes);
        Serial.print(", soundOn=");
        Serial.print(alarms[i].soundOn);
        
        // Print saved days
        Serial.print(", days=");
        for (int day = 0; day < 7; ++day) {
            Serial.print(alarms[i].days[day] ? "1" : "0");
            if (day < 6) {
                Serial.print(",");
            }
        }
        Serial.println();
    }

    preferences.end();
}

void readAlarms()
{
    preferences.begin("alarms", false);

    for (int i = 0; i < MAX_ALARMS; ++i)
    {
        alarms[i].exists = preferences.getBool(("exists" + String(i)).c_str(), false);
        alarms[i].enabled = preferences.getBool(("enabled" + String(i)).c_str(), false);
        alarms[i].hours = preferences.getInt(("hours" + String(i)).c_str(), 0);
        alarms[i].minutes = preferences.getInt(("minutes" + String(i)).c_str(), 0);
        alarms[i].soundOn = preferences.getBool(("soundOn" + String(i)).c_str(), false);

        // Read the days array
        for (int day = 0; day < 7; ++day) {
            alarms[i].days[day] = preferences.getBool(("day" + String(i) + "_" + String(day)).c_str(), false);
        }

        Serial.print("Alarm ");
        Serial.print(i);
        Serial.print(": exists=");
        Serial.print(alarms[i].exists);
        Serial.print(", enabled=");
        Serial.print(alarms[i].enabled);
        Serial.print(", hours=");
        Serial.print(alarms[i].hours);
        Serial.print(", minutes=");
        Serial.print(alarms[i].minutes);
        Serial.print(", soundOn=");
        Serial.print(alarms[i].soundOn);
        
        // Print loaded days
        Serial.print(", days=");
        for (int day = 0; day < 7; ++day) {
            Serial.print(alarms[i].days[day] ? "1" : "0");
            if (day < 6) {
                Serial.print(",");
            }
        }
        Serial.println();
    }

    preferences.end();
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