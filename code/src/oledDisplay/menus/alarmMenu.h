#ifndef ALARMMENU_H
#define ALARMMENU_H

#include "../../defines.h"

struct Submenu;

void initAlarmMenus();
Submenu *createAlarmsMenu();

void initManageAlarm();
void manageAlarms();

extern Submenu *alarmsSubmenu;
Submenu *createAlarmsMenu();

void disableAlarmsIn();
void enableAlarmsIn();
void readAlarmsIn();
void refreshAlarmsSubmenu();

#endif
