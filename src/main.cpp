#include <Arduino.h>

#include "Controller/EPDController/EPDController.h"
#include "View/WeatherView.h"

void setup() {
    EPDController::init();

    WeatherView weatherView;
    weatherView.show();

    delay(1000);
    weatherView.drawBaseNavBarWithTitle(strings_weather_title, false);

    delay(1000);
    EPDController::sleep(REFRESH_INTERVAL);
}

void loop() {}
