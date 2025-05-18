#include "weather.h"

bool isWeatherAvailable = false;
savedDailyForecastData weatherDailyForecastData[MAX_DAYS]; // Days / Data for these days
CurrentWeatherData currentWeatherData;

TaskHandle_t WeatherTask;

void weatherTask(void *parameter);
void syncWeather();
void dumpWeather();

void createWeatherTask()
{
    xTaskCreatePinnedToCore(
        weatherTask,       // Function to implement the task
        "Syncing Weather", // Name of the task
        2048,             // Stack size (words)
        NULL,              // Parameter to pass
        1,                 // Priority
        &WeatherTask,      // Task handle
        1                  // Core to run the task on (Core 1)
    );
}

void deleteWeatherTask()
{
    vTaskDelete(WeatherTask);
}

void weatherTask(void *parameter)
{
    uint32_t lastDailyWeatherSync = 0;
    uint32_t lastCurrentWeatherSync = 0;
    int dailyWeatherFailCount = 0;
    int currentWeatherFailCount = 0;

    if (!isWeatherAvailable)
    {
        syncDailyWeather();
        vTaskDelay(pdMS_TO_TICKS(10000));
        syncCurrentWeather();
    }

    while (true)
    {
        uint32_t currentTime = millis();

        if (currentTime - lastDailyWeatherSync >= DAILY_WEATHER_INTERVAL)
        {
            if (WiFi.status() == WL_CONNECTED)
            {
                if (syncDailyWeather())
                {
                    lastDailyWeatherSync = currentTime;
                    dailyWeatherFailCount = 0; 
                    Serial.println("Daily weather synced.");
                }
                else
                {
                    dailyWeatherFailCount++;
                    Serial.printf("Daily weather sync failed (%d/5). Retrying in 5 seconds.\n", dailyWeatherFailCount);
                    if (dailyWeatherFailCount >= 5)
                    {
                        Serial.println("Daily weather failed 5 times. Waiting 30 minutes before retrying.");
                        vTaskDelay(pdMS_TO_TICKS(30 * 60 * 1000));
                        dailyWeatherFailCount = 0;
                    }
                    else
                    {
                        vTaskDelay(pdMS_TO_TICKS(5000));
                    }
                }
            }
            else
            {
                Serial.println("WiFi not connected. Cannot sync daily weather.");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5000));

        if (currentTime - lastCurrentWeatherSync >= CURRENT_WEATHER_INTERVAL)
        {
            if (WiFi.status() == WL_CONNECTED)
            {
                if (syncCurrentWeather())
                {
                    lastCurrentWeatherSync = currentTime;
                    currentWeatherFailCount = 0;
                    Serial.println("Current weather synced.");
                }
                else
                {
                    currentWeatherFailCount++;
                    Serial.printf("Current weather sync failed (%d/5). Retrying in 5 seconds.\n", currentWeatherFailCount);
                    if (currentWeatherFailCount >= 5)
                    {
                        Serial.println("Current weather failed 5 times. Waiting 30 minutes before retrying.");
                        vTaskDelay(pdMS_TO_TICKS(30 * 60 * 1000));
                        currentWeatherFailCount = 0;
                    }
                    else
                    {
                        vTaskDelay(pdMS_TO_TICKS(5000)); // Retry in 5 seconds
                    }
                }
            }
            else
            {
                Serial.println("WiFi not connected. Cannot sync current weather.");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}


bool syncDailyWeather()
{
    // Dynamically allocate OM_DailyForecast
    OM_DailyForecast *omdailyForecast = new OM_DailyForecast;

    bool returnedData = getDailyForecast(omdailyForecast, WEATHER_LATIT, WEATHER_LONGTIT);
    if (returnedData == false)
    {
        return false;
    }

    // Update weatherDailyForecastData with the retrieved data
    for (int i = 0; i < MAX_DAYS; i++)
    {
        weatherDailyForecastData[i].dt = omdailyForecast->daily_time[i];
        weatherDailyForecastData[i].windDirection = omdailyForecast->wind_deg_dominant[i];
        weatherDailyForecastData[i].temp = (omdailyForecast->apparent_temp_max[i] + omdailyForecast->apparent_temp_min[i]) / 2;
        weatherDailyForecastData[i].minTemp = omdailyForecast->temp_min[i];
        weatherDailyForecastData[i].maxTemp = omdailyForecast->temp_max[i];
        weatherDailyForecastData[i].weatherConditionId = omdailyForecast->weather_code[i];
        weatherDailyForecastData[i].windSpeed = omdailyForecast->wind_speed_max[i];
        weatherDailyForecastData[i].windGusts = omdailyForecast->wind_gust_max[i];
        weatherDailyForecastData[i].pop = omdailyForecast->precipitation_max[i];
        weatherDailyForecastData[i].sunrise = omdailyForecast->sunrise[i];
        weatherDailyForecastData[i].sunset = omdailyForecast->sunset[i];
    }

    // Deallocate the memory to prevent memory leaks
    delete omdailyForecast;

    // Mark the weather data as available
    isWeatherAvailable = true;
    return true;
}

bool syncCurrentWeather()
{
    // Dynamically allocate OM_CurrentWeather
    OM_CurrentWeather *omCurrentWeather = new OM_CurrentWeather;

    // Get the current weather data
    bool returnedData = getCurrentWeather(omCurrentWeather, WEATHER_LATIT, WEATHER_LONGTIT);
    if (returnedData == false)
    {
        return false;
    }

    // Populate the CurrentWeatherData structure
    currentWeatherData.dt = omCurrentWeather->time;
    currentWeatherData.main = weatherConditionIdToStr(omCurrentWeather->weather_code);
    currentWeatherData.windDirection = omCurrentWeather->wind_deg;
    currentWeatherData.isDay = omCurrentWeather->is_day;
    currentWeatherData.temp = omCurrentWeather->temp;
    currentWeatherData.pressure = omCurrentWeather->pressure;
    currentWeatherData.humidity = omCurrentWeather->humidity;
    currentWeatherData.cloudsPerc = omCurrentWeather->cloud_cover;
    currentWeatherData.windSpeed = omCurrentWeather->wind_speed;
    currentWeatherData.windGusts = omCurrentWeather->wind_gust;
    currentWeatherData.weatherConditionId = omCurrentWeather->weather_code;

    // Deallocate the memory to prevent memory leaks
    delete omCurrentWeather;

    // Mark the weather data as available
    isWeatherAvailable = true;
    return true;
}

String weatherConditionIdToStr(int code_no)
{
    if (isWeatherAvailable == true)
    {
        switch (code_no)
        {
        case 0:
            return "Clear sky";
        case 1:
        case 2:
        case 3:
            return "Partly cloudy";
        case 45:
        case 48:
            return "Fog";
        case 51:
        case 53:
        case 55:
            return "Drizzle";
        case 56:
        case 57:
            return "Freezing drizzle";
        case 61:
        case 63:
        case 65:
            return "Rain";
        case 66:
        case 67:
            return "Freezing rain";
        case 71:
        case 73:
        case 75:
            return "Snow";
        case 77:
            return "Snow grains";
        case 80:
        case 81:
        case 82:
            return "Rain showers";
        case 85:
        case 86:
            return "Snow showers";
        case 95:
            return "Thunderstorm";
        case 96:
        case 99:
            return "Thunderstorm with hail";
        default:
            return "Unknown";
        }
    }
    else
    {
        return "N/A";
    }
}

void displaywidget(int weatherCode)
{
    const uint8_t *bitmap;
    if (isWeatherAvailable == true)
    {
        switch (weatherCode)
        {
        case 0:
            bitmap = wi_day_sunny_50x50; // Clear sky
            break;
        case 1:
        case 2:
        case 3:
            bitmap = wi_day_cloudy_50x50; // Mainly clear, partly cloudy, and overcast
            break;
        case 45:
        case 48:
            bitmap = wi_fog_50x50; // Fog and depositing rime fog
            break;
        case 51:
        case 53:
        case 55:
            bitmap = wi_rain_50x50; // Drizzle: Light, moderate, and dense intensity
            break;
        case 56:
        case 57:
            bitmap = wi_day_sleet_50x50; // Freezing Drizzle: Light and dense intensity
            break;
        case 61:
        case 63:
        case 65:
            bitmap = wi_day_rain_50x50; // Rain: Slight, moderate and heavy intensity
            break;
        case 66:
        case 67:
            bitmap = wi_rain_wind_50x50; // Freezing Rain: Light and heavy intensity
            break;
        case 71:
        case 73:
        case 75:
            bitmap = wi_day_snow_50x50; // Snow fall: Slight, moderate, and heavy intensity
            break;
        case 77:
            bitmap = wi_snowflake_cold_50x50; // Snow grains
            break;
        case 80:
        case 81:
        case 82:
            bitmap = wi_showers_50x50; // Rain showers: Slight, moderate, and violent
            break;
        case 85:
        case 86:
            bitmap = wi_snow_50x50; // Snow showers slight and heavy
            break;
        case 95:
            bitmap = wi_thunderstorm_50x50; // Thunderstorm: Slight or moderate
            break;
        case 96:
        case 99:
            bitmap = wi_day_thunderstorm_50x50; // Thunderstorm with slight and heavy hail
            break;
        default:
            bitmap = wi_cloud_down_50x50; // Default icon if code is unknown
            break;
        }

        display.drawBitmap(0, 3, bitmap, 50, 50, BLACK, WHITE); // Adjust color as needed
    }
    else
    {
        display.drawBitmap(0, 3, wi_cloud_down_50x50, 50, 50, BLACK, WHITE); // Adjust color as needed
    }
}

void displaySmallWidget(int weatherCode, int x, int y)
{
    const uint8_t *bitmap;
    if (isWeatherAvailable == true)
    {
        switch (weatherCode)
        {
        case 0:
            bitmap = wi_day_sunny_40x40; // Clear sky
            break;
        case 1:
        case 2:
        case 3:
            bitmap = wi_day_cloudy_40x40; // Mainly clear, partly cloudy, and overcast
            break;
        case 45:
        case 48:
            bitmap = wi_fog_40x40; // Fog and depositing rime fog
            break;
        case 51:
        case 53:
        case 55:
            bitmap = wi_rain_40x40; // Drizzle: Light, moderate, and dense intensity
            break;
        case 56:
        case 57:
            bitmap = wi_day_sleet_40x40; // Freezing Drizzle: Light and dense intensity
            break;
        case 61:
        case 63:
        case 65:
            bitmap = wi_day_rain_40x40; // Rain: Slight, moderate and heavy intensity
            break;
        case 66:
        case 67:
            bitmap = wi_rain_wind_40x40; // Freezing Rain: Light and heavy intensity
            break;
        case 71:
        case 73:
        case 75:
            bitmap = wi_day_snow_40x40; // Snow fall: Slight, moderate, and heavy intensity
            break;
        case 77:
            bitmap = wi_snowflake_cold_40x40; // Snow grains
            break;
        case 80:
        case 81:
        case 82:
            bitmap = wi_showers_40x40; // Rain showers: Slight, moderate, and violent
            break;
        case 85:
        case 86:
            bitmap = wi_snow_40x40; // Snow showers slight and heavy
            break;
        case 95:
            bitmap = wi_thunderstorm_40x40; // Thunderstorm: Slight or moderate
            break;
        case 96:
        case 99:
            bitmap = wi_day_thunderstorm_40x40; // Thunderstorm with slight and heavy hail
            break;
        default:
            bitmap = wi_cloud_down_40x40; // Default icon if code is unknown
            break;
        }

        display.drawBitmap(x, y, bitmap, 40, 40, BLACK, WHITE); // Adjust color as needed
    }
    else
    {
        display.drawBitmap(x, y, wi_cloud_down_40x40, 40, 40, BLACK, WHITE); // Adjust color as needed
    }
}
