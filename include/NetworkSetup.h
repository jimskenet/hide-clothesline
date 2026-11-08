#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

extern const char* AP_SSID;
extern const char* AP_PASSWORD;

extern const IPAddress AP_IP;
extern const IPAddress AP_GATEWAY;
extern const IPAddress AP_SUBNET;

extern const unsigned long WIFI_TIMEOUT_MS;

extern WebServer server;
extern Preferences preferences;

void startSetupAP();
void startSavedWiFiConnection();
void beginWiFiConnection(const String& ssid, const String& password);