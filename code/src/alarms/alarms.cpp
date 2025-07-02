#include "alarms.h"
#include <HTTPClient.h>

Alarm alarms[MAX_ALARMS];

TaskHandle_t alarmTaskHandle;

void ringAlarm(void *parameter);
void createRiningingTask();

void sendOnPostRequest();
void sendOffPostRequest();
void sendTogglePostRequest();

void checkAlarm(int index);
void checkAlarmsTask(void *pvParameters);

void initialzeAlarmArray()
{
  for (int i = 0; i < MAX_ALARMS; i++)
  {
    alarms[i] = {
        // lets not have random data there
        false,                                      // exists
        false,                                      // enabled
        {true, true, true, true, true, true, true}, // days
        0,                                          // hours
        0,                                          // minutes
        true,                                       // soundOn
        true                                        // lightOn
    };
  }
}

void createAlarmTask()
{
  xTaskCreatePinnedToCore(
      checkAlarmsTask,  // Function to implement the task
      "Alarms",         // Task name
      2048,             // Stack size (words)
      NULL,             // Task input parameter
      1,                // Priority (0 is lowest)
      &alarmTaskHandle, // Task handle
      0                 // Core to run the task on (0 or 1)
  );
}

void checkAlarms()
{
  int currentDay = weekday() - 1; // Adjust to 0-based index
  Serial.println("current day:" + String(currentDay));

  for (int i = 0; i < MAX_ALARMS; ++i)
  {
    if (alarms[i].days[currentDay] && alarms[i].enabled && alarms[i].exists)
    {
      checkAlarm(i);
    }
  }
}

void checkAlarmsTask(void *pvParameters)
{
  while (true)
  {
    checkAlarms();
    vTaskDelay(pdMS_TO_TICKS(45 * 1000));
  }
}

void disableAllAlarms()
{
  for (int i = 0; i < MAX_ALARMS; i++)
  {
    alarms[i].enabled = false;
  }
}

void enableAllAlarms()
{
  for (int i = 0; i < MAX_ALARMS; i++)
  {
    alarms[i].enabled = true;
  }
}

int lastRingingMinute = -1;
int lastRingingHour = -1;
bool ringing = false;

bool buzzerEnabled = false;
bool lightCtrlEnabled = false;

void checkAlarm(int index)
{
  int currentHours = hour();
  int currentMinutes = minute();
  Serial.println("Check Alarm Function");

  if (currentHours == alarms[index].hours && currentMinutes == alarms[index].minutes && (ringing == false || lastRingingMinute != currentMinutes || lastRingingHour != currentHours))
  {
    Serial.print("Alarm! It's time to wake up on ");
    buzzerEnabled = alarms[index].soundOn;
    lightCtrlEnabled = alarms[index].lightOn;
    createRiningingTask();

    lastRingingMinute = currentMinutes;
    lastRingingHour = currentHours;
    ringing = true;
  }
}

TaskHandle_t Alarm;

void createRiningingTask()
{
  xTaskCreatePinnedToCore(
      ringAlarm,   // Function to implement the task
      "ringAlarm", // Name of the task
      2048,        // Stack size (words)
      NULL,        // Parameter to pass
      2,           // Priority
      &Alarm,      // Task handle
      0            // Core to run the task on (Core 0)
  );
}

static unsigned long alarmStartTime = 0;

void touchStopAlarm(int hour, bool ringOn, bool lightOn)
{
  if (readHallSwitch() == true || (millis() - alarmStartTime >= 30 * 60 * 1000))
  {
    if ((!(hour >= 11 && hour <= 21) || ringOn == false) && lightOn == true)
    {
      Serial.println("stopping alarm");
      if (lightOn && WiFi.status() == WL_CONNECTED)
      {
        vTaskDelay(pdMS_TO_TICKS(5 * 60 * 1000));
        sendOffPostRequest();
      }
    }
    ringing = false;
    vTaskDelete(Alarm);
  }
}

void ringAlarm(void *parameter)
{
  unsigned long startTime = millis();
  bool ringOn = buzzerEnabled;
  bool lightOn = lightCtrlEnabled;

  int currentHour = hour();
  int currentMinute = minute();

  int alarmMelody[] = {NOTE_C7, NOTE_C7, NOTE_B7, NOTE_B7, NOTE_B7, NOTE_B7, NOTE_G6, NOTE_G6};
  int alarmDurations[] = {8, 8, 8, 12, 12, 12, 8, 8};
  alarmStartTime = millis();

  if ((!(currentHour >= 11 && currentHour <= 21) || ringOn == false) && lightOn == true)
  {
    sendOnPostRequest();
  }
  Serial.println("Starting Alarm");

  unsigned long lastToggleRequestTime = startTime;

  while (true)
  {
    if ((millis() - startTime >= 15000 || WiFi.SSID() != SSID1 || WiFi.status() != WL_CONNECTED || (currentHour >= 11 && currentHour <= 21)) && ringOn == true)
    {
      Serial.println("ringin Alarm");

      for (int i = 0; i < sizeof(alarmMelody) / sizeof(alarmMelody[0]); i++)
      {
        tone(BUZZER_PIN, alarmMelody[i], 1000 / alarmDurations[i]);
        delay(1000 / alarmDurations[i] * 1.30);
        noTone(BUZZER_PIN);
      }
    }

    if (millis() - startTime >= 900000)
    {
      if (millis() - lastToggleRequestTime >= 10000)
      {
        sendTogglePostRequest();
        lastToggleRequestTime = millis();
      }
    }

    touchStopAlarm(currentHour, ringOn, lightOn);
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void sendOnPostRequest()
{
  if ((WiFi.status() == WL_CONNECTED))
  { // Check WiFi connection status
    if (WiFi.SSID() == SSID1)
    {

      HTTPClient http;

      http.begin(LIGHT_GATE);                             // Specify destination for HTTP request
      http.addHeader("Content-Type", "application/json"); // Specify content-type header

      int httpResponseCode = http.POST("{\"state\": \"ON\", \"transition\": 300}"); // Send the actual POST request

      if (httpResponseCode > 0)
      {
        String response = http.getString(); // Get the response to the request
        Serial.println(httpResponseCode);   // Print return code
        Serial.println(response);           // Print request answer
      }
      else
      {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end(); // Free resources
    }
  }
}

void sendOffPostRequest()
{
  if ((WiFi.status() == WL_CONNECTED))
  { // Check WiFi connection status
    if (WiFi.SSID() == SSID1)
    {
      HTTPClient http;

      http.begin(LIGHT_GATE);                             // Specify destination for HTTP request
      http.addHeader("Content-Type", "application/json"); // Specify content-type header

      int httpResponseCode = http.POST("{\"state\": \"OFF\", \"transition\": 30}"); // Send the actual POST request

      if (httpResponseCode > 0)
      {
        String response = http.getString(); // Get the response to the request
        Serial.println(httpResponseCode);   // Print return code
        Serial.println(response);           // Print request answer
      }
      else
      {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end(); // Free resources
    }
  }
}

void sendTogglePostRequest()
{
  if ((WiFi.status() == WL_CONNECTED))
  { // Check WiFi connection status
    if (WiFi.SSID() == SSID1)
    {
      HTTPClient http;

      http.begin(LIGHT_GATE);                             // Specify destination for HTTP request
      http.addHeader("Content-Type", "application/json"); // Specify content-type header

      int httpResponseCode = http.POST("{\"command\": \"toggle\"}"); // Send the actual POST request

      if (httpResponseCode > 0)
      {
        String response = http.getString(); // Get the response to the request
        Serial.println(httpResponseCode);   // Print return code
        Serial.println(response);           // Print request answer
      }
      else
      {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end(); // Free resources
    }
  }
}