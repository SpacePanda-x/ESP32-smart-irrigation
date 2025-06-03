#include "weather.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <secrets.h>
#include <Logger.h>

const char *API_URL = OPENMETEO_URL
    "&daily=precipitation_sum,et0_fao_evapotranspiration,sunrise"
    "&forecast_days=1"
    "&past_days=7"
    "&timeformat=unixtime";

bool getWeather(WeatherData &data)
{
    HTTPClient http;
    http.begin(API_URL);

    int httpCode = http.GET();
    if (httpCode != 200)
    {
        Log.printf("HTTP request failed: %s", httpCode);
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    Log.printf("%s\n", payload); // Optional for debugging

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
        Log.print("JSON parse failed: ");
        Log.println(error.c_str());
        return false;
    }

    JsonArray timeArray = doc["daily"]["time"];
    JsonArray sunriseArray = doc["daily"]["sunrise"];
    JsonArray precip = doc["daily"]["precipitation_sum"];
    JsonArray et0 = doc["daily"]["et0_fao_evapotranspiration"];

    if (timeArray.isNull() || sunriseArray.isNull() || precip.isNull() || et0.isNull())
    {
        Log.println("Missing data arrays in JSON.");
        return false;
    }

    size_t days = timeArray.size();
    if (sunriseArray.size() != days || precip.size() != days || et0.size() != days)
    {
        Log.println("Inconsistent array sizes in weather data.");
        return false;
    }

    data.precipitation.clear();
    data.evapotranspiration.clear();
    data.sunrise.clear();

    for (size_t i = 0; i < days; i++)
    {
        data.sunrise.push_back(sunriseArray[i].as<time_t>());
        data.precipitation.push_back(precip[i].as<float>());
        data.evapotranspiration.push_back(et0[i].as<float>());
    }

    return true;
}
