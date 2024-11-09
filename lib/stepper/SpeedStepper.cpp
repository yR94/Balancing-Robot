#include "SpeedStepper.h"

// Initialize the static instance pointer to null
Stepper* Stepper::instance = nullptr;

Stepper::Stepper(gpio_num_t stepPin, gpio_num_t dirPin, int TimerNumber)
    : StepPin(stepPin), DirPin(dirPin), TimerNum(TimerNumber), cnt(0), Speed(0) {}

void Stepper::begin() {
    // Set the static instance pointer to this instance
    instance = this;

    // Set GPIO pins to OUTPUT mode
    pinMode(StepPin, OUTPUT);
    pinMode(DirPin, OUTPUT);

    // Setup the timer (this example uses timer 0)
    timer = timerBegin(TimerNum, 80, true); // 80 is the prescaler (adjust for timing)
    timerAttachInterrupt(timer, Timer_ISR, true);
    timerAlarmWrite(timer, 1000, true); // 1000 microseconds period (adjust for speed)
    timerAlarmEnable(timer); // Enable the timer
}

void Stepper::setSpeed(int Speed) {
    // Adjust the timer period based on the speed
    int period = 1000000 / Speed;  // Convert speed to period (in microseconds)
    timerAlarmWrite(timer, period, true); // Update timer alarm with new period
}

int Stepper::get_cnt() {
    return cnt;
}

void IRAM_ATTR Stepper::Timer_ISR() {
    // Use the static instance pointer to access non-static members
    if (instance != nullptr) {
        digitalWrite(instance->StepPin, HIGH);
        digitalWrite(instance->StepPin, LOW);
    }
}
