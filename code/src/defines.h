#pragma once

#include "fonts/fonts.h"

#include <Arduino.h>
#include <LittleFS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1327.h>
#include <AS1115.h>
#include <SparkFun_VEML6030_Ambient_Light_Sensor.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <TimeLib.h>
#include <NTPClient.h>
#include <Preferences.h>
#include <Adafruit_SHT4x.h>
#include <Adafruit_BMP280.h>
// #include <buttonoledMana.h>
#include <Grafici.h>
#include <Timezone.h>
#include <AT42QT2120.h>
#include "Adafruit_APDS9960.h"

#include "esp_pm.h"
#include "esp_wifi.h"

#include "../icons/generated_icons/icons_18x18.h"
#include "../icons/generated_icons/icons_24x24.h"
#include "../icons/generated_icons/icons_32x32.h"
#include "../icons/generated_icons/icons_48x48.h"

// MCP
#define MCP23018_ADDRESS 0x27 // https://www.chiark.greenend.org.uk/~peterb/electronics/mcp23018/addr

#define MENU_PIN 1
#define BACK_PIN 0
#define DOWN_PIN 3
#define UP_PIN 2

#define HALL_SWITCH 12 // Hall switch only used to turn off alarm currently

#define MCP_5V 4 // B7

#define MCP_CHARGER_CONTROL_PIN 5

#define MCP_LED1_P1 8
#define MCP_LED1_P2 9
#define MCP_LED2_P1 10
#define MCP_LED2_P2 11

#define AUTO_OFF_DELAY_MS 180000 // leds how long until they automatically turn off when using toggle

// Interrupts

#define MCP_INTERRUPT_PIN 5
#define TOUCH_INTERRUPT 2

// Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

#define SCREEN_ORBIT_INTERVAL 3600000

#define OLED_CLK 20
#define OLED_MOSI 19
#define OLED_CS 22
#define OLED_DC 21

#define MIN_CONTRAST 80
#define MAX_CONTRAST 255
#define MIN_CONTRAST_NIGHT 60
#define MAX_CONTRAST_NIGHT 150

#define OLED_RESET -1

// I2C
#define I2C_SCL_PIN  GPIO_NUM_7 
#define I2C_SDA_PIN  GPIO_NUM_6
#define I2C_FREQ 100

// WiFi
#define SIZE_WIFI_CRED_STAT 3

#define WIFI_SYNC_TIME 10000

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

// APDS sensor
#define APDS9960_INT 6  // Interrupt pin
#define GESTURES_ENABLED false
#define PROXIMITY_THRESHOLD 50

// Light Sensor
#define OLED_DISABLE_THRESHOLD 20 // lux at which oled will turn off
#define LED_DIM_THRESHOLD 15 // lux below what screen will have lowest brightness
#define LED_DISABLE_THRESHOLD 1 // lux at which led screen will turn off

#define DISPLAY_HOLD_DELAY 30000
#define LIGHT_ALPHA 0.2f // Smoothing factor (lower = smoother, higher = more responsive)

#define DIMMING_INTERVAL 1000 // how often to update screen brightness

#define DIM_DELAY 30000 // how long to have max brightness after input gets detected

#define LED_HYSTERESIS 2
#define LED_MAP_MAX_LIGHT 100 // range in which the brightness gets mapped
#define LED_BRIGHTNESS_MIN 0
#define LED_BRIGHTNESS_MAX 15
#define LED_BRIGHTNESS_MAX_NIGHT 7

// Button stuff
#define SMALL_BUTTON_DELAY_MS 15

#define BUTTON_TASK_DELAY 10 // In ms, lower means faster button detection but more cpu usage
#define ADD_BUTTON_DELAY 1
#define BUTTON_LONG_PRESS_MS 1000     // Duration until long press registers in miliseconds

#define BUT_STATE LOW
#define BUT_CLICK_STATE HIGH
#define BUTTON_INTER_COND FALLING

// Touch stuff I guess beh
#define RESET_DELAY_TOUCH 2000
#define CALIBRATION_LOOP_DELAY_TOUCH 50
#define KEYS_AMMOUNT 7

// Menus
#define LOOP_FUNCTION_TIMEOUT_MS 120000 // how fast to exit from the loop function in menu this only works if the loop is calling shouldExitLoop()
#define MENU_TIMEOUT 60000 // How fast to go to main page without any input

#define MAX_STACK_SIZE 1 // Max depth of submenus not counting in main menu and starting from 0
#define MAX_MENU_ITEMS 30 // Max menus change this too if you increase alarm number-- yeah you have to or theres a panic so yay
#define MAX_ALARMS 30 // Max number of alarms

#define MAIN_PAGE_DURATION 60000
#define SCREENSAVER_DURATION 30000

#define N_FLYERS 5  // Number of flyers on screensaver

// Buzzer
#define BUZZER_PIN GPIO_NUM_18
#define START_SOUND true // uncomment to enable start sound

// Power management
#define MIN_VOLTAGE 3.30  // Minimum voltage of LiPo battery
#define MAX_VOLTAGE 4.20  // Maximum voltage of LiPo battery

#define VOLTAGE_DIVIDER_PIN GPIO_NUM_4
#define ADC_VOLTAGE_DIVIDER 710.094f // 300K and 806K
#define ADC_OFFSET 400 // It subtracts from the read milivolts to calibrate the adc a bit its not great but it works

#define BATT_TARGET_VOLTAGE 3.85   // Target voltage in volts
#define BATT_HYSTERESIS 0.15       // charging Hysteresis in volts
#define BAT_CHE_INTERVAL 15000 // how often to check battery voltage and control charger

#define TIMER_WAKUP_TIME 5000 // How long it will stay awake after waking up from timer
#define GPIO_WAKUP_TIME 30000 // How long to stay awake after waking up from input
#define SLEEPING_TIME 50 * 1000000 // In microseconds how long it will sleep for 
#define MAX_AWAKE_HARD_LIMIT 300000 // Hard limit on how long it can stay awake on battery
#define BATTERY_WAIT_TIMEOUT 30000 // How long it waits before sleeping for the first time

// Charts
#define CHART_READINGS 55 // Number of readings to keep
#define INTERVAL_CHARTS 1570909 // How often to read data for charts 
#define BOOL_STR(b) ((b) ? String("True") : String("False")) // dont touch

#include "hardware/buzzer/pitches.h"

#include "confidential.h"

typedef enum
{
    Unknown = 0, // For the task to look for answers, used on the yatchy
    None = 1,
    Back = 2,
    Menu = 3,
    Up = 4,
    Down = 5,
    LongBack = 6,
    LongMenu = 7,
    LongUp = 8,
    LongDown = 9,
} inkButtonStates;

#include "hardware/i2c/i2c.h"
#include "hardware/sensors/tempSensor/tempSensor.h"
#include "hardware/sensors/lightSensor/lightSensor.h"
#include "hardware/sensors/pressureSensor/pressureSensor.h"
#include "hardware/sensors/colorDistSensor/colorDistSensor.h"
#include "hardware/hardware.h"
#include "hardware/display/display.h"
#include "hardware/input/buttons/buttons.h"
#include "hardware/mcp23018/mcp23018.h"
#include "hardware/input/buttons/combinations.h"
#include "hardware/input/touch/touch.h"
#include "hardware/input/gestures/gestures.h"
#include "hardware/LedDisplay/LedDisplay.h"
#include "functions.h"
#include "WiFi/WiFi.h"
#include "WiFi/ota.h"
#include "NTP/NTP.h"
#include "weather/weather.h"
#include "oledDisplay/interface.h"
#include "oledDisplay/mainPage.h"
#include "oledDisplay/menus/alarmMenu.h"
#include "oledDisplay/menus/charts.h"
#include "oledDisplay/menus/menus.h"
#include "Flash/Flash.h"
#include "alarms/alarms.h"
#include "battery/battery.h"