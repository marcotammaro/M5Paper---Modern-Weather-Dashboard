#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <M5EPD.h>
#include <MoonRise.h>
#include <Time.h>
#include <WiFiClient.h>

#include "Config.h"
#include "Controller/EPDController/EPDController.h"
#include "Enum/WeatherIcon.h"
#include "Model/WeatherModel.h"

class WeatherController {
   public:
    WeatherController();
    bool hasEndedDataLoading();
    bool hasEndedDataLoadingWithError();
    void getWeatherData(bool = false);

    WeatherModel data;

   private:
    bool postToWeatherAPI(DynamicJsonDocument &);
    bool postToGeocodingAPI(DynamicJsonDocument &);
    bool parseWeatherDoc(const JsonObject &);
    bool parseGeocodingDoc(const JsonObject &);

    WeatherIcon openWeatherMapToWeatherIcon(String);

    xTaskHandle xHandle_GetWeatherData;
    static void getWeatherDataImpl(void *);

    bool getWeatherDataCompleted;
    bool weatherAPISuccess;
    bool geocodingAPISuccess;
    bool parseWeatherSuccess;
    bool parseGeocodingSuccess;
};