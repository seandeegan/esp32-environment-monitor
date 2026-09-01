#include <Arduino.h>
#include <WiFi.h>
#include "wifi_manager.h"

const char* ssid = "REMOVED_WIFI_SSID";
const char* password = "REMOVED_WIFI_PASSWORD";


void initWiFi(){
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);


    Serial.print("Connecting to WiFi");
    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");


        // 20s timeout
        if(millis() - start > 20000){
            Serial.println();
            Serial.println("WiFi connection timed out");
            Serial.print("Status: ");
            Serial.println(WiFi.status());
            return;
        }
        
    }

    Serial.println();
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

}