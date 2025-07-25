#ifndef CHARTS_H
#define CHARTS_H

#include "../../defines.h"

void loopTempGraph();
void initTempGraph();

void initHumidityGraph();
void loopHumidityGraph();

void loopLightGraph();
void initLightGraph();

extern float temperatureArray[CHART_READINGS];
extern float humidityArray[CHART_READINGS];
extern float lightArray[CHART_READINGS];

#endif
