#ifndef DEFINES_H
#define DEFINES_H

#include "fonts/fonts.h"

#include <Arduino.h>
#include <LittleFS.h>
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
// #include <buttonManager.h>
#include <Grafici.h>
#include <Timezone.h>

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

// WiFi
#define SIZE_WIFI_CRED_STAT 3

#define WIFI_SYNC_TIME 50000

#define WIFI_COUNTRY_FIX 1 // Enable this to 1 to enable the fix
/*
Supported country codes are "01"(world safe mode) "AT","AU","BE","BG","BR", "CA","CH","CN","CY","CZ","DE","DK","EE","ES","FI","FR","GB","GR","HK","HR","HU", "IE","IN","IS","IT","JP","KR","LI","LT","LU","LV","MT","MX","NL","NO","NZ","PL","PT", "RO","SE","SI","SK","TW","US"
*/
#define WIFI_COUNTRY_CODE "PL"
#define WIFI_COUNTRY_FORCE false // This should be false, you can set it to true to check if something starts working

// NTP
//#define TIME_OFFSET_S 3600            // Time offset in seconds, use this as timezones
// Just change the timezones in NTP.cpp file im too lazy beh

// Weather
#define DAILY_WEATHER_INTERVAL 5 * 60 * 60 * 1000
#define CURRENT_WEATHER_INTERVAL 30 * 60 * 1000  // 30 minutes

// Light Sensor
#define OLED_DISABLE_THRESHOLD 20 // lux at which oled will turn off
#define LED_DIM_THRESHOLD 30 // lux below what screen will have lowest brightness
#define LED_DISABLE_THRESHOLD 1 // lux at which led screen will turn off

#define DIMMING_INTERVAL 1000 // how often to update screen brightness

#define DIM_DELAY 15000 // how long to have max brightness after input gets detected

#define LED_HYSTERESIS 2
#define LED_MAX_BRIGHTNESS 100
#define LED_BRIGHTNESS_MIN 0
#define LED_BRIGHTNESS_MAX 7

// Inputs
#define HALL_SWITCH GPIO_NUM_39 // Hall switch only used to turn off alarm currently

// Button stuf
#define MENU_PIN GPIO_NUM_25
#define BACK_PIN GPIO_NUM_26
#define DOWN_PIN GPIO_NUM_13
#define UP_PIN GPIO_NUM_12

#define SMALL_BUTTON_DELAY_MS 15

#define BUTTON_TASK_DELAY 10 // In ms, lower means faster button detection but more cpu usage
#define ADD_BUTTON_DELAY 1
#define BUTTON_LONG_PRESS_MS 1000     // Duration until long press registers in miliseconds

#define BUT_STATE HIGH
#define BUT_CLICK_STATE LOW
#define BUTTON_INTER_COND FALLING

// Touch stuff I guess beh
#define TOUCH_1_Seg_PIN GPIO_NUM_33
#define TOUCH_1_Seg_THRESHOLD 4
#define TOUCH_2_Seg_PIN GPIO_NUM_4
#define TOUCH_2_Seg_THRESHOLD 4
#define TOUCH_3_Seg_PIN GPIO_NUM_32
#define TOUCH_3_Seg_THRESHOLD 5
#define TOUCH_4_Seg_PIN GPIO_NUM_27
#define TOUCH_4_Seg_THRESHOLD 5
#define TOUCH_5_Seg_PIN GPIO_NUM_2
#define TOUCH_5_Seg_THRESHOLD 5

// On battery
#define TOUCH_1_Seg_THRESHOLD_BAT 29
#define TOUCH_2_Seg_THRESHOLD_BAT 30
#define TOUCH_3_Seg_THRESHOLD_BAT 32
#define TOUCH_4_Seg_THRESHOLD_BAT 33
#define TOUCH_5_Seg_THRESHOLD_BAT 35

// While Sleeping
#define TOUCH_1_Seg_THRESHOLD_SLEEP 29
#define TOUCH_2_Seg_THRESHOLD_SLEEP 32
#define TOUCH_3_Seg_THRESHOLD_SLEEP 33
#define TOUCH_4_Seg_THRESHOLD_SLEEP 33
#define TOUCH_5_Seg_THRESHOLD_SLEEP 40

// Menus
#define LOOP_FUNCTION_TIMEOUT_MS 60000 // how fast to exit from loop activated in menu this only works if the loop is calling shouldExitLoop()
#define MENU_TIMEOUT 20000 // How fast to go to main page without any input

#define MAX_MENU_ITEMS 15 // Max menus change this too if you increase alarm number--  no doont think so ?
#define MAX_ALARMS 30 // Max number of alarms

#define MAIN_PAGE_DURATION 60000
#define SCREENSAVER_DURATION 30000

#define N_FLYERS 5  // Number of flyers on screensaver

// Buzzer
#define BUZZER_PIN GPIO_NUM_14
#define START_SOUND true // uncomment to enable start sound

// Power management
#define MIN_VOLTAGE 3.30  // Minimum voltage of LiPo battery
#define MAX_VOLTAGE 4.20  // Maximum voltage of LiPo battery

#define VOLTAGE_DIVIDER_PIN 34 
#define ADC_VOLTAGE_DIVIDER 710.094f // 300K and 806K
#define ADC_OFFSET 77 // It subtracts from the read milivolts to calibrate the adc a bit its not great but it works

#define POWER_STATE_PIN GPIO_NUM_36

#define CHARGER_CONTROL_PIN GPIO_NUM_23
#define BATT_TARGET_VOLTAGE 3.85   // Target voltage in volts
#define BATT_HYSTERESIS 0.15       // charging Hysteresis in volts

#define TIMER_WAKUP_TIME 500 // How long it will stay awake after waking up from timer
#define GPIO_WAKUP_TIME 30000 // How long to stay awake after waking up from input
#define SLEEPING_TIME 50 * 1000000 // In microseconds how long it will sleep for 

// Charts
#define CHART_READINGS 55 // Number of readings to keep
#define INTERVAL_CHARTS 1570909 // How often to read data for charts 
#define BOOL_STR(b) ((b) ? String("True") : String("False")) // dont touch

#include "hardware/pitches.h"

#include "confidential.h"

#include "hardware/hardware.h"
#include "hardware/input/buttons/buttons.h"
#include "hardware/input/buttons/combinations.h"
#include "hardware/input/touch/touch.h"
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
