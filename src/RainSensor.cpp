#include <RainSensor.h>

int rain_detected(int rain_pin){
    return digitalRead(rain_pin) == LOW;
}
