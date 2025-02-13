#ifndef DEFINES_H
#define DEFINES_H

#include "fonts/fonts.h"

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TM1637Display.h>
#include <LTR_F216A.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <TimeLib.h>
#include <NTPClient.h>
#include <Preferences.h>
#include <Adafruit_SHT4x.h>
#include <oledManager.h>
#include <buttonManager.h>
#include <Grafici.h>

#include "esp_pm.h"
#include "esp_wifi.h"

#include "../icons/icons/icons_18x18.h"
#include "../icons/icons/icons_24x24.h"
#include "../icons/icons/icons_32x32.h"
#include "../icons/icons/icons_48x48.h"

extern bool OTAEnabled;

// Led Display
#define CLK  18 
#define DIO  19 

// NTP
#define TIME_OFFSET_S 3600            // Time offset in seconds, use this as timezones

// Weather
#define DAILY_WEATHER_INTERVAL 5 * 60 * 60 * 1000
#define CURRENT_WEATHER_INTERVAL 30 * 60 * 1000  // 30 minutes

// Light Sensor
#define OLED_DISABLE_THRESHOLD 20
#define LED_DIM_THRESHOLD 30
#define LED_DISABLE_THRESHOLD 1
#define LED_HYSTERESIS 2
#define LED_MAX_BRIGHTNESS 100
#define LED_BRIGHTNESS_MIN 0
#define LED_BRIGHTNESS_MAX 7

#define DIM_DELAY 15000

#define MAX_INCREASE_OF_LIGHT_LEVEL 1000

// Inputs
#define HALL_SWITCH GPIO_NUM_39

#define MAX_MENU_ITEMS 15
#define MAX_ALARMS 15

// Menus
#define LOOP_FUNCTION_TIMEOUT_MS 60000 // 1 minute timeout
#define TOTAL_NAV_BUTTONS 4
#define MAX_DEPTH 20

#define MENU_TIMEOUT 10000

#define MAIN_PAGE_DURATION 60000
#define SCREENSAVER_DURATION 30000

#define N_FLYERS 5  // Numver of flyers on screensaver

// Buzzer
#define BUZZER_PIN GPIO_NUM_14
#define START_SOUND true // uncomment to enable start sound

// Battery
#define MIN_VOLTAGE 3.30  // Minimum voltage of LiPo battery
#define MAX_VOLTAGE 4.20  // Maximum voltage of LiPo battery

#define VOLTAGE_DIVIDER_PIN 34 
#define ADC_VOLTAGE_DIVIDER 710.094f // 300K and 806K
#define ADC_OFFSET 77 // In milivolts to calibrate it

#define POWER_STATE_PIN GPIO_NUM_36

#define CHARGER_CONTROL_PIN GPIO_NUM_23
#define BATT_TARGET_VOLTAGE 3.85   // Target voltage in volts
#define BATT_HYSTERESIS 0.15       // charging Hysteresis in volts

#define TIMER_WAKUP_TIME 5000 // In miliseconds
#define GPIO_WAKUP_TIME 30000 // In miliseconds
#define SLEEPING_TIME 35 * 1000000 

#define CHARGING_THRESHOLD 1000
#define STANDBY_THRESHOLD 1000

// Charts
#define CHART_READINGS 55 // Number of readings to keep
#define INTERVAL_CHARTS 300000
#define BOOL_STR(b) ((b) ? String("True") : String("False"))


#include "hardware/pitches.h"

#include "confidential.h"

#include "hardware/hardware.h"
#include "functions.h"
#include "WiFi/WiFi.h"
#include "NTP/NTP.h"
#include "weather/weather.h"
#include "LedDisplay/LedDisplay.h"
#include "lightSensor/lightSensor.h"
#include "oledDisplay/interface.h"
#include "oledDisplay/mainPage.h"
#include "oledDisplay/charts.h"
#include "oledDisplay/menus.h"
#include "Flash/Flash.h"
#include "alarms/alarms.h"
#include "battery/battery.h"
#include "tempSensor/tempSensor.h"


#endif
