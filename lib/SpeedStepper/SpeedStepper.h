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
    void setSpeed(float Speed);

    // Set the timer periods dynamically
    void Bip();


    int64_t get_cnt();

private:
    // GPIO pins
    gpio_num_t StepPin;
    gpio_num_t DirPin;
    int TimerNum;
    int64_t cnt;
    float Speed;
    int dir;
    

    // Static pointers to an instance of Stepper for each motor
    static Stepper* instance0;  // For Timer 0
    static Stepper* instance1;  // For Timer 1
    
    // Timer handles
    hw_timer_t *timer = NULL;

    // Static Timer ISR functions for each timer
    static void IRAM_ATTR Timer_ISR0();
    static void IRAM_ATTR Timer_ISR1();
};


#endif
