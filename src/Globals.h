#pragma once
#include <M5EPD.h>

// Main canvas that view use to render UI, it is defined in
// EPDController.cpp
extern M5EPD_Canvas canvas;

// Screen dimensions
const int maxX = 960;
const int maxY = 540;

// Height of the text at different size
const int textSize14H = 16;
const int textSize20H = 24;
const int textSize30H = 36;

// Num of showen days in the forecast and chart widget
const int maxForecastDays = 7;