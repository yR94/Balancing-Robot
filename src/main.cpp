#include <Arduino.h>
#include <esp_timer.h>


bool LedOn=0;
long t;

hw_timer_t *Timer0_Cfg = NULL;
 
void IRAM_ATTR Timer0_ISR()
{
  LedOn = !LedOn;
    digitalWrite(LED_BUILTIN, LedOn ? HIGH : LOW);
   digitalWrite(GPIO_NUM_32,  HIGH);
   digitalWrite(GPIO_NUM_32, LOW);
}
void setup()
{
  pinMode(GPIO_NUM_32 ,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
Serial.begin(9600);
  
    Timer0_Cfg = timerBegin(0, 80, true);//clock set to 80 MHz by setting the divder to 800 the gets 100 us increments => 800/80e-6 = 100e-6
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 1000, true); // by seting the overflow to 1000 we gets 1000*100e-6 = 10 ms
    timerAlarmEnable(Timer0_Cfg);
}
 

void loop()
{
  delay(1000);
      timerAlarmWrite(Timer0_Cfg, 1000, true);
 delay(1000);
      timerAlarmWrite(Timer0_Cfg, 2000, true);

   
}
