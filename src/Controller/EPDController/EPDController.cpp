#include "EPDController.h"

M5EPD_Canvas canvas(&M5.EPD);

/* Initialize the M5Paper */
void EPDController::init() {
    M5.begin(true, false, true, true, false);
    M5.RTC.begin();

    SPI.begin(14, 13, 12, 4);
    SD.begin(4, SPI, 20000000);

    M5.EPD.SetRotation(0);
    M5.TP.SetRotation(0);

    M5.EPD.Clear(false);

    canvas.loadFont("/font/Roboto-Black.ttf", SD);
    canvas.setTextColor(G15);
    canvas.createRender(14);
    canvas.createRender(18);
    canvas.createRender(20);
    canvas.createRender(30);
    canvas.createRender(70);

    Serial.println("Initialized EPD!");
}

void EPDController::sleep(int sec) {
    EPDController::stopWiFi();
    Serial.println("Going to sleep, bye bye!");
    M5.shutdown(sec);
}

/* Start and connect to the wifi */
bool EPDController::startWiFi() {
    IPAddress dns(8, 8, 8, 8);  // Google DNS

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    delay(100);

    WiFi.begin(WIFI_SSID, WIFI_PW);

    for (int retry = 0; WiFi.status() != WL_CONNECTED && retry < 30; retry++) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected at: " + WiFi.localIP().toString());
        return true;
    } else {
        Serial.println("WiFi connection *** FAILED ***");
        return false;
    }

    WiFi.setSleep(WIFI_PS_NONE);
}

/* Stop the wifi connection */
void EPDController::stopWiFi() {
    Serial.println("Stop WiFi");
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
}

int EPDController::getBatteryPercentage() {
    uint32_t vol = M5.getBatteryVoltage();

    if (vol < 3300) {
        vol = 3300;
    } else if (vol > 4350) {
        vol = 4350;
    }

    float battery = (float)(vol - 3300) / (float)(4350 - 3300);

    if (battery <= 0.01) {
        battery = 0.01;
    }
    if (battery > 1) {
        battery = 1;
    }

    return (int)(battery * 100);
};