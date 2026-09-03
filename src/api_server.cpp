#include <Arduino.h>
#include "api_server.h"
#include <WebServer.h>


float getTemperature();
float getHumidity();
float getAverageTemp();



WebServer server (80);

void handleTemperature(){
    server.send(200,
         "text/plain",
          String(getTemperature())
    );
}

void handleHumidity(){
    server.send(200,
    "text/plain",
    String(getHumidity()));
}

void handleAverageTemp(){
    server.send(200,
    "text/plain",
    String(getAverageTemp()));

}

void initRestAPI(){
    

    server.on("/temperature", HTTP_GET, handleTemperature);
    server.on("/humidity", HTTP_GET, handleHumidity);
    server.on("/averagetemp", HTTP_GET, handleAverageTemp);

    server.begin();
}

void handleRestAPI(){
    server.handleClient();
}

