#pragma once

#include <Arduino.h>

#include "Controller/WeatherController/WeatherController.h"
#include "Model/WeatherModel.h"
#include "Strings.h"

class WeatherView {
   public:
    WeatherView();

    void show();
    void drawContent();

    void drawTodayWidget(int, int);
    void drawSetRiseWidget(int, int);
    void drawForecastWidget(int, int);
    void drawChartsWidget(int, int);
    void drawGraph(int, int, int, int, int, int, float[]);
    void drawLoadingData(int, int);
    void drawErrorMessage();
    void drawBaseNavBarWithTitle(String, bool = true);

   private:
    WeatherController weatherController;

    String getDateString(time_t);
    String getTimeString(time_t);
    String getWeekDayString(time_t);
    String capitalizeFirstChar(String);
    const char* weatherIconToSdFile(WeatherIcon);
};