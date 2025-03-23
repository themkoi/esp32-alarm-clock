#ifndef OPENMETEO_H
#define OPENMETEO_H

#include <ArduinoJson.h>
#include <HTTPClient.h>

#define MAX_HOURS 167
#define MAX_DAYS 7

#define MAX_HOURS_AIR_QUALITY 119


#define OM_LOGS_ENABLED

String getStringRequest(String url);

// Hourly Forecast with Sunrise Sunset
typedef struct OM_HourlyForecast
{
    // Hourly

    time_t hourly_time[MAX_HOURS] = {0};

    float temp[MAX_HOURS] = {0};
    uint8_t humidity[MAX_HOURS] = {0};
    float apparent_temp[MAX_HOURS] = {0};

    float pressure[MAX_HOURS] = {0};

    uint8_t precipitation[MAX_HOURS] = {0};
    uint8_t cloud_cover[MAX_HOURS] = {0};
    uint16_t visibility[MAX_HOURS] = {0};

    float wind_speed[MAX_HOURS] = {0};
    uint16_t wind_deg[MAX_HOURS] = {0};
    float wind_gust[MAX_HOURS] = {0};

    uint8_t weather_code[MAX_HOURS] = {0};

    bool is_day[MAX_HOURS] = {0};

    // Daily
    time_t daily_time[MAX_DAYS] = {0};

    uint32_t sunrise[MAX_DAYS] = {0};
    uint32_t sunset[MAX_DAYS] = {0};

} OM_HourlyForecast;

typedef struct OM_DailyForecast
{
    time_t daily_time[MAX_DAYS] = {0};

    uint8_t weather_code[MAX_DAYS] = {0};

    float temp_max[MAX_DAYS] = {0};
    float temp_min[MAX_DAYS] = {0};

    float apparent_temp_max[MAX_DAYS] = {0};
    float apparent_temp_min[MAX_DAYS] = {0};

    uint32_t sunrise[MAX_DAYS] = {0};
    uint32_t sunset[MAX_DAYS] = {0};

    uint8_t precipitation_max[MAX_DAYS] = {0};
    uint8_t precipitation_hours[MAX_DAYS] = {0};

    float wind_speed_max[MAX_DAYS] = {0};
    float wind_gust_max[MAX_DAYS] = {0};
    uint16_t wind_deg_dominant[MAX_DAYS] = {0};

} OM_DailyForecast;

typedef struct OM_CurrentWeather
{
    time_t time = 0;

    float temp = 0;
    uint8_t humidity = 0;
    float apparent_temp = 0;

    bool is_day = 0;

    uint8_t weather_code = 0;

    float pressure = 0;
    uint8_t cloud_cover = 0;

    float wind_speed = 0;
    uint16_t wind_deg = 0;
    float wind_gust = 0;

} OM_CurrentWeather;

typedef struct OM_AirQualityForecast
{
    time_t time[MAX_HOURS_AIR_QUALITY] = {0};
    uint8_t EU_AQI[MAX_HOURS_AIR_QUALITY] = {0};

} OM_AirQualityForecast;

typedef struct OM_CurrentAirQuality
{
    time_t time = 0;
    uint8_t EU_AQI = 0;
} OM_CurrentAirQuality;

#define HOURLY_API_LINK "&hourly=temperature_2m,relative_humidity_2m,apparent_temperature,precipitation_probability,weather_code,pressure_msl,cloud_cover,visibility,wind_speed_10m,wind_direction_10m,wind_gusts_10m,is_day&daily=sunrise,sunset&timeformat=unixtime&timezone=auto"

bool getHourlyForecast(OM_HourlyForecast *structure, float latitude, float longitude, String apiLink = HOURLY_API_LINK);

#define DAILY_API_LINK "&daily=weather_code,temperature_2m_max,temperature_2m_min,apparent_temperature_max,apparent_temperature_min,sunrise,sunset,precipitation_hours,precipitation_probability_max,wind_speed_10m_max,wind_gusts_10m_max,wind_direction_10m_dominant&timeformat=unixtime&timezone=auto"

bool getDailyForecast(OM_DailyForecast *structure, float latitude, float longitude, String apiLink = DAILY_API_LINK);

#define CURRENT_API_LINK "&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,weather_code,cloud_cover,pressure_msl,wind_speed_10m,wind_direction_10m,wind_gusts_10m&timeformat=unixtime&timezone=auto"

bool getCurrentWeather(OM_CurrentWeather *structure, float latitude, float longitude, String apiLink = CURRENT_API_LINK);

#define AIR_HOURLY_API_LINK "&hourly=european_aqi&timeformat=unixtime&timezone=auto"

bool getAirQualityForecast(OM_AirQualityForecast *structure, float latitude, float longitude, String apiLink = AIR_HOURLY_API_LINK);

#define AIR_CURRENT_API_LINK "&current=european_aqi&timeformat=unixtime&timezone=auto"

bool getCurrentAirQuality(OM_CurrentAirQuality *structure, float latitude, float longitude, String apiLink);


#endif