#include <Arduino.h>
#include <esp_timer.h>


bool LedOn=0;
long t=100;
int singe =-1;
hw_timer_t *Timer0_Cfg = NULL;
hw_timer_t *Timer1_Cfg = NULL;

uint64_t cnt=0,maxcnt=2;//maxcnt=50~600 fast  760~1100 slow

void RunLeftSpeed(float speed)
{

speed>0 ? digitalWrite(GPIO_NUM_33,  HIGH) : digitalWrite(GPIO_NUM_33,  LOW); 
maxcnt=constrain(map(abs(speed),0,100,900,30),30,900);
if (speed==0)maxcnt=-1; // fully stop
 

 


};
 
void IRAM_ATTR Timer0_ISR()
{
  
    digitalWrite(LED_BUILTIN, LedOn ? HIGH : LOW);
    LedOn = !LedOn;
  //if(LedOn)maxcnt=0;
  //else maxcnt=550;
  
  t=t+singe;
t>0 ? digitalWrite(GPIO_NUM_33,  HIGH) : digitalWrite(GPIO_NUM_33,  LOW);  
maxcnt=constrain(map(abs(t),0,100,500,30),30,500);//speed mode;
//maxcnt=constrain(map(abs(t),0,100,80000,700),700,80000);//slow mode;

if (abs(t)<=0.1)maxcnt=-1; // fully stop
 

  if (t>5) singe=-1;
  if (t<-5) singe=1;
 
  
}


void IRAM_ATTR Timer1_ISR()
{


  if (cnt>maxcnt)
{
 digitalWrite(GPIO_NUM_32,  HIGH);
 digitalWrite(GPIO_NUM_32, LOW);
 cnt=0;
}
cnt++;
  
}



void setup()
{
  pinMode(GPIO_NUM_33 ,OUTPUT);
  pinMode(GPIO_NUM_32 ,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
Serial.begin(9600);


    
    Timer0_Cfg = timerBegin(0, 800, true);//clock set to 80 MHz by setting the divder to 800 the gets 100 us increments => 800/80e-6 = 100e-6
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 10000, true); // by seting the overflow to 1000 we gets 1000*100e-6 = 10 ms
    timerAlarmEnable(Timer0_Cfg);
  
    Timer1_Cfg = timerBegin(1, 80, true);//clock set to 80 MHz by setting the divder to 800 the gets 100 us increments => 800/80e-6 = 100e-6
    timerAttachInterrupt(Timer1_Cfg, &Timer1_ISR, true);
    timerAlarmWrite(Timer1_Cfg, 20, true); // by seting the overflow to 1000 we gets 1000*100e-6 = 10 ms//Max Rate 3000
    timerAlarmEnable(Timer1_Cfg);


}
 

void loop()
{

 Serial.println(maxcnt);
   
}
