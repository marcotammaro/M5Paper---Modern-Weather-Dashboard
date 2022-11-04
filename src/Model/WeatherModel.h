#pragma once

#include <Arduino.h>
#include <Time.h>

#include "Enum/WeatherIcon.h"
#include "Globals.h"

struct WeatherModel {
    time_t lastUpdate = 0;

    // Today widget
    String todayLocation = "";
    time_t todayDate = 0;
    float todayTemp = 0;
    String todayWeatherDesc = "";
    WeatherIcon todayIcon = WeatherIcon::SUN;

    // Set/Rise Widget
    time_t sunriseTime = 0;
    time_t sunsetTime = 0;
    time_t moonriseTime = 0;
    time_t moonsetTime = 0;

    // Forecast widget
    time_t forecastDates[maxForecastDays] = {0};
    float forecastMinTemps[maxForecastDays] = {0};
    float forecastMaxTemps[maxForecastDays] = {0};
    WeatherIcon forecastIcons[maxForecastDays] = {WeatherIcon::SUN};

    // Chart widget
    float temperatureValues[maxForecastDays] = {0};
    float rainValues[maxForecastDays] = {0};
    float humidityValues[maxForecastDays] = {0};
    float windValues[maxForecastDays] = {0};
};
