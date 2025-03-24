#include "OpenMeteo.h"
#include <Arduino.h>
#include <WiFi.h>

String getStringRequest(String url)
{
    WiFiClient client;
    HTTPClient http;
    http.begin(client, url);
    int httpResponseCode = http.GET();
    String payload = "";

    if (httpResponseCode > 0)
    {
#ifdef OM_LOGS_ENABLED
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
#endif
        payload = http.getString();
    }
    else
    {
#ifdef OM_LOGS_ENABLED
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
#endif
    }
    http.end();
    return payload;
}

bool getHourlyForecast(OM_HourlyForecast *structure, float latitude, float longitude, String apiLink)
{
    String url = "http://api.open-meteo.com/v1/forecast?latitude=" + String(latitude) + "&longitude=" + String(longitude) + apiLink;
    String jsonString = getStringRequest(url);
    if (jsonString.isEmpty()) return false;

    JsonDocument jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, jsonString);
    if (error) return false;

    for (size_t i = 0; i < MAX_HOURS; i++)
    {
        structure->hourly_time[i] = jsonDoc["hourly"]["time"][i];
        structure->temp[i] = jsonDoc["hourly"]["temperature_2m"][i];
        structure->humidity[i] = jsonDoc["hourly"]["relative_humidity_2m"][i];
        structure->apparent_temp[i] = jsonDoc["hourly"]["apparent_temperature"][i];
        structure->precipitation[i] = jsonDoc["hourly"]["precipitation_probability"][i];
        structure->weather_code[i] = jsonDoc["hourly"]["weather_code"][i];
        structure->pressure[i] = jsonDoc["hourly"]["pressure_msl"][i];
        structure->cloud_cover[i] = jsonDoc["hourly"]["cloud_cover"][i];
        structure->visibility[i] = jsonDoc["hourly"]["visibility"][i];
        structure->wind_speed[i] = jsonDoc["hourly"]["wind_speed_10m"][i];
        structure->wind_deg[i] = jsonDoc["hourly"]["wind_direction_10m"][i];
        structure->wind_gust[i] = jsonDoc["hourly"]["wind_gusts_10m"][i];
        structure->is_day[i] = jsonDoc["hourly"]["is_day"][i];
    }
    for (size_t i = 0; i < MAX_DAYS; i++)
    {
        structure->daily_time[i] = jsonDoc["daily"]["time"][i];
        structure->sunrise[i] = jsonDoc["daily"]["sunrise"][i];
        structure->sunset[i] = jsonDoc["daily"]["sunset"][i];
    }
    return true;
}

bool getDailyForecast(OM_DailyForecast *structure, float latitude, float longitude, String apiLink)
{
    String url = "http://api.open-meteo.com/v1/forecast?latitude=" + String(latitude) + "&longitude=" + String(longitude) + apiLink;
    String jsonString = getStringRequest(url);
    if (jsonString.isEmpty()) return false;

    JsonDocument jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, jsonString);
    if (error) return false;

    for (size_t i = 0; i < MAX_DAYS; i++)
    {
        structure->daily_time[i] = jsonDoc["daily"]["time"][i];
        structure->weather_code[i] = jsonDoc["daily"]["weather_code"][i];
        structure->temp_max[i] = jsonDoc["daily"]["temperature_2m_max"][i];
        structure->temp_min[i] = jsonDoc["daily"]["temperature_2m_min"][i];
        structure->apparent_temp_max[i] = jsonDoc["daily"]["apparent_temperature_max"][i];
        structure->apparent_temp_min[i] = jsonDoc["daily"]["apparent_temperature_min"][i];
        structure->sunrise[i] = jsonDoc["daily"]["sunrise"][i];
        structure->sunset[i] = jsonDoc["daily"]["sunset"][i];
        structure->precipitation_hours[i] = jsonDoc["daily"]["precipitation_hours"][i];
        structure->precipitation_max[i] = jsonDoc["daily"]["precipitation_probability_max"][i];
        structure->wind_speed_max[i] = jsonDoc["daily"]["wind_speed_10m_max"][i];
        structure->wind_gust_max[i] = jsonDoc["daily"]["wind_gusts_10m_max"][i];
        structure->wind_deg_dominant[i] = jsonDoc["daily"]["wind_direction_10m_dominant"][i];
    }
    return true;
}

bool getCurrentWeather(OM_CurrentWeather *structure, float latitude, float longitude, String apiLink)
{
    String url = "http://api.open-meteo.com/v1/forecast?latitude=" + String(latitude) + "&longitude=" + String(longitude) + apiLink;
    String jsonString = getStringRequest(url);
    if (jsonString.isEmpty()) return false;

    JsonDocument jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, jsonString);
    if (error) return false;

    structure->time = jsonDoc["current"]["time"];
    structure->temp = jsonDoc["current"]["temperature_2m"];
    structure->humidity = jsonDoc["current"]["relative_humidity_2m"];
    structure->apparent_temp = jsonDoc["current"]["apparent_temperature"];
    structure->is_day = jsonDoc["current"]["is_day"];
    structure->weather_code = jsonDoc["current"]["weather_code"];
    structure->cloud_cover = jsonDoc["current"]["cloud_cover"];
    structure->pressure = jsonDoc["current"]["pressure_msl"];
    structure->wind_speed = jsonDoc["current"]["wind_speed_10m"];
    structure->wind_deg = jsonDoc["current"]["wind_direction_10m"];
    structure->wind_gust = jsonDoc["current"]["wind_gusts_10m"];
    return true;
}

bool getCurrentAirQuality(OM_CurrentAirQuality *structure, float latitude, float longitude, String apiLink)
{
    String url = "http://air-quality-api.open-meteo.com/v1/air-quality?latitude=" + String(latitude) + "&longitude=" + String(longitude) + apiLink;
    String jsonString = getStringRequest(url);
    if (jsonString.isEmpty()) return false;

    JsonDocument jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, jsonString);
    if (error) return false;

    structure->time = jsonDoc["hourly"]["time"];
    structure->EU_AQI = jsonDoc["hourly"]["european_aqi"];
    return true;
}

bool getAirQualityForecast(OM_AirQualityForecast *structure, float latitude, float longitude, String apiLink)
{
    String url = "http://air-quality-api.open-meteo.com/v1/air-quality?latitude=" + String(latitude) + "&longitude=" + String(longitude) + apiLink;
    String jsonString = getStringRequest(url);
    if (jsonString.isEmpty()) return false;

    JsonDocument jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, jsonString);
    if (error) return false;

    for (size_t i = 0; i < MAX_HOURS_AIR_QUALITY; i++)
    {
        structure->time[i] = jsonDoc["hourly"]["time"][i];
        structure->EU_AQI[i] = jsonDoc["hourly"]["european_aqi"][i];
    }
    return true;
}