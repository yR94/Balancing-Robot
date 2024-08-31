#include <Arduino.h>


uint32_t i =0;

int dir=1;


//////////////////
void setup() {
  // put your setup code here, to run once:
 pinMode(LED_BUILTIN,OUTPUT);
 pinMode(GPIO_NUM_32,OUTPUT);
 pinMode(GPIO_NUM_33,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.begin(9600);




   delay(1000);
 


}


int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void loop() {
i++;


digitalWrite(LED_BUILTIN,i%2==0 ? HIGH : LOW);

digitalWrite(GPIO_NUM_32,i%2==0 ? HIGH : LOW);// Step

if(i%1500==0)dir= !dir;

digitalWrite(GPIO_NUM_33,dir ? HIGH : LOW);// diraction



delay(1);

}
