#include <Arduino.h>
// #include <NetworkSetup.h>
// #include <HTTPHandler.h>
// #include <ServoControl.h>
// #include <RainSensor.h>

// #define RAIN_SENSOR_PIN 27
// Servo myServo; 

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   // startSavedWiFiConnection();

//   // setupServer();

//   myServo.attach(13);
//   pinMode(RAIN_SENSOR_PIN, INPUT);
// }

// void loop() {
//   // server.handleClient();
//   Serial.println("Checking rain sensor...");
//   Serial.print(digitalRead(RAIN_SENSOR_PIN));
//   Serial.println();

//   if(rain_detected(RAIN_SENSOR_PIN)) {
//     move_zero(&myServo);
//   }
//   else {
//     move_ninety(&myServo);                                
//   }
//   delay(500);
// }                             

/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-rain-sensor
 */

#define DO_PIN 27
#define AO_PIN 34

void setup() {
  Serial.begin(115200);

  analogReadResolution(12);
  analogSetPinAttenuation(AO_PIN, ADC_11db);

  pinMode(DO_PIN, INPUT);
}

void loop() {
  int rain_state = digitalRead(DO_PIN);
  int rain_value = analogRead(AO_PIN);

  Serial.print("Digital: ");
  Serial.print(rain_state);

  Serial.print(" | Analog: ");
  Serial.println(rain_value);

  delay(1000);
}
