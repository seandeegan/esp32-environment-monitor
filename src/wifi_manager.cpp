#include <Arduino.h>
#include <WiFi.h>

#include "secrets.h"
#include "wifi_manager.h"

namespace {
constexpr unsigned long connectTimeoutMs = 20000;
constexpr unsigned long retryIntervalMs = 10000;
enum class ConnectionState { Connecting, Connected, Offline };
ConnectionState state = ConnectionState::Offline;
unsigned long stateStartedMs = 0;

void startConnection(unsigned long now) {
    WiFi.begin(Secrets::ssid, Secrets::password);
    state = ConnectionState::Connecting;
    stateStartedMs = now;
    Serial.println("Connecting to Wi-Fi...");
}
}  // namespace

void initWiFi() {
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    // Use one explicit retry policy so monitoring continues while offline.
    WiFi.setAutoReconnect(false);
    startConnection(millis());
}

bool handleWiFi(unsigned long now) {
    if (WiFi.status() == WL_CONNECTED) {
        if (state == ConnectionState::Connected) {
            return false;
        }
        state = ConnectionState::Connected;
        Serial.print("Wi-Fi connected. IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    }

    if (state == ConnectionState::Connected ||
        (state == ConnectionState::Connecting && now - stateStartedMs >= connectTimeoutMs)) {
        WiFi.disconnect();
        state = ConnectionState::Offline;
        stateStartedMs = now;
        Serial.println("Wi-Fi unavailable; retrying in 10 seconds.");
        return true;
    }
    if (state == ConnectionState::Offline && now - stateStartedMs >= retryIntervalMs) {
        startConnection(now);
        return true;
    }
    return false;
}

bool isWiFiConnected() { return WiFi.status() == WL_CONNECTED; }

const char* getWiFiStatusText() {
    switch (state) {
        case ConnectionState::Connecting: return "connecting";
        case ConnectionState::Connected: return "connected";
        default: return "offline (will retry)";
    }
}
