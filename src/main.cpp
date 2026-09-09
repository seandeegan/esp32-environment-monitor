#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TFT_Touch.h>
#include <math.h>

#include "api_server.h"
#include "sensor_manager.h"
#include "wifi_manager.h"

namespace {
constexpr int touchDataOutPin = 39;
constexpr int touchDataInPin = 32;
constexpr int touchChipSelectPin = 33;
constexpr int touchClockPin = 25;
constexpr unsigned long buttonDebounceMs = 350;
constexpr int valueColumnX = 120;
constexpr int valueRowHeight = 16;

TFT_eSPI display;
TFT_Touch touch(touchChipSelectPin, touchClockPin, touchDataInPin, touchDataOutPin);
TFT_eSPI_Button unitButton;
char unitButtonLabel[] = "F/C";
bool displayFahrenheit = true;
unsigned long lastButtonPressMs = 0;

void drawValue(int y, float value, const char* unit) {
    // Clear the entire field so shorter values and "--" leave no old digits.
    display.fillRect(valueColumnX, y, display.width() - valueColumnX,
                     valueRowHeight, TFT_BLACK);
    display.setCursor(valueColumnX, y);
    if (!isfinite(value)) {
        display.print("--");
        return;
    }
    display.print(value, 1);
    display.print(unit);
}

float displayTemperature(float celsius) {
    return displayFahrenheit ? celsius * 1.8f + 32.0f : celsius;
}

void updateDisplay() {
    display.setTextSize(2);
    display.setTextColor(TFT_GREEN, TFT_BLACK);
    const char* unit = displayFahrenheit ? "F" : "C";
    drawValue(20, displayTemperature(getTemperature()), unit);
    drawValue(40, getHumidity(), "%");
    drawValue(80, displayTemperature(getAverageTemp()), unit);

    display.fillRect(0, 115, display.width(), 32, TFT_BLACK);
    display.setTextSize(1);
    display.setCursor(0, 115);
    display.print("Wi-Fi: ");
    display.print(getWiFiStatusText());
    display.setCursor(0, 130);
    display.print(hasValidSensorReading() ? "Sensor: OK" : "Sensor: unavailable");
}

void initDisplay() {
    display.init();
    display.setRotation(0);
    display.fillScreen(TFT_BLACK);
    display.setTextWrap(false);

    // Existing panel-specific calibration for portrait 240x320 operation.
    // Recalibrate with lib/TFT_Touch/Examples/TFT_Touch_Calibrate_v2 after
    // changing the panel or orientation; the original measurements are unknown.
    touch.setCal(3800, 500, 230, 3670, 240, 320, 0);
    unitButton.initButton(&display, 50, 200, 100, 40, TFT_WHITE, TFT_BLUE,
                          TFT_WHITE, unitButtonLabel, 2);
    unitButton.drawButton();

    display.setTextSize(2);
    display.setTextColor(TFT_GREEN, TFT_BLACK);
    display.setCursor(0, 20);
    display.print("Temp:");
    display.setCursor(0, 40);
    display.print("Humidity:");
    display.setCursor(0, 80);
    display.print("Avg temp:");
}

bool handleTouch(unsigned long now) {
    if (touch.Pressed()) {
        unitButton.press(unitButton.contains(touch.X(), touch.Y()));
    } else {
        unitButton.press(false);
    }
    if (unitButton.justPressed() && now - lastButtonPressMs >= buttonDebounceMs) {
        lastButtonPressMs = now;
        displayFahrenheit = !displayFahrenheit;
        return true;
    }
    return false;
}
}  // namespace

void setup() {
    Serial.begin(9600);
    initDisplay();
    initSensor();
    initWiFi();
    initRestAPI();
    updateDisplay();
}

void loop() {
    const unsigned long now = millis();
    const bool wifiChanged = handleWiFi(now);
    const bool sensorChanged = handleSensor(now);
    const bool unitsChanged = handleTouch(now);
    handleRestAPI();

    // Drawing only on state changes leaves time for HTTP requests and input.
    if (wifiChanged || sensorChanged || unitsChanged) {
        updateDisplay();
    }
}
