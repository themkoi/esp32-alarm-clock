#ifndef ALARMS_H
#define ALARMS_H

#include "../defines.h"

void onAlarm();
void createAlarmTask();
void ringAlarm(void *parameter);

void disableAllAlarms();
void enableAllAlarms();  // <-- Added semicolon here

void initialzeAlarmArray();

struct Alarm {
    bool exists;
    bool enabled;
    bool days[7]; // One bool for each day (Monday to Sunday)
    int hours;
    int minutes;
    bool soundOn;
};

extern bool ringing;

extern Alarm alarms[MAX_ALARMS];  // Declaration of the array

#endif
