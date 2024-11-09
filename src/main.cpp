#include <Arduino.h>
#include <esp_timer.h>
#include "SpeedStepper.h"

Stepper StepperL(GPIO_NUM_32, GPIO_NUM_33, 0); // Pin 5 for step, Pin 18 for direction, Timer 0
Stepper StepperR(GPIO_NUM_25, GPIO_NUM_26, 1); // Pin 5 for step, Pin 18 for direction, Timer 0

void setup() {
    StepperL.begin();
    StepperR.begin();
    StepperL.setSpeed(1000); // Set motor speed to 200 steps per second
    StepperR.setSpeed(1500); // Set motor speed to 200 steps per second
}
#define t 1000
void loop() {
   StepperL.setSpeed(1500); // Set motor speed to 200 steps per second
   delay(t);
      StepperL.setSpeed(1000); // Set motor speed to 200 steps per second
   delay(t);
      StepperL.setSpeed(500); // Set motor speed to 200 steps per second
   delay(t);
      StepperL.setSpeed(200); // Set motor speed to 200 steps per second
   delay(t);
}