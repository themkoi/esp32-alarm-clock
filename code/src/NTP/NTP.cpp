#include "NTP.h"
#include <Timezone.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
TaskHandle_t NTPTask;

// Define Timezone rules for Poland (Central European Time)
TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, 120}; // UTC+2
TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, 60};  // UTC+1
Timezone myTZ(myDST, mySTD);
TimeChangeRule *tcr;

void syncTimeTask(void *parameter);

void createTimeTask()
{
  xTaskCreatePinnedToCore(
      syncTimeTask,   
      "SyncTimeTask", 
      10000,          
      NULL,           
      1,              
      &NTPTask,       
      1               
  );
}

void deleteTimeTask()
{
  vTaskDelete(NTPTask);
}

void syncTimeLibWithRTC() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  setTime(tv.tv_sec);
  Serial.println("TimeLib synchronized with internal RTC");
}

void syncESP32RTC()
{
  time_t now = ::now();
  struct timeval tv;
  tv.tv_sec = now;
  tv.tv_usec = 0;
  settimeofday(&tv, NULL);
  Serial.println("Internal RTC synchronized with TimeLib");
}

void syncTimeTask(void *parameter)
{
  while (true)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      synchronizeAndSetTime();
    }
    else
    {
      Serial.println("WiFi not connected. Cannot sync time.");
    }
    vTaskDelay(pdMS_TO_TICKS(10 * 60 * 1000));
  }
}

void synchronizeAndSetTime()
{
  Serial.println("Synchronizing Time");
  timeClient.begin();
  timeClient.update();
  
  time_t utc = timeClient.getEpochTime();
  time_t local = myTZ.toLocal(utc, &tcr);
  
  setTime(local);
  timeClient.end();
  syncESP32RTC();
  Serial.println("Current time: " + String(hour()) + ":" + String(minute()) + " " + tcr->abbrev);
}
