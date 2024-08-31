#include <Arduino.h>
#include <Wire.h>

# define MPU_addr 0x68

uint32_t i =0;
hw_timer_t *Timer0_Cfg = NULL;



//////////////////
void setup() {
  // put your setup code here, to run once:
 pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.begin(9600);

    Wire.begin();
    Wire.beginTransmission(0x68);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);



   delay(1000);
 


}


int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void loop() {
i++;


digitalWrite(LED_BUILTIN,i%2==0 ? HIGH : LOW);


Wire.beginTransmission(MPU_addr);
Wire.write(0x3B);
Wire.endTransmission(false);
Wire.requestFrom(MPU_addr,6,true);
AcX=Wire.read()<<8|Wire.read();
AcY=Wire.read()<<8|Wire.read();
AcZ=Wire.read()<<8|Wire.read();
Wire.requestFrom(MPU_addr,6,true);
GyX = (Wire.read() << 8 | Wire.read()) ; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
GyY = (Wire.read() << 8 | Wire.read()) ;
GyZ = (Wire.read() << 8 | Wire.read()) ;


Serial.print(AcX);
Serial.print(' ');
Serial.print(AcY);
Serial.print(' ');
Serial.print(AcZ);
Serial.print(' ');
Serial.print(GyX);
Serial.print(' ');
Serial.print(GyY);
Serial.print(' ');
Serial.println(GyZ);




delay(500);

}
