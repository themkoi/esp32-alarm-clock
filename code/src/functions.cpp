#include "functions.h"

void centerText(String text, int y, int x)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  if (x == -1)
  {
    x = (SCREEN_WIDTH - w) / 2;
  }
  display.setCursor(x, y);
  display.print(text);
}

void leftAlignText(String text, int y,int offset, int x)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  if (x == -1)
  {
    x = ((SCREEN_WIDTH - w) - offset) - 2;
  }
  display.setCursor(x, y);
  display.print(text);
}

String formatWithLeadingZero(int number)
{
    return (number < 10) ? "0" + String(number) : String(number);
}


String getMonthName(int monthNumber)
{
  const char *months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
  if (monthNumber >= 1 && monthNumber <= 12)
  {
    return String(months[monthNumber - 1]);
  }
  else
  {
    return "Invalid month number";
  }
}

String getCurrentMonthName()
{
  const char *months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
  time_t currentTime = now();          // Get the current time
  int monthIndex = month(currentTime); // Get the month (1 = January, 2 = February, etc.)
  return months[monthIndex - 1];       // Adjust index to match array (0 = January, 1 = February, etc.)
}

String getCurrentWeekdayName()
{
  const char *weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  time_t currentTime = now();              // Get the current time
  int weekdayIndex = weekday(currentTime); // Get the day of the week (1 = Sunday, 2 = Monday, etc.)
  return weekdays[weekdayIndex - 1];       // Adjust index to match array (0 = Sunday, 1 = Monday, etc.)
}

String getShortCurrentWeekdayName()
{
  const char *weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  time_t currentTime = now();              // Get the current time
  int weekdayIndex = weekday(currentTime); // Get the day of the week (1 = Sunday, 2 = Monday, etc.)
  return weekdays[weekdayIndex - 1];       // Adjust index to match array (0 = Sunday, 1 = Monday, etc.)
}

String getWeekdayName(int weekday)
{
  const char *weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  int weekdayIndex = weekday;        // Get the day of the week (1 = Sunday, 2 = Monday, etc.)
  return weekdays[weekdayIndex - 1]; // Adjust index to match array (0 = Sunday, 1 = Monday, etc.)
}

String getShortWeekdayName(int weekday)
{
  const char *weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  int weekdayIndex = weekday;        // Get the day of the week (1 = Sunday, 2 = Monday, etc.)
  return weekdays[weekdayIndex - 1]; // Adjust index to match array (0 = Sunday, 1 = Monday, etc.)
}

String getShorterWeekdayName(int weekday) {
  const char *weekdays[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
  return weekdays[weekday - 1];
}

String getShortestWeekdayName(int weekday) {
  const char *weekdays[] = {"Su", "M", "Tu", "W", "Th", "F", "Sa"};
  return weekdays[weekday - 1];
}



String getNextDayName(int daysAfterToday)
{
  // Get the current time
  time_t currentTime = now();

  // Calculate the future time by adding days
  time_t futureTime = currentTime + (daysAfterToday * 24 * 60 * 60);

  // Get the day of the week for the future time
  int dayOfWeek = weekday(futureTime);

  // Get the name of the day
  String dayName = dayStr(dayOfWeek);

  return dayName;
}

String getShortNextDay(int days)
{
  time_t now = time(nullptr);
  time_t fut = now + (days * 86400);
  int dow = weekday(fut);

  switch (dow) {
    case 1: return "Sun";
    case 2: return "Mon";
    case 3: return "Tue";
    case 4: return "Wed";
    case 5: return "Thu";
    case 6: return "Fri";
    case 7: return "Sat";
    default: return "Err";
  }
}

String getSignalQuality(int32_t rssi) {
  if (rssi >= -50) {
    return "Great";
  } else if (rssi >= -60) {
    return "Good";
  } else if (rssi >= -70) {
    return "Fair";
  } else {
    return "Weak";
  }
}



String resetReasonToString(esp_reset_reason_t reason)
{
 switch (reason) {
        case ESP_RST_UNKNOWN:
            return "ESP_RST_UNKNOWN"; // Reset reason cannot be determined
        case ESP_RST_POWERON:
            return "ESP_RST_POWERON"; // Reset due to power-on event
        case ESP_RST_EXT:
            return "ESP_RST_EXT";     // Reset by external pin (not applicable for ESP32)
        case ESP_RST_SW:
            return "ESP_RST_SW";      // Software reset via esp_restart
        case ESP_RST_PANIC:
            return "ESP_RST_PANIC";   // Software reset due to exception/panic
        case ESP_RST_INT_WDT:
            return "ESP_RST_INT_WDT"; // Reset (software or hardware) due to interrupt watchdog
        case ESP_RST_TASK_WDT:
            return "ESP_RST_TASK_WDT"; // Reset due to task watchdog
        case ESP_RST_WDT:
            return "ESP_RST_WDT";     // Reset due to other watchdogs
        case ESP_RST_DEEPSLEEP:
            return "ESP_RST_DEEPSLEEP"; // Reset after exiting deep sleep mode
        case ESP_RST_BROWNOUT:
            return "ESP_RST_BROWNOUT"; // Brownout reset (software or hardware)
        case ESP_RST_SDIO:
            return "ESP_RST_SDIO";    // Reset over SDIO
        default:
            return "UNKNOWN"; // Unknown reset reason
    }
}

String wifiStatusToString(int status)
{
  switch (status)
  {
  case WL_IDLE_STATUS:
    return "Idle";
  case WL_NO_SSID_AVAIL:
    return "No SSID Available";
  case WL_SCAN_COMPLETED:
    return "Scan Completed";
  case WL_CONNECTED:
    return "Connected";
  case WL_CONNECT_FAILED:
    return "Connect Failed";
  case WL_CONNECTION_LOST:
    return "Connection Lost";
  case WL_DISCONNECTED:
    return "Disconnected";
  default:
    return "Unknown Status";
  }
}

String microsecondsToTimeString(uint64_t microseconds)
{
  // Convert microseconds to seconds
  uint64_t total_seconds = microseconds / 1000000;

  // Calculate hours, minutes, and seconds
  uint64_t hours = total_seconds / 3600;
  uint64_t remaining_seconds = total_seconds % 3600;
  uint64_t minutes = remaining_seconds / 60;
  uint64_t seconds = remaining_seconds % 60;

  // Create the formatted string
  String formatted_time = String(hours) + ":" + String(minutes) + ":" + String(seconds);
  return formatted_time;
}
