#ifndef NETWORK_SETUP_H
#define NETWORK_SETUP_H

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>

// Configuration

extern const char* AP_SSID;
extern const char* AP_PASSWORD;

extern const IPAddress AP_IP;
extern const IPAddress AP_GATEWAY;
extern const IPAddress AP_SUBNET;

extern const unsigned long WIFI_TIMEOUT_MS;

// Globals

extern WebServer server;
extern Preferences preferences;

extern String pendingSSID;
extern String pendingPassword;

extern bool connectionPending;
extern bool credentialsSaved;

extern unsigned long connectionStartTime;

// Wi-Fi setup
void startSetupAP();
void startSavedWiFiConnection();


#endif