#include <ServoControl.h>

void move_zero(Servo* myServo) {
  myServo->write(0); // Move the servo to 0 degrees
}

void move_ninety(Servo* myServo) {
  myServo->write(90); // Move the servo to 90 degrees
}

void move_one_eighty(Servo* myServo) {
  myServo->write(180); // Move the servo to 180 degrees
}