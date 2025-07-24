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
  int previousHour = hour();
  int previousMinute = minute();
  while (true)
  {
    int currentHour = hour();
    int currentMinute = minute();

    if (currentHour != previousHour || currentMinute != previousMinute)
    {
      previousHour = currentHour;
      previousMinute = currentMinute;
      checkAlarms();
    }
    vTaskDelay(pdMS_TO_TICKS(30 * 1000));
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
      4,           // Priority
      &Alarm,      // Task handle
      1            // Core to run the task on (Core 0)
  );
}

void touchStopAlarm(int hour, bool ringOn, bool lightOn, unsigned long startTime)
{
  if (readHallSwitch() == true || (millis() - startTime >= 30 * 60 * 1000))
  {
    if (lightOn == true)
    {
      Serial.println("stopping alarm");
      if (WiFi.status() == WL_CONNECTED)
      {
        vTaskDelay(pdMS_TO_TICKS(5 * 60 * 1000));
        sendOffPostRequest();
      }
    }
    ringing = false;
    vTaskDelete(Alarm);
  }
}

struct Melody
{
  const int *notes;
  const int *durations;
  int length;
};

void playMelody(const Melody &m)
{
  for (int i = 0; i < m.length; i++)
  {
    int duration = 1000 / m.durations[i];
    tone(BUZZER_PIN, m.notes[i], duration);
    delay(duration * 1.3);
    noTone(BUZZER_PIN);
  }
}

void ringAlarm(void *parameter)
{
  unsigned long startTime = millis();
  bool ringOn = buzzerEnabled;
  bool lightOn = lightCtrlEnabled;

  int currentHour = hour();
  int currentMinute = minute();

  int alarmMelody[] = {NOTE_C5, NOTE_C5, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_G4, NOTE_G4};
  int alarmDurations[] = {8, 8, 8, 12, 12, 12, 8, 8};
  int SecAlarmMelody[] = {NOTE_C3, NOTE_C3, NOTE_B2, NOTE_B2, NOTE_B2, NOTE_B2, NOTE_G2, NOTE_G2};
  int SecAlarmDurations[] = {4, 4, 4, 8, 8, 8, 4, 4};

  Melody melody1 = {alarmMelody, alarmDurations, sizeof(alarmMelody) / sizeof(int)};
  Melody melody2 = {SecAlarmMelody, SecAlarmDurations, sizeof(SecAlarmMelody) / sizeof(int)};
  if (lightOn == true && WiFi.status() == WL_CONNECTED)
  {
    sendOnPostRequest();
  }
  Serial.println("Starting Alarm");

  unsigned long lastToggleRequestTime = startTime;
  bool alarmDetectInput = false;

  while (true)
  {
    if (alarmDetectInput == false)
    {
      alarmDetectInput = (useAllTouch() != No_Seg);
    }
    if ((millis() - startTime >= 15000 || WiFi.SSID() != SSID1 || WiFi.status() != WL_CONNECTED || (currentHour >= 11 && currentHour <= 21)) && ringOn == true)
    {
      if (alarmDetectInput == false)
      {
        playMelody(melody1);
      }
      else
      {
        playMelody(melody2);
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

    touchStopAlarm(currentHour, ringOn, lightOn, startTime);
    vTaskDelay(pdMS_TO_TICKS(1));
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