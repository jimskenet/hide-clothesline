#include <Arduino.h>
#include <NetworkSetup.h>
#include <HTTPHandler.h>
#include <ServoControl.h>
#include <RainSensor.h>
#include <ControlTasks.h>

// ---------------- Pin configuration ----------------

#define RAIN_SENSOR_PIN 27
#define SERVO_PIN 13

// ---------------- Servo positions ----------------

#define SERVO_SHELTERED_ANGLE 0
#define SERVO_EXPOSED_ANGLE   90

// ---------------- Global objects ----------------

Servo myServo;

QueueHandle_t rainQueue;
QueueHandle_t manualQueue;
bool currentRainState = false;
SemaphoreHandle_t rainMutex;

// ---------------- Sensor Task ----------------
void SensorTask(void* parameter) {
    bool rainState;

    while (true) {
        rainState = rain_detected(RAIN_SENSOR_PIN);

        // Update the shared rain status.
        if (xSemaphoreTake(rainMutex, portMAX_DELAY) == pdTRUE) {
            currentRainState = rainState;
            xSemaphoreGive(rainMutex);
        }

        // Update the queue for ServoTask.
        xQueueOverwrite(rainQueue, &rainState);

        Serial.print("Rain sensor: ");
        Serial.println(rainState ? "RAIN DETECTED" : "DRY");

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

// ---------------- Servo Task ----------------

void ServoTask(void* parameter) {
    bool rainState = false;
    bool lastRainState = false;

    ManualCommand command;

    int currentAngle = SERVO_EXPOSED_ANGLE;

    // Initialize servo to exposed position.
    myServo.write(currentAngle);

    while (true) {

        // Read newest rain sensor state.
        if (xQueueReceive(rainQueue, &rainState, 0) == pdTRUE) {
            lastRainState = rainState;
        }

        // Rain takes priority over manual commands.
        if (lastRainState) {

            if (currentAngle != SERVO_SHELTERED_ANGLE) {
                currentAngle = SERVO_SHELTERED_ANGLE;
                myServo.write(currentAngle);

                Serial.println("Rain detected -> servo sheltered.");
            }

            // Discard any queued manual commands while raining.
            while (xQueueReceive(manualQueue, &command, 0) == pdTRUE) {
                // Intentionally discard.
            }
        }
        else {
            // Process manual commands only when dry.
            if (xQueueReceive(manualQueue, &command, 0) == pdTRUE) {

                if (command == CMD_DEPLOY) {
                    currentAngle = SERVO_EXPOSED_ANGLE;
                    myServo.write(currentAngle);

                    Serial.println("Manual command -> servo exposed.");
                }

                else if (command == CMD_RETRACT) {
                    currentAngle = SERVO_SHELTERED_ANGLE;
                    myServo.write(currentAngle);

                    Serial.println("Manual command -> servo sheltered.");
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// ---------------- Wi-Fi / HTTP Task ----------------

void WiFiTask(void* parameter) {
    unsigned long lastStatusPrint = 0;

    while (true) {

        // WebServer is accessed ONLY from this task.
        server.handleClient();

        // Print Wi-Fi status periodically without blocking.
        if (millis() - lastStatusPrint >= 10000) {
            lastStatusPrint = millis();

            if (WiFi.status() == WL_CONNECTED) {
                Serial.print("Wi-Fi connected. Router IP: ");
                Serial.println(WiFi.localIP());
            } else {
                Serial.println("Wi-Fi station not connected.");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// ---------------- Setup ----------------

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(RAIN_SENSOR_PIN, INPUT);
   
    rainMutex = xSemaphoreCreateMutex();

    if (rainMutex == nullptr) {
        Serial.println("Failed to create rain mutex!");
        while (true) {
            delay(1000);
        }
    }

    bool initialRainState = rain_detected(RAIN_SENSOR_PIN);
    currentRainState = initialRainState;
        
    myServo.attach(SERVO_PIN);

    // Rain queue holds only the most recent rain state.
    rainQueue = xQueueCreate(1, sizeof(bool));

    // Manual commands can be buffered.
    manualQueue = xQueueCreate(5, sizeof(ManualCommand));

    if (rainQueue == nullptr || manualQueue == nullptr) {
        Serial.println("Failed to create queues!");
        while (true) {
            delay(1000);
        }
    }

    xQueueOverwrite(rainQueue, &initialRainState);

    // Start AP and initiate saved Wi-Fi connection without waiting.
    startSavedWiFiConnection();

    // Register routes and start the web server.
    setupServer();

    // Create independent FreeRTOS tasks.
    xTaskCreate(
        SensorTask,
        "SensorTask",
        2048,
        nullptr,
        2,
        nullptr
    );

    xTaskCreate(
        ServoTask,
        "ServoTask",
        2048,
        nullptr,
        3,
        nullptr
    );

    xTaskCreate(
        WiFiTask,
        "WiFiTask",
        4096,
        nullptr,
        1,
        nullptr
    );

    Serial.println("All tasks started.");
}

// ---------------- Arduino loop ----------------

void loop() {
    // FreeRTOS tasks handle the actual work.
    vTaskDelay(portMAX_DELAY);
}