#ifndef SpeedStepper
#define SpeedStepper

#include <Arduino.h>
#include <esp_timer.h>
class Stepper {
public:
    // Constructor
    Stepper(gpio_num_t stepPin, gpio_num_t dirPin, int TimerNumber);
    
    // Initialize the timers
    void begin();
    
    // Set the timer periods dynamically
    void setSpeed(int Speed);
    int get_cnt();

private:
    // GPIO pins
    gpio_num_t StepPin;
    gpio_num_t DirPin;
    int TimerNum;
    int cnt;
    int Speed;

    // Static pointer to an instance of Stepper
    static Stepper* instance;  // Static pointer to the current instance
    
    // Timer handles
    hw_timer_t *timer = NULL;

    // Timer interrupt service routines
    static void IRAM_ATTR Timer_ISR();
    
};

#endif
