#ifndef INTERFACE_H
#define INTERFACE_H

#include "../defines.h"

void initMenus();
void handleMenus();
void showMenu();

void addNewAlarm();

bool shouldExitLoop();

extern bool menuRunning;
extern bool idleEnabled;
extern bool exitLoopFunction;

#endif
