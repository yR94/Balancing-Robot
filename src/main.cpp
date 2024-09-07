#include <Arduino.h>
#include <Wire.h>

# define MPU_addr 0x68

#define Acc_SF   8192.0
#define Gyro_SF   43.6

uint32_t i =0;
hw_timer_t *Timer0_Cfg = NULL;
float ax=0,ay=0,az=0,gx=0,gy=0,gz=0;
float pitch=0,roll=0,yaw=0;
float AccPitch=0,AccRoll=0;
float GyroPitch=0,GyroRoll=0,GyroYaw=0;
unsigned long T = 0;
float dt = 0.005;


void IMU_init();
void IMU_Read();
void InitTimer();

void IRAM_ATTR Timer0_ISR()
{

  i++;


digitalWrite(LED_BUILTIN,i%2==0 ? HIGH : LOW);





}




/////////////////
void setup() {
  // put your setup code here, to run once:
 pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.begin(9600);

   IMU_init();
   InitTimer();

   


   
 


}


void InitTimer()
{
    Timer0_Cfg = timerBegin(0, 8000, true);//clock set to 80 MHz by setting the divder to 800 the gets 100 us increments => 800/80e-6 = 100e-6
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 2000, true); // by seting the overflow to 1000 we gets 1000*100e-6 = 10 ms
    timerAlarmEnable(Timer0_Cfg);
}
void loop() {

IMU_Read();

GyroPitch = pitch+gz*dt;
AccPitch = -atan2(ay,ax)*180/PI;

pitch = 0.96*GyroPitch + 0.04*AccPitch;


Serial.println(pitch);
//Serial.print(micros()-T);
//
while (micros()-T<=dt*100000)
{
 
}

//Serial.println(micros()-T);
T = micros() ;



}


void IMU_Read()
{
  int16_t ax_raw,ay_raw,az_raw,gx_raw,gy_raw,gz_raw;

Wire.beginTransmission(MPU_addr);
Wire.write(0x3B);
Wire.endTransmission(); 
Wire.requestFrom(MPU_addr,6,true);
ax_raw=Wire.read()<<8|Wire.read();
ay_raw=Wire.read()<<8|Wire.read();
az_raw=Wire.read()<<8|Wire.read();

Wire.beginTransmission(MPU_addr);                                   //Start communication with the gyro Wire.write(0x43); 
Wire.write(0x43);                                                        //Start reading the Who_am_I register 75h
Wire.endTransmission();                                                 //End the transmission
Wire.requestFrom(MPU_addr,6,true);
gx_raw = (Wire.read() << 8 | Wire.read()) -48; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
gy_raw = (Wire.read() << 8 | Wire.read()) +58;
gz_raw = (Wire.read() << 8 | Wire.read()) -235;

ax = ax_raw/Acc_SF;
ay = ay_raw/Acc_SF;
az = az_raw/Acc_SF;
gx = gx_raw/Gyro_SF;
gy = gy_raw/Gyro_SF;
gz = gz_raw/Gyro_SF;



/*

Serial.print(ax);
Serial.print(' ');
Serial.print(ay);
Serial.print(' ');
Serial.print(az);
Serial.print(' ');
Serial.print(gx); 
Serial.print(' ');
Serial.print(gy);
Serial.print(' ');
Serial.println(gz);
Serial.println("----------------------");
*/
}


void IMU_GetAngels()
{
  
}


void IMU_init()
{

  
 Wire.begin();
  //By default the MPU-6050 sleeps. So we have to wake it up.
  Wire.beginTransmission(MPU_addr);                                     //Start communication with the address found during search.
  Wire.write(0x6B);                                                         //We want to write to the PWR_MGMT_1 register (6B hex)
  Wire.write(0x00);                                                         //Set the register bits as 00000000 to activate the gyro
  Wire.endTransmission();                                                   //End the transmission with the gyro.
  //Set the full scale of the gyro to +/- 250 degrees per second
  Wire.beginTransmission(MPU_addr);                                     //Start communication with the address found during search.
  Wire.write(0x1B);                                                         //We want to write to the GYRO_CONFIG register (1B hex)
  Wire.write(0x00);                                                         //Set the register bits as 00000000 (250dps full scale)
  Wire.endTransmission();                                                   //End the transmission with the gyro
  //Set the full scale of the accelerometer to +/- 4g.
  Wire.beginTransmission(MPU_addr);                                     //Start communication with the address found during search.
  Wire.write(0x1C);                                                         //We want to write to the ACCEL_CONFIG register (1A hex)
  Wire.write(0x00);                                                         //Set the register bits as 00001000 (+/- 4g full scale range)
  Wire.endTransmission();                                                   //End the transmission with the gyro
  //Set some filtering to improve the raw data.
  Wire.beginTransmission(MPU_addr);                                     //Start communication with the address found during search
  Wire.write(0x1A);                                                         //We want to write to the CONFIG register (1A hex)
  Wire.write(0x03);                                                         //Set the register bits as 00000011 (Set Digital Low Pass Filter to ~43Hz)
  Wire.endTransmission();  

}