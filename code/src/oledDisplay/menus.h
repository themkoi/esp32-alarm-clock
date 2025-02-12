#ifndef MENUS_H
#define MENUS_H

#include "defines.h"

void currentWeather();
void displayWeatherCast(int dayIndex);

String convertWindDirection(uint16_t degrees);

void checkExit();

void initWeatherMenu();
void currentWeatherMenu();
void wifiDebugMenu();
void CPUDebugMenu();
void generalDebugMenu();
void touchDebugMenu();
void fpsCalc();
#endif
