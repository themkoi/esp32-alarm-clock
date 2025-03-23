#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include "../defines.h"

void createDimmingTask();

void dimLedDisplay();

float getLightLevel();
int smoothTouchRead(int pin);

bool checkForNight();

int getLightState();

extern TaskHandle_t dimmingTaskHandle;
extern bool dimmingTaskRunning;
extern bool maxBrightness;
extern bool displayON;

extern bool inputDetected;
extern float lightLevel;


#endif
