#include <Arduino.h>
#include <WebServer.h>
#include <math.h>

#include "api_server.h"
#include "sensor_manager.h"
#include "wifi_manager.h"

namespace {
WebServer server(80);

String jsonNumber(float value) {
    // JSON has no NaN or infinity literal.
    return isfinite(value) ? String(value, 2) : String("null");
}

void sendJson(const String& response) {
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", response);
}

void handleReadings() {
    String response = "{\"temperature_c\":";
    response += jsonNumber(getTemperature());
    response += ",\"humidity\":";
    response += jsonNumber(getHumidity());
    response += "}";
    sendJson(response);
}

void handleTemperature() {
    sendJson(String("{\"temperature_c\":") + jsonNumber(getTemperature()) + "}");
}

void handleHumidity() {
    sendJson(String("{\"humidity\":") + jsonNumber(getHumidity()) + "}");
}

void handleAverageTemp() {
    sendJson(String("{\"average_temperature_c\":") + jsonNumber(getAverageTemp()) + "}");
}

void handleStatus() {
    String response = "{\"reading_valid\":";
    response += hasValidSensorReading() ? "true" : "false";
    response += ",\"sample_count\":";
    response += String(static_cast<unsigned int>(getSampleCount()));
    response += ",\"last_sample_age_ms\":";
    response += getSampleCount() ? String(getLastSampleAgeMs()) : String("null");
    response += ",\"wifi_connected\":";
    response += isWiFiConnected() ? "true" : "false";
    response += "}";
    sendJson(response);
}
}  // namespace

void initRestAPI() {
    server.on("/readings", HTTP_GET, handleReadings);
    server.on("/temperature", HTTP_GET, handleTemperature);
    server.on("/humidity", HTTP_GET, handleHumidity);
    server.on("/averagetemp", HTTP_GET, handleAverageTemp);
    server.on("/status", HTTP_GET, handleStatus);
    server.onNotFound([]() {
        server.send(404, "application/json", "{\"error\":\"not_found\"}");
    });
    server.begin();
}

void handleRestAPI() {
    server.handleClient();
}
