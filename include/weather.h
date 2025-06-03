#ifndef WEATHER_H
#define WEATHER_H

#include <vector>
#include <Arduino.h>

struct WeatherData
{
    std::vector<float> precipitation;
    std::vector<float> evapotranspiration;
    std::vector<time_t> sunrise;
};

bool getWeather(WeatherData &data);

#endif
