#include "defines.h"

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
  createLightTask();
  createTimeTask();
}

void loop()
{
  delay(100000);
}
