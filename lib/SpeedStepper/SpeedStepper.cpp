#include "SpeedStepper.h"
// Initialize the static instance pointers to null
Stepper* Stepper::instance0 = nullptr;
Stepper* Stepper::instance1 = nullptr;

Stepper::Stepper(gpio_num_t stepPin, gpio_num_t dirPin, int TimerNumber)
    : StepPin(stepPin), DirPin(dirPin), TimerNum(TimerNumber), cnt(0), Speed(0) {}

void Stepper::begin() {
    // Set the static instance pointer to this instance based on the timer number
    if (TimerNum == 0) {
        instance0 = this;
    } else if (TimerNum == 1) {
        instance1 = this;
    }

    // Set GPIO pins to OUTPUT mode
    pinMode(StepPin, OUTPUT);
    pinMode(DirPin, OUTPUT);

    // Setup the timer (this example uses timer 0 or timer 1)
    if (TimerNum == 0) {
        timer = timerBegin(TimerNum, 8, true);  // 80 is the prescaler
        timerAttachInterrupt(timer, Timer_ISR0, true);
    } else if (TimerNum == 1) {
        timer = timerBegin(TimerNum, 8, true);  // 80 is the prescaler
        timerAttachInterrupt(timer, Timer_ISR1, true);
    }
    
    timerAlarmWrite(timer, 1000, true); // 1000 microseconds period (adjust for speed)
    timerAlarmEnable(timer);            // Enable the timer
}

void Stepper::setSpeed(float Speed) {

    // Adjust the timer period based on the speed
    if(Speed==0) timerAlarmDisable(timer);
    else
    {
    timerAlarmEnable(timer);
    uint64_t period =  abs(1e6/Speed);  // Convert speed to period (in microseconds)
    Speed<0?digitalWrite(DirPin,LOW):digitalWrite(DirPin,HIGH);
    timerAlarmWrite(timer, period, true); // Update timer alarm with new period
    }
}

void Stepper::Bip() {

setSpeed(7000);
delay(40);
setSpeed(0); // Set motor speed to 200 steps per second
}

int Stepper::get_cnt() {
    return cnt;
}

// Timer ISR for Timer 0
void IRAM_ATTR Stepper::Timer_ISR0() {
    if (instance0 != nullptr) {
        digitalWrite(instance0->StepPin, HIGH);
        digitalWrite(instance0->StepPin, LOW);
        instance0->Speed>=0?instance0->cnt++:instance0->cnt--;
    
    }
}

// Timer ISR for Timer 1
void IRAM_ATTR Stepper::Timer_ISR1() {
    if (instance1 != nullptr) {
        digitalWrite(instance1->StepPin, HIGH);
        digitalWrite(instance1->StepPin, LOW);
        instance1->Speed>=0?instance1->cnt++:instance1->cnt--;
    }
}
