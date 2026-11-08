#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <NetworkSetup.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern bool currentRainState;
extern SemaphoreHandle_t rainMutex;

// HTTP handlers    
void handleRoot();
void handleScan();
void handleConnect();
void handleStatus();
void handleReset();

// Server setup
void setupServer();

#endif