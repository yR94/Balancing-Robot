
// void InitTimer();
// void InitTimer()
// {
//     Timer0_Cfg = timerBegin(0, 8000, true);//clock set to 80 MHz by setting the divder to 800 the gets 100 us increments => 800/80e-6 = 100e-6
//     timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
//     timerAlarmWrite(Timer0_Cfg, 2000, true); // by seting the overflow to 1000 we gets 1000*100e-6 = 10 ms
//     timerAlarmEnable(Timer0_Cfg);

//     Timer1_Cfg = timerBegin(1, 80, true);//clock set to 80 MHz by setting the divder to 800 the gets 100 us increments => 800/80e-6 = 100e-6
//     timerAttachInterrupt(Timer1_Cfg, &Timer1_ISR, true);
//     timerAlarmWrite(Timer1_Cfg, 20, true); // by seting the overflow to 1000 we gets 1000*100e-6 = 10 ms//Max Rate 3000
//     timerAlarmEnable(Timer1_Cfg);
// }
// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "SpeedStepper.h"


hw_timer_t *Timer0_Cfg = NULL;
hw_timer_t *Timer1_Cfg = NULL;
const char* ssid     = "Yosef40";
const char* password = "0545847144";

// Create an instance of the WebServer on port 80
WebServer server(80);





Stepper StepperL(GPIO_NUM_32, GPIO_NUM_33, 0); // Pin 5 for step, Pin 18 for direction, Timer 0
Stepper StepperR(GPIO_NUM_25, GPIO_NUM_26, 1); // Pin 5 for step, Pin 18 for direction, Timer 0

#define Kp 1 //12
#define Kd 1


int lastCmd = 0;

String valueString = String(0);
void handleRoot();

int Vel = 0;
void handleStop() {
  Serial.println("Stop");
StepperL.setSpeed(0);
StepperR.setSpeed(0); 
  server.send(200);
  lastCmd=0;
}
void handleForward() {
  Serial.println("Forward");
StepperL.setSpeed(Vel);
StepperR.setSpeed(Vel);
  server.send(200);
  lastCmd=1;
}

void handleLeft() {
  Serial.println("Left");
StepperL.setSpeed(Vel);
StepperR.setSpeed(-Vel);
  server.send(200);
  lastCmd=2;
}



void handleRight() {
  Serial.println("Right");
StepperL.setSpeed(-Vel);
StepperR.setSpeed(Vel); 
  server.send(200);
  lastCmd=3;
}

void handleReverse() {
  Serial.println("Reverse");
StepperL.setSpeed(-Vel);
StepperR.setSpeed(-Vel);       
  server.send(200);
  lastCmd=4;
}

void handleSpeed() {
  if (server.hasArg("value")) {
    valueString = server.arg("value");
    int value = valueString.toInt();
    
      Vel = map(value, 0, 100, 0, 1000);

      Serial.println("Motor speed set to " + String(value));

      switch (lastCmd)
      {

              case 1:
       handleForward();
        break;
              case 2:
        handleLeft();
        break;
              case 3:
        handleRight();
        break;
              case 4:
        handleReverse();
        break;
      
      default:
       handleStop();
        break;
      }
    
  }
  server.send(200);
}


void setup() {

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    //Wifi setings

    
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Define routes
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/left", handleLeft);
  server.on("/stop", handleStop);
  server.on("/right", handleRight);
  server.on("/reverse", handleReverse);
  server.on("/speed", handleSpeed);

  // Start the server
  server.begin();


    // configure Arduino LED pin for output
    pinMode(BUILTIN_LED, OUTPUT);
   
    StepperL.begin();
    StepperR.begin();
    StepperL.setSpeed(0); // Set motor speed to 200 steps per second
    StepperR.setSpeed(0); // Set motor speed to 200 steps per second
digitalWrite(BUILTIN_LED,HIGH);
   StepperR.Bip();
   StepperR.Bip();
   StepperR.Bip();
    delay(500);

        StepperL.Bip();
        StepperL.Bip();
        StepperL.Bip();
    delay(100);

    
}

bool overclock = 0;

void loop() {

server.handleClient();
analogWrite(BUILTIN_LED,map(Vel,0,100,0,25));

}



void handleRoot() {
  const char html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML><html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
    <link rel="icon" href="data:,">
    <style>
      html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; }
      .button { -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; background-color: #4CAF50; border: none; color: rgb(250, 250, 250); padding: 12px 28px; text-decoration: none; font-size: 26px; margin: 1px; cursor: pointer; }
      .button2 {background-color: #555555;}
    </style>
    <script>
      function moveForward() { fetch('/forward'); }
      function moveLeft() { fetch('/left'); }
      function stopRobot() { fetch('/stop'); }
      function moveRight() { fetch('/right'); }
      function moveReverse() { fetch('/reverse'); }
      function updateMotorSpeed(pos) {
        document.getElementById('motorSpeed').innerHTML = pos;
        fetch(`/speed?value=${pos}`);
      }
    </script>
  </head>
  <body>
    <h1>ESP32 Motor Control</h1>
    <p><button class="button" onclick="moveForward()">FORWARD</button></p>
    <div style="clear: both;">
      <p>
        <button class="button" onclick="moveLeft()">LEFT</button>
        <button class="button button2" onclick="stopRobot()">STOP</button>
        <button class="button" onclick="moveRight()">RIGHT</button>
      </p>
    </div>
    <p><button class="button" onclick="moveReverse()">REVERSE</button></p>
    <p>Motor Speed: <span id="motorSpeed">0</span></p>
    <input type="range" min="0" max="100" step="1" id="motorSlider" oninput="updateMotorSpeed(this.value)" value="0"/>
     <!-- PID Tune -->
    <p>
      <button class="button" onclick="moveLeft()">Kp+</button>
      <button class="button" onclick="moveRight()">Kp-</button>
    </p>
    <p>
      <button class="button" onclick="moveLeft()">Kd+</button>
      <button class="button" onclick="moveRight()">Kd-</button>
    </p>
    <p><button class="button2" onclick="moveForward()">Save</button></p>
    
    <div class="chart-container">
      <!-- IMU Data Plots -->
      <div id="chart-accel"></div>
      <div id="chart-gyro"></div>
    </div>
  </body>
</html>
  
  )rawliteral";
  server.send(200, "text/html", html);
}
