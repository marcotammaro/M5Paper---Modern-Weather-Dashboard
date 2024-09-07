#pragma once

// The time threshold (in seconds) after which the dashboard will refresh data.
// NOTICE: Refreshing more than 1,000 times will result in exceeding the free
// subscription limits.
#define REFRESH_INTERVAL 60 * 60

// WiFI
#define WIFI_SSID "YOUR WIFI SSID"
#define WIFI_PW "YOUR WIFI PASSWORD"

// Open Weather Map
#define LATITUDE 40.829338
#define LONGITUDE 14.247718
#define LOCALIZATION "en"
#define UNITS "metric"
#define OPENWEATHER_SRV "api.openweathermap.org"
#define OPENWEATHER_PORT 80
#define OPENWEATHER_API "YOUR OPEN WEATHER MAP API KEY"
