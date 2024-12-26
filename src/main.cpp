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

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KdND, EXPRESS OR
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
#include "I2Cdev.h"
#include "MPU6050.h"
#include "SpeedStepper.h"
#include <WiFi.h>
#include <WebServer.h>
#include "SpeedStepper.h"



const char* ssid     = "Yosef40";
const char* password = "0545847144";



// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high
//MPU6050 accelgyro(0x68, &Wire1); // <-- use for AD0 low, but 2nd Wire (TWI/I2C) object

Stepper StepperL(GPIO_NUM_32, GPIO_NUM_33, 0); // Pin 5 for step, Pin 18 for direction, Timer 0
Stepper StepperR(GPIO_NUM_25, GPIO_NUM_26, 1); // Pin 5 for step, Pin 18 for direction, Timer 0

WebServer server(80);



float Kp = 380.0; //12
float LPF = 0.04;
float offset = 0;
float Kd = 0.0;
float PosKd = 16;
float PosKp = 3;

int16_t ax, ay, az;
int16_t gx, gy, gz;


String valueString = String(0);
void handleRoot();



unsigned long T;
float GyroPitch=0;
float AccPitch=0; 
float Pitch; 
float a=0.01;
float SF = 2.0*131/32767.0;

float PosRef = 0;
float PosSpeed = 0;

float RotSpeedL = 0;
float RotSpeedR = 0;

// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO
int Ts  = 5000;
// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO


#define LED_PIN LED_BUILTIN
bool blinkState = false;
void handleCurrentPitch() {
  // Send the current pitch value as a response
  String pitchString = String(Pitch);
  server.send(200, "text/plain", pitchString);  // Return the pitch as plain text
}
void handleOffse() {
  if (server.hasArg("value")) {
    valueString = server.arg("value");
    offset = valueString.toFloat();
   
  }
  server.send(200);
}

void handleKp() {
  if (server.hasArg("value")) {
    valueString = server.arg("value");
    PosKp = valueString.toFloat();
  }
  server.send(200);
}

void handleLPF() {
  if (server.hasArg("value")) {
    valueString = server.arg("value");
    PosKd = valueString.toFloat();
  }
  server.send(200);
}

void handleKd() {
  if (server.hasArg("value")) {
    valueString = server.arg("value");
    Kd = valueString.toFloat();
  }
  server.send(200);
}



void handleForward() {
PosSpeed+=10;
  server.send(200);
}

void handleLeft() {
 RotSpeedL += 10;
 RotSpeedR -= 10;
  server.send(200);
}

void handleStope() {
PosSpeed=0;
 RotSpeedL = 0;
 RotSpeedR = 0;
 Serial.println("Stop!");
  server.send(200);
}

void handleReverse() {
PosSpeed-=10;
  server.send(200);
}
void handleRight() {
 RotSpeedL -= 10;
 RotSpeedR += 10;
  server.send(200);
}




void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
   

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
  server.on("/Kp", HTTP_GET, handleKp);
  server.on("/LPF", HTTP_GET, handleLPF);
  server.on("/Offse", HTTP_GET, handleOffse);
  server.on("/currentPitch", HTTP_GET, handleCurrentPitch);

  server.on("/forward", HTTP_GET, handleForward);
  server.on("/left", HTTP_GET, handleLeft);
  server.on("/stop", HTTP_GET, handleStope);
  server.on("/reverse", HTTP_GET, handleReverse);
  server.on("/right", HTTP_GET, handleRight);

  
  // Start the server
  server.begin();



    // configure Arduino LED pin for output
    pinMode(LED_PIN, OUTPUT);
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    Pitch = atan2(az,-ax)*180/PI;

    StepperL.begin();
    StepperR.begin();
    StepperL.setSpeed(0); // Set motor speed to 200 steps per second
    StepperR.setSpeed(0); // Set motor speed to 200 steps per second

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
float error=0;
float controlSignal=0;
float linearDist = 0;



void loop() {
server.handleClient();
    if(overclock==0)
    {
  T=micros();
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
 

 AccPitch = atan2(az,-ax)*180/PI;
// GyroPitch=0.95*GyroPitch+0.05*gy;
 
 Pitch  = a*AccPitch +(1-a)*(Pitch-SF*gy*Ts*1e-6);

 PosRef=PosRef+PosSpeed;

linearDist =0.95*linearDist -0.05*constrain(((StepperL.get_cnt()-StepperR.get_cnt())/2-PosRef)*0.001*PosKp+0.001*controlSignal*PosKd,-8,8);

 error = (Pitch-offset-linearDist);


  controlSignal = (error*Kp)*LPF+(1-LPF)*controlSignal;

  StepperL.setSpeed(controlSignal+RotSpeedL);
  StepperR.setSpeed(-controlSignal-RotSpeedR);


//linearDist = 0;

Serial.print(linearDist);
 Serial.print(' ');
 Serial.print(Pitch);
 Serial.print(' ');
Serial.println(error);



    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
    //Serial.println(micros()-T);

    overclock = 1;
    while (micros()-T<Ts)
    {
      overclock = 0;
    }
    }
    else 
    {
      Serial.println("Overflow");
    StepperL.setSpeed(0); // Set motor speed to 200 steps per second
    StepperR.setSpeed(0); // Set motor speed to 200 steps per second
    }

    if(abs(Pitch)>30)
    {
    StepperL.setSpeed(0); // Set motor speed to 200 steps per second
    StepperR.setSpeed(0); // Set motor speed to 200 steps per second
    
    }

}



void handleRoot() {
  const char html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
    <link rel="icon" href="data:,">
    <style>
        html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; }
        .button { background-color: #4CAF50; border: none; color: rgb(250, 250, 250); padding: 12px 28px; text-decoration: none; font-size: 26px; margin: 1px; cursor: pointer; }
        .button2 {background-color: #555555;}
        .slider-container { margin-top: 20px; }
        .slider { width: 100%; max-width: 600px; }
    </style>
    <script>
        function moveForward() { fetch('/forward'); }
        function moveLeft() { fetch('/left'); }
        function stopRobot() { fetch('/stop'); }
        function moveRight() { fetch('/right'); }
        function moveReverse() { fetch('/reverse'); }
        

        function updateOffset(pos) {
            document.getElementById('Offset').innerHTML = pos;
            fetch(`/Offse?value=${pos}`);
        }

        function updateKp(pos) {
            document.getElementById('KpValue').innerHTML = pos;
            fetch(`/Kp?value=${pos}`);
        }

        function updateLPF(pos) {
            document.getElementById('LPFValue').innerHTML = pos;
            fetch(`/LPF?value=${pos}`);
        }

        // Fetch the current pitch and update the display
        function fetchCurrentPitch() {
            fetch('/currentPitch')
                .then(response => response.text())
                .then(pitch => {
                    document.getElementById('currentPitch').innerHTML = pitch;  // Update pitch on the page
                });
        }

        // Fetch the current pitch every second
        setInterval(fetchCurrentPitch, 500);
    </script>
</head>
<body>
    <h1>ESP32 Motor Control</h1>
    <p><button class="button" onclick="moveForward()">FORWARD</button></p>
    <div style="clear: both;">
        <p>
            <button class="button" onclick="moveLeft()">LEFT</button>
            <button class="button" onclick="stopRobot()">STOP</button>
            <button class="button" onclick="moveRight()">RIGHT</button>
        </p>
    </div>
    <p><button class="button" onclick="moveReverse()">REVERSE</button></p>

    <!-- Slider for offset (pitch) -->
    <div class="slider-container">
        <p>Pitch Offset: <span id="Offset">0</span></p>
        <input type="range" min="-10" max="10" step="0.05" id="motorSlider" oninput="updateOffset(this.value)" value="0" class="slider">
    </div>

    <!-- Sliders for Kp and LPF -->
    <div class="slider-container">
        <p>Kp: <span id="KpValue">8.4</span></p>
        <input type="range" min="0" max="40" step="0.0001" id="KpSlider" oninput="updateKp(this.value)" value="84.0" class="slider">
    </div>
    
    <div class="slider-container">
        <p>LPF: <span id="LPFValue">0.0</span></p>
        <input type="range" min="0" max="40" step="0.0001" id="LPFSlider" oninput="updateLPF(this.value)" value="0.95" class="slider">
    </div>

    <!-- Display current pitch value -->
    <div class="slider-container">
        <p>Current Pitch: <span id="currentPitch">0</span>°</p>
    </div>

    <!-- PID Tune -->
    <p>
        <button class="button" onclick="moveLeft()">Kp+</button>
        <button class="button" onclick="moveRight()">Kp-</button>
    </p>
    <p>
        <button class="button" onclick="moveLeft()">Kd+</button>
        <button class="button" onclick="moveRight()">Kd-</button>
    </p>
    
    <div class="chart-container">
        <!-- IMU Data Plots (empty for now) -->
        <div id="chart-accel"></div>
        <div id="chart-gyro"></div>
    </div>
</body>
</html>


  
  )rawliteral";
  server.send(200, "text/html", html);
}




