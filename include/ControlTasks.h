#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

enum ManualCommand : uint8_t {
    CMD_DEPLOY,
    CMD_RETRACT
};

extern QueueHandle_t rainQueue;
extern QueueHandle_t manualQueue;

void SensorTask(void* parameter);
void ServoTask(void* parameter);
void WiFiTask(void* parameter);