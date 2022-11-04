#include "WeatherView.h"

WeatherView::WeatherView() {}

void WeatherView::show() {
    Serial.println("Showing WeatherView");
    M5.EPD.Clear(true);
    drawBaseNavBarWithTitle(strings_weather_title);

    weatherController = WeatherController();
    weatherController.getWeatherData();  // get weather data
    drawLoadingData(470, 230);  // this will take the cpu until controller
                                // finish loading the weather data
                                // once finished, the view will be drawn

    if (weatherController.hasEndedDataLoadingWithError()) {
        drawErrorMessage();
    } else {
        drawContent();
    }
}

void WeatherView::drawContent() {
    drawTodayWidget(80, 20);
    drawSetRiseWidget(80, 275);
    drawForecastWidget(340, 20);
    drawChartsWidget(700, 20);
};

void WeatherView::drawTodayWidget(int x, int y) {
    int w = 240;
    int h = 235;
    int lineW = 4;

    String locationDate = weatherController.data.todayLocation + " " +
                          getDateString(weatherController.data.todayDate);
    String temperature = String((int)weatherController.data.todayTemp) + "°";
    String desc = capitalizeFirstChar(weatherController.data.todayWeatherDesc);
    String lastUpdate =
        strings_last_update + getTimeString(weatherController.data.lastUpdate);
    const char* icon = weatherIconToSdFile(weatherController.data.todayIcon);

    canvas.createCanvas(w, h);
    canvas.fillCanvas(0);
    canvas.pushCanvas(x, y, UPDATE_MODE_A2);

    canvas.fillRoundRect(0, 0, w, h, 25, G15);
    canvas.fillRoundRect(lineW, lineW, w - lineW * 2, h - lineW * 2, 21, G1);

    canvas.setTextSize(20);
    canvas.drawString(locationDate, 20, 20);
    canvas.setTextSize(70);
    canvas.drawString(temperature, 20, 60);
    canvas.setTextSize(18);
    canvas.drawString(desc, 20, 173);
    canvas.drawString(lastUpdate, 20, 195);

    canvas.drawPngFile(SD, icon, 10, 118, 48, 48);

    canvas.pushCanvas(x, y, UPDATE_MODE_A2);
    canvas.deleteCanvas();
};

void WeatherView::drawSetRiseWidget(int x, int y) {
    int w = 240;
    int h = 235;
    int lineW = 4;

    String sun = getTimeString(weatherController.data.sunsetTime) + " - " +
                 getTimeString(weatherController.data.sunriseTime);
    String moon = getTimeString(weatherController.data.moonsetTime) + " - " +
                  getTimeString(weatherController.data.moonriseTime);

    canvas.createCanvas(w, h);
    canvas.fillCanvas(0);
    canvas.pushCanvas(x, y, UPDATE_MODE_A2);

    canvas.fillRoundRect(0, 0, w, h, 25, G15);
    canvas.fillRoundRect(lineW, lineW, w - lineW * 2, h - lineW * 2, 21, G1);
    canvas.fillRect(0, h / 2 - lineW / 2, w, lineW, G15);

    canvas.setTextSize(20);
    canvas.drawCentreString(strings_sunrise_sunset, w / 2,
                            h / 6 - textSize20H / 2, 1);
    canvas.drawCentreString(sun, w / 2, h / 3 - textSize20H / 2, 1);

    canvas.drawCentreString(strings_moonrise_moonset, w / 2,
                            h / 2 + h / 6 - textSize20H / 2, 1);
    canvas.drawCentreString(moon, w / 2, h / 2 + h / 3 - textSize20H / 2, 1);

    canvas.drawPngFile(SD, "/icons/sunrise.png", lineW + 2, h / 3 - 24, 48, 48);
    canvas.drawPngFile(SD, "/icons/sunset.png", w - lineW - 50, h / 3 - 24, 48,
                       48);
    canvas.drawPngFile(SD, "/icons/moonrise.png", lineW + 2, h / 2 + h / 3 - 24,
                       48, 48);
    canvas.drawPngFile(SD, "/icons/moonset.png", w - lineW - 50,
                       h / 2 + h / 3 - 24, 48, 48);

    canvas.pushCanvas(x, y, UPDATE_MODE_A2);
    canvas.deleteCanvas();
};

void WeatherView::drawForecastWidget(int x, int y) {
    int w = 340;
    int h = 490;
    int lineW = 4;
    int innerLineW = 2;
    int rowH = 60;

    time_t* forecastDates = weatherController.data.forecastDates;
    float* forecastMinTemps = weatherController.data.forecastMinTemps;
    float* forecastMaxTemps = weatherController.data.forecastMaxTemps;
    WeatherIcon* forecastIcons = weatherController.data.forecastIcons;

    canvas.createCanvas(w, h);
    canvas.fillCanvas(0);
    canvas.pushCanvas(x, y, UPDATE_MODE_A2);

    canvas.fillRoundRect(0, 0, w, h, 25, G15);
    canvas.fillRoundRect(lineW, lineW, w - lineW * 2, h - lineW * 2, 21, G1);
    canvas.fillRect(0, rowH, w, innerLineW, G15);

    canvas.setTextSize(20);
    canvas.drawString(strings_forecast_title, 20, 36 - textSize20H / 2);

    for (int i = 1; i < 8; i++) {
        String dayName = getWeekDayString(forecastDates[i - 1]);
        String date = getDateString(forecastDates[i - 1]);
        const char* icon = weatherIconToSdFile(forecastIcons[i - 1]);
        String temp =
            strings_forecast_min + String((int)forecastMinTemps[i - 1]) + "° " +
            strings_forecast_max + String((int)forecastMaxTemps[i - 1]) + "°";

        int iconY = rowH + innerLineW + (i * rowH) - (rowH / 2) - 24;
        canvas.drawPngFile(SD, icon, 5, iconY, 48, 48);

        int dayNameY =
            rowH + innerLineW + (i * rowH) - (rowH / 2) - textSize20H;
        int dayNumY = rowH + innerLineW + (i * rowH) - (rowH / 2);
        canvas.drawString(dayName, 56, dayNameY);
        canvas.drawString(date, 56, dayNumY);

        int tempY =
            rowH + innerLineW + (i * rowH) - (rowH / 2) - textSize20H / 2;
        canvas.drawRightString(temp, w - 10, tempY, 1);

        if (i != 7) {
            int separatorY = rowH + innerLineW + (i * rowH) - innerLineW;
            canvas.fillRect(0, separatorY, w, innerLineW, G15);
        }
    };

    canvas.pushCanvas(x, y, UPDATE_MODE_A2);
    canvas.deleteCanvas();
};

void WeatherView::drawChartsWidget(int x, int y) {
    int w = 240;
    int h = 490;
    int lineW = 4;
    int innerLineW = 2;
    int rowH = 122;

    canvas.createCanvas(w, h);
    canvas.fillCanvas(0);
    canvas.pushCanvas(x, y, UPDATE_MODE_A2);

    canvas.fillRoundRect(0, 0, w, h, 25, G15);
    canvas.fillRoundRect(lineW, lineW, w - lineW * 2, h - lineW * 2, 21, G1);

    canvas.setTextSize(20);

    String chartLabels[4] = {strings_temperature_chart, strings_rain_chart,
                             strings_humidity_chart, strings_wind_chart};
    float chartMixY[4] = {0, 0, 0, 0};
    float chartMaxY[4] = {50, 10, 100, 10};
    float chartValues[4][maxForecastDays];
    memcpy(chartValues[0], weatherController.data.temperatureValues,
           maxForecastDays * sizeof(float));
    memcpy(chartValues[1], weatherController.data.rainValues,
           maxForecastDays * sizeof(float));
    memcpy(chartValues[2], weatherController.data.humidityValues,
           maxForecastDays * sizeof(float));
    memcpy(chartValues[3], weatherController.data.windValues,
           maxForecastDays * sizeof(float));

    int chartW = w - lineW * 2;
    int chartH = rowH - lineW - innerLineW - textSize20H - 20;
    int chartX = lineW;

    for (int i = 0; i < 4; i++) {
        int chartY = (i * rowH) + textSize20H + 15;
        int titleY = (i * rowH) + 15;
        canvas.setTextSize(20);
        canvas.drawString(chartLabels[i], chartX + (chartW / maxForecastDays),
                          titleY);
        if (i != 3) {
            int separatorY = ((i + 1) * rowH) - innerLineW;
            canvas.fillRect(0, separatorY, w, innerLineW, G15);
        }

        drawGraph(chartX, chartY, chartW, chartH, chartMixY[i], chartMaxY[i],
                  chartValues[i]);
    };

    canvas.pushCanvas(x, y, UPDATE_MODE_A2);
    canvas.deleteCanvas();
};

void WeatherView::drawGraph(int x, int y, int w, int h, int yMin, int yMax,
                            float values[]) {
    // Used to plot the line between two value points
    int previousValueX = 0;
    int previousValueY = 0;

    canvas.setTextSize(14);
    canvas.drawRightString(String(yMin), x + (w / maxForecastDays) - 5,
                           y + h - textSize14H / 2, 1);
    canvas.drawRightString(String(yMax), x + (w / maxForecastDays) - 5,
                           y - textSize14H / 2, 1);
    canvas.fillRect(x + (w / maxForecastDays), y, 2, h, G15);

    for (int i = 0; i < maxForecastDays; i++) {
        float value = values[i];

        if (value < yMin) {
            value = yMin;
        } else if (value > yMax) {
            value = yMax;
        }

        int valueX = x + ((i + 1) * w / maxForecastDays);
        int valueY = y + h - (h * ((value - yMin) / (yMax - yMin)));

        canvas.fillCircle(valueX, valueY, 2, G15);
        if (i > 0) {
            canvas.drawLine(previousValueX, previousValueY, valueX, valueY,
                            G15);
        }

        previousValueX = valueX;
        previousValueY = valueY;
    }
}

void WeatherView::drawLoadingData(int x, int y) {
    int dim = 80;
    canvas.createCanvas(dim + 20, dim + textSize20H + 15);
    canvas.setTextSize(20);
    canvas.drawCentreString(strings_loading, dim / 2, dim + 15, 1);
    canvas.pushCanvas(x, y, UPDATE_MODE_A2);
    canvas.deleteCanvas();

    // loop with loading animation
    M5EPD_Canvas animatedCanvas(&M5.EPD);
    while (!weatherController.hasEndedDataLoading()) {
        for (int i = 1; i < 5; i++) {
            animatedCanvas.createCanvas(dim, dim);
            animatedCanvas.fillCanvas(0);
            animatedCanvas.pushCanvas(x, y, UPDATE_MODE_DU4);

            animatedCanvas.fillCircle(dim / 2, dim / 2, dim / 2, G15);
            animatedCanvas.fillCircle(dim / 2, dim / 2, dim / 2 - 4, G1);

            switch (i) {
                case 1:
                    animatedCanvas.fillRect(0, 0, dim / 2, dim, G1);
                    animatedCanvas.fillRect(0, dim / 2, dim, dim / 2, G1);
                    break;
                case 2:
                    animatedCanvas.fillRect(0, 0, dim, dim / 2, G1);
                    animatedCanvas.fillRect(0, 0, dim / 2, dim, G1);
                    break;
                case 3:
                    animatedCanvas.fillRect(0, 0, dim, dim / 2, G1);
                    animatedCanvas.fillRect(dim / 2, 0, dim / 2, dim, G1);
                    break;
                case 4:
                    animatedCanvas.fillRect(0, dim / 2, dim, dim / 2, G1);
                    animatedCanvas.fillRect(dim / 2, 0, dim / 2, dim, G1);
                    break;
                default:
                    break;
            }

            animatedCanvas.pushCanvas(x, y, UPDATE_MODE_A2);
            animatedCanvas.deleteCanvas();
            delay(250);
        }
    }

    // cleaning up the animation
    canvas.createCanvas(dim + 20, dim + textSize20H + 15);
    canvas.fillCanvas(0);
    canvas.pushCanvas(x, y, UPDATE_MODE_A2);
    canvas.deleteCanvas();
}

void WeatherView::drawErrorMessage() {
    canvas.createCanvas(maxX - 60, maxY);
    canvas.setTextSize(30);
    canvas.drawCentreString(string_api_error, (maxX - 60) / 2,
                            maxY / 2 - (textSize30H / 2), 1);
    canvas.pushCanvas(60, 0, UPDATE_MODE_A2);
    canvas.deleteCanvas();
}

/* Convert the time_t to the date with format DD/MM */
String WeatherView::getDateString(time_t rawtime) {
    char buff[32];

    sprintf(buff, "%02d/%02d", day(rawtime), month(rawtime));

    return (String)buff;
}

/* Convert the time_t to the time with format HH:mm */
String WeatherView::getTimeString(time_t rawtime) {
    char buff[32];

    sprintf(buff, "%02d:%02d", hour(rawtime), minute(rawtime));

    return (String)buff;
}

/* Convert the time_t to the week day string */
String WeatherView::getWeekDayString(time_t rawtime) {
    char buff[32];
    sprintf(buff, "%01d", weekday(rawtime));
    String weekday = (String)buff;

    if (weekday == "2") {
        return strings_weekday_2;
    } else if (weekday == "3") {
        return strings_weekday_3;
    } else if (weekday == "4") {
        return strings_weekday_4;
    } else if (weekday == "5") {
        return strings_weekday_5;
    } else if (weekday == "6") {
        return strings_weekday_6;
    } else if (weekday == "7") {
        return strings_weekday_7;
    } else if (weekday == "1") {
        return strings_weekday_1;
    } else {
        return "";
    }
}

/* Convert the passed string to a new string with first char uppercase */
String WeatherView::capitalizeFirstChar(String str) {
    String firstChar = str.substring(0, 1);
    firstChar.toUpperCase();
    return firstChar + str.substring(1);
}

/* Convert WeatherView enum to the corresponding SD card icon name */
const char* WeatherView::weatherIconToSdFile(WeatherIcon icon) {
    switch (icon) {
        case WeatherIcon::SUN:
            return "/icons/01_sun.png";
        case WeatherIcon::SUN_CLOUD:
            return "/icons/02_sun_cloud.png";
        case WeatherIcon::CLOUD:
            return "/icons/03_cloud.png";
        case WeatherIcon::CLOUD_CLOUD:
            return "/icons/04_cloud_cloud.png";
        case WeatherIcon::CLOUD_RAIN:
            return "/icons/09_cloud_rain.png";
        case WeatherIcon::CLOUD_SUN_RAIN:
            return "/icons/10_cloud_sun_rain.png";
        case WeatherIcon::THUNDER:
            return "/icons/11_thunder.png";
        case WeatherIcon::SNOW:
            return "/icons/13_snow.png";
        case WeatherIcon::FOG:
            return "/icons/50_fog.png";
    }
    return "/icons/01_sun.png";
}

void WeatherView::drawBaseNavBarWithTitle(String title, bool isOn /* = true*/) {
    int w = 60;
    int h = 540;
    int lineW = 2;

    canvas.createCanvas(w, h);
    canvas.fillCanvas(0);
    canvas.pushCanvas(0, 0, UPDATE_MODE_A2);

    // Base layout
    canvas.fillRect(w - lineW, 0, lineW, h, G15);
    canvas.fillRect(0, 5 * h / 6, w, h / 6, G15);

    // Battery status
    int batteryCapacity = EPDController::getBatteryPercentage();
    int batteryW = 44;
    int batteryH = 16;
    int batteryX = (w - batteryW) / 2;
    int batteryY = (5 * h / 6) + (h / 24) - batteryH / 2;
    canvas.drawRect(batteryX, batteryY, batteryW - 4, batteryH, G1);
    canvas.drawRect(batteryX + (batteryW - 4), batteryY + 3, 4, 10, G1);
    for (int i = batteryX; i < batteryX + (batteryW - 4); i++) {
        canvas.drawLine(i, batteryY, i, batteryY + 15, G1);
        if ((i - batteryX) * 100.0 / 40.0 > batteryCapacity) {
            break;
        }
    }

    // Alive status
    int circleR = 12;
    int cirleY = (5 * h / 6) + (3 * h / 24) - circleR;
    if (isOn) {
        canvas.fillCircle(w / 2, cirleY, circleR, G1);
    } else {
        canvas.fillCircle(w / 2, cirleY, circleR, G1);
        canvas.fillCircle(w / 2, cirleY, circleR - lineW, G15);
    }

    canvas.pushCanvas(0, 0, UPDATE_MODE_A2);
    canvas.deleteCanvas();

    // Title
    M5.EPD.SetRotation(90);  // x/y and w/h are inverted
    canvas.createCanvas(h - (2 * h / 6), w - lineW);
    canvas.setTextSize(30);
    canvas.drawString(title, 24, w / 2 - textSize30H / 2);

    canvas.pushCanvas(h / 6, 0, UPDATE_MODE_A2);
    canvas.deleteCanvas();

    M5.EPD.SetRotation(0);
};