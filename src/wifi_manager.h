#pragma once

void initWiFi();
bool handleWiFi(unsigned long now);
bool isWiFiConnected();
const char* getWiFiStatusText();
