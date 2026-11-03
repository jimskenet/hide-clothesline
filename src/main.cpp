#include <Arduino.h>
#include <NetworkSetup.h>
#include <HTTPHandler.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  startSavedWiFiConnection();

  setupServer();
}

void loop() {
  server.handleClient();
}