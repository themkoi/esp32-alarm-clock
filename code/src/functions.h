#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "defines.h"

void centerText(String text, int y, int x = -1);
void leftAlignText(String text, int y,int offset, int x = -1);
String formatWithLeadingZero(int number);

String getCurrentWeekdayName();
String getShortCurrentWeekdayName();

String getWeekdayName(int weekday);
String getShortWeekdayName(int weekday);
String getShorterWeekdayName(int weekday);
String getShortestWeekdayName(int weekday);

String getNextDayName(int daysAfterToday);
String getShortNextDay(int days);

String getCurrentMonthName();
String getMonthName(int monthNumber);


String resetReasonToString(esp_reset_reason_t reason);

int getChipRevision();

String microsecondsToTimeString(uint64_t microseconds);

String wifiStatusToString(int status);
String getSignalQuality(int32_t rssi);


#endif
