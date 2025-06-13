#ifndef ALARMS_H
#define ALARMS_H

#include "../defines.h"

void createAlarmTask();
void ringAlarm(void *parameter);

void disableAllAlarms();
void enableAllAlarms();

void initialzeAlarmArray();

void checkAlarms();

struct Alarm {
    bool exists;
    bool enabled;
    bool days[7]; // One bool for each day (Monday to Sunday)
    int hours;
    int minutes;
    bool soundOn;
};

extern bool ringing;

extern TaskHandle_t alarmTaskHandle;

extern Alarm alarms[MAX_ALARMS];

#endif
