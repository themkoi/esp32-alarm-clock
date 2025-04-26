#ifndef WEATHER_H
#define WEATHER_H

#include "defines.h"
#include "OpenMeteo.h"
#include "../icons/icons/weather_icons_50x50.h"
#include "../icons/icons/weather_icons_40x40.h"

struct CurrentWeatherData {
    uint32_t dt; // time, unix
    String main; // Weather condition description
    uint16_t windDirection; // Wind direction in degrees
    bool isDay; // Temperature in Celsius
    float temp; // Temperature in Celsius
    float pressure; // Pressure in hPa
    uint8_t humidity; // Humidity percentage
    uint8_t cloudsPerc; // Cloud cover percentage
    float windSpeed; // Wind speed in m/s
    float windGusts; // Wind gusts in m/s (if available)
    uint8_t weatherConditionId; // Weather condition ID from Open Meteo
};

extern CurrentWeatherData currentWeatherData;

struct savedDailyForecastData {
    uint32_t dt; // time, unix, use strTime
    uint16_t windDirection;
    float temp; // C, depends
    float minTemp; // C, depends
    float maxTemp; // C, depends
    uint16_t weatherConditionId; // Use weatherConditionIdToStr
    float windSpeed; // m/s
    float windGusts; // m/s
    uint8_t pop; // Probability of precipitation in %
    uint32_t sunrise; // time, unix
    uint32_t sunset; // time, unix
};

extern bool isWeatherAvailable;
extern savedDailyForecastData weatherDailyForecastData[MAX_DAYS]; // Days / Data for these days

bool syncDailyWeather();
bool syncCurrentWeather();

void deleteWeatherTask();

void createWeatherTask();

String weatherConditionIdToStr(int weatherCode = 0);
void displaywidget(int weatherCode);
void displaySmallWidget(int weatherCode, int x, int y);

#endif
