#include "WeatherController.h"

WeatherController::WeatherController() {
    getWeatherDataCompleted = false;
    weatherAPISuccess = false;
    geocodingAPISuccess = false;
    parseWeatherSuccess = false;
    parseGeocodingSuccess = false;
};

bool WeatherController::hasEndedDataLoading() {
    return getWeatherDataCompleted;
};

bool WeatherController::hasEndedDataLoadingWithError() {
    return !(weatherAPISuccess && geocodingAPISuccess && parseWeatherSuccess &&
             parseGeocodingSuccess);
};

void WeatherController::getWeatherData(bool threadCreated /* = false*/) {
    if (!threadCreated) {
        // Crating a thread that will manage the download
        // and the parsing of the weather data
        xTaskCreate(this->getWeatherDataImpl, "GetWeatherData", 4096, this, 5,
                    &xHandle_GetWeatherData);
    } else {
        // doc sizes calculated with
        // https://arduinojson.org/v6/assistant/#/step1

        EPDController::startWiFi();

        DynamicJsonDocument weatherDoc(35 * 1024);
        weatherAPISuccess = postToWeatherAPI(weatherDoc);

        DynamicJsonDocument geocodingDoc(2 * 1024);
        geocodingAPISuccess = postToGeocodingAPI(geocodingDoc);

        parseWeatherSuccess = parseWeatherDoc(weatherDoc.as<JsonObject>());
        parseGeocodingSuccess =
            parseGeocodingDoc(geocodingDoc[0].as<JsonObject>());

        getWeatherDataCompleted = true;
        EPDController::stopWiFi();
        vTaskDelete(xHandle_GetWeatherData);
    }
};

void WeatherController::getWeatherDataImpl(void *_this) {
    WeatherController *controller = (WeatherController *)_this;
    controller->getWeatherData(true);
}

bool WeatherController::postToWeatherAPI(DynamicJsonDocument &doc) {
    WiFiClient client;
    HTTPClient http;
    String uri;

    uri += "/data/3.0/onecall";
    uri += "?lat=" + String((float)LATITUDE, 5);
    uri += "&lon=" + String((float)LONGITUDE, 5);
    uri += "&units=" + (String)UNITS;
    uri += "&exclude=minutely,hourly";
    uri += "&lang=" + (String)LOCALIZATION;
    uri += "&appid=" + (String)OPENWEATHER_API;

    Serial.printf(
        "[WeatherController::postToWeatherAPI] Requesting %s to %s:%d\n",
        uri.c_str(), OPENWEATHER_SRV, OPENWEATHER_PORT);

    client.stop();
    http.begin(client, OPENWEATHER_SRV, OPENWEATHER_PORT, uri);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        Serial.printf(
            "[WeatherController::postToWeatherAPI] Unable to get weather info, "
            "error: %s",
            http.errorToString(httpCode).c_str());
        client.stop();
        http.end();
        return false;
    } else {
        Serial.println(
            "[WeatherController::postToWeatherAPI] Successfully got weather "
            "info");
        DeserializationError error = deserializeJson(doc, http.getStream());

        if (error) {
            Serial.print(
                F("[WeatherController::postToWeatherAPI] JSON Deserialization "
                  "failed: "));
            Serial.println(error.c_str());
            return false;
        } else {
            return true;
        }
    }
};

bool WeatherController::postToGeocodingAPI(DynamicJsonDocument &doc) {
    WiFiClient client;
    HTTPClient http;
    String uri;

    uri += "/geo/1.0/reverse";
    uri += "?lat=" + String((float)LATITUDE, 5);
    uri += "&lon=" + String((float)LONGITUDE, 5);
    uri += "&limit=1";
    uri += "&lang=" + (String)LOCALIZATION;
    uri += "&appid=" + (String)OPENWEATHER_API;

    Serial.printf(
        "[WeatherController::postToGeocodingAPI] Requesting %s to %s:%d\n",
        uri.c_str(), OPENWEATHER_SRV, OPENWEATHER_PORT);

    client.stop();
    http.begin(client, OPENWEATHER_SRV, OPENWEATHER_PORT, uri);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        Serial.printf(
            "[WeatherController::postToGeocodingAPI] Unable to get geocoding "
            "info, "
            "error: %s",
            http.errorToString(httpCode).c_str());
        client.stop();
        http.end();
        return false;
    } else {
        Serial.println(
            "[WeatherController::postToGeocodingAPI] Successfully got "
            "geocoding "
            "info");
        DeserializationError error = deserializeJson(doc, http.getStream());

        if (error) {
            Serial.print(F(
                "[WeatherController::postToGeocodingAPI] JSON Deserialization "
                "failed: "));
            Serial.println(error.c_str());
            return false;
        } else {
            return true;
        }
    }
};

bool WeatherController::parseWeatherDoc(const JsonObject &root) {
    int currentTimeOffset = root["timezone_offset"].as<int>();
    time_t currentTime = currentTimeOffset + root["current"]["dt"].as<int>();
    data.lastUpdate = currentTime;

    data.todayDate = currentTimeOffset + root["current"]["dt"].as<int>();
    data.todayTemp = root["current"]["feels_like"].as<float>();
    data.todayWeatherDesc =
        root["current"]["weather"][0]["description"].as<String>();
    data.todayIcon = openWeatherMapToWeatherIcon(
        root["current"]["weather"][0]["icon"].as<String>());

    data.sunriseTime = currentTimeOffset + root["current"]["sunrise"].as<int>();
    data.sunsetTime = currentTimeOffset + root["current"]["sunset"].as<int>();

    MoonRise mr;
    mr.calculate(LATITUDE, LONGITUDE, currentTime);
    data.moonriseTime = mr.riseTime;
    data.moonsetTime = mr.setTime;

    JsonArray daily_list = root["daily"];
    for (int i = 0; i < maxForecastDays + 1; i++) {
        int j = i + 1;
        if (j < daily_list.size()) {
            data.forecastDates[i] =
                currentTimeOffset + daily_list[j]["dt"].as<int>();
            data.forecastMinTemps[i] = daily_list[j]["temp"]["min"].as<float>();
            data.forecastMaxTemps[i] = daily_list[j]["temp"]["max"].as<float>();
            data.forecastIcons[i] = openWeatherMapToWeatherIcon(
                daily_list[j]["weather"][0]["icon"].as<String>());

            data.temperatureValues[i] =
                daily_list[j]["feels_like"]["day"].as<float>();
            data.rainValues[i] = daily_list[j]["rain"].as<float>();
            data.humidityValues[i] = daily_list[j]["humidity"].as<float>();
            data.windValues[i] = daily_list[j]["wind_speed"].as<float>();
        }
    }

    return true;
};

bool WeatherController::parseGeocodingDoc(const JsonObject &root) {
    data.todayLocation = root["local_names"][(String)LOCALIZATION].as<String>();
    return true;
};

WeatherIcon WeatherController::openWeatherMapToWeatherIcon(String icon) {
    if (icon == "01d" || icon == "01n") {
        return WeatherIcon::SUN;
    } else if (icon == "02d" || icon == "02n") {
        return WeatherIcon::SUN_CLOUD;
    } else if (icon == "03d" || icon == "03n") {
        return WeatherIcon::CLOUD;
    } else if (icon == "04d" || icon == "04n") {
        return WeatherIcon::CLOUD_CLOUD;
    } else if (icon == "09d" || icon == "09n") {
        return WeatherIcon::CLOUD_RAIN;
    } else if (icon == "10d" || icon == "10n") {
        return WeatherIcon::CLOUD_SUN_RAIN;
    } else if (icon == "11d" || icon == "11n") {
        return WeatherIcon::THUNDER;
    } else if (icon == "13d" || icon == "13n") {
        return WeatherIcon::SNOW;
    } else if (icon == "50d" || icon == "50n") {
        return WeatherIcon::FOG;
    } else {
        return WeatherIcon::SUN;
    }
}
