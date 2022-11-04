#pragma once
#include <Arduino.h>
#include <M5EPD.h>
#include <Time.h>
#include <TimeLib.h>
#include <WiFi.h>

#include "Config.h"
#include "Globals.h"

class EPDController {
   public:
    static void init();
    static void sleep(int);

    static bool startWiFi();
    static void stopWiFi();

    static int getBatteryPercentage();
};