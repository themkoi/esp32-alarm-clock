#include "defines.h"

timeval tv;

void setup()
{
  initHardware();
  initWifi();
  checkForRunOta();
  createLedDisplayTask();
  createDimmingTask();
  initialzeAlarmArray();
  readAlarms();
  createAlarmTask();
  setupScreensaver();
  initMenus();
  createBatteryTask();
  createTempTask();
  createTimeTask();
}

void loop()
{
  delay(100000);
}
