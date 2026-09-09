#include <Arduino.h>
#include "api_server.h"
#include <WebServer.h>
#include <math.h>


float getTemperature();
float getHumidity();
float getAverageTemp();



WebServer server (80);

static String jsonNumber(float value){
    return isfinite(value) ? String(value, 2) : String("null");
}

void handleReadings(){
    String response = "{\"temperature_c\":";
    response += jsonNumber(getTemperature());
    response += ",\"humidity\":";
    response += jsonNumber(getHumidity());
    response += "}";
    server.send(200, "application/json", response);
}

void handleTemperature(){
    server.send(200,
         "application/json",
          String("{\"temperature_c\":") + jsonNumber(getTemperature()) + "}"
    );
}

void handleHumidity(){
    server.send(200,
    "application/json",
    String("{\"humidity\":") + jsonNumber(getHumidity()) + "}");
}

void handleAverageTemp(){
    server.send(200,
    "application/json",
    String("{\"average_temperature_c\":") + jsonNumber(getAverageTemp()) + "}");

}

void initRestAPI(){
    

    server.on("/readings", HTTP_GET, handleReadings);
    server.on("/temperature", HTTP_GET, handleTemperature);
    server.on("/humidity", HTTP_GET, handleHumidity);
    server.on("/averagetemp", HTTP_GET, handleAverageTemp);

    server.begin();
}

void handleRestAPI(){
    server.handleClient();
}

