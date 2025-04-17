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
#define HALL_SWITCH GPIO_NUM_39
//                              rest of inputs are in the buttonmanager was simpler this way I guess

// Menus
#define LOOP_FUNCTION_TIMEOUT_MS 60000 // how fast to exit from loop activated in menu this only works if the loop is calling shouldExitLoop()
#define MENU_TIMEOUT 20000 // How fast to go to main page without any input

#define MAX_MENU_ITEMS 15 // Max menus change this too if you increase alarm number--  no doont think so ?
#define MAX_ALARMS 20 // Max number of alarms

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

#define TIMER_WAKUP_TIME 5000 // How long it will stay awake after waking up from timer
#define GPIO_WAKUP_TIME 30000 // How long to stay awake after waking up from input
#define SLEEPING_TIME 50 * 1000000 // In microseconds how long it will sleep for 

// Charts
#define CHART_READINGS 55 // Number of readings to keep
#define INTERVAL_CHARTS 300000 // How often to read data for charts 
#define BOOL_STR(b) ((b) ? String("True") : String("False")) // dont touch


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
