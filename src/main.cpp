#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <esp_http_server.h>
#include "Arduino.h"

// Replace with your network credentials
const char* ssid = "Yosef40";
const char* password = "0545847144";

// Set up the IMU (MPU6050 in this case)
MPU6050 mpu;

uint16_t ax,ay,az,gx,gy,gz;
// HTTP server handle
httpd_handle_t server = NULL;

// Web interface
static const char* HTML_PAGE = R"rawliteral(

<html>
  <head>
    <title>ESP32 IMU Data and Car Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
    <style>
      body { 
        font-family: Arial; 
        text-align: center; 
        margin: 0 auto; 
        padding-top: 30px; 
        display: flex;
        flex-direction: row;
        justify-content: space-between;
        align-items: center;
      }

      /* IMU Data Plots */
      #chart-accel, #chart-gyro { 
        width: 800px; 
        height:300px; 
        margin: 20px auto;
      }

      .chart-container {
        display: flex;
        justify-content: space-between;
        padding: 20px;
        flex-direction: column;
        align-items: center;
      }

      /* Control panel */
      .control-panel {
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        margin-right: 20px;
      }

      /* Rose-style Button Container */
      .button-container {
        position: relative;
        width: 200px;
        height: 200px;
        border-radius: 50%;
        border: 2px solid #2f4468;
        margin: 20px auto;
      }

      .control-button {
        position: absolute;
        width: 50px;
        height: 50px;
        background-color: #2f4468;
        border-radius: 50%;
        color: white;
        font-weight: bold;
        display: flex;
        align-items: center;
        justify-content: center;
        cursor: pointer;
        transition: all 0.3s ease;
      }

      .control-button:hover {
        background-color: #1c2e47;
      }

      /* Scroll bars */
      .scroll-bars {
        display: flex;
        flex-direction: column;
        gap: 20px;
      }

      .scroll-bar {
        width: 200px;
        height: 20px;
      }

      /* Mode Selector */
      .mode-selector {
        margin-left: 20px;
        padding: 10px;
        width: 150px;
      }

      /* General purpose buttons */
      .general-buttons {
        margin-top: 20px;
        display: flex;
        flex-direction: column;
        gap: 10px;
      }

      .general-button {
        width: 120px;
        height: 40px;
        background-color: #2f4468;
        color: white;
        font-weight: bold;
        border: none;
        border-radius: 5px;
        cursor: pointer;
        transition: background-color 0.3s;
      }

      .general-button:hover {
        background-color: #1c2e47;
      }
    </style>
  </head>
  <body>
    <div class="chart-container">
      <!-- IMU Data Plots -->
      <div id="chart-accel"></div>
      <div id="chart-gyro"></div>
    </div>

    <!-- Control Panel -->
    <div class="control-panel">
      <!-- Rose Button Layout -->
      <div class="button-container">
        <div id="up" class="control-button" style="top: 10px; left: 75px;">Forward</div>
        <div id="down" class="control-button" style="bottom: 10px; left: 75px;">Back</div>
        <div id="left" class="control-button" style="top: 75px; left: 10px;">Right</div>
        <div id="right" class="control-button" style="top: 75px; right: 10px;">Left</div>
        <div id="stop" class="control-button" style="top: 75px; left: 75px;">Stop</div>
      </div>

      <!-- Mode Selector -->
      <select class="mode-selector" id="mode-selector">
        <option value="manual">Manual</option>
        <option value="automatic">Automatic</option>
        <option value="maintenance">Maintenance</option>
      </select>

      <!-- Scroll Bars -->
      <div class="scroll-bars">
        <input type="range" class="scroll-bar" min="0" max="100" value="50" id="speed-slider">
        <input type="range" class="scroll-bar" min="0" max="100" value="50" id="turn-slider">
      </div>

      <!-- General Purpose Buttons -->
      <div class="general-buttons">
        <button class="general-button" id="button1">Button 1</button>
        <button class="general-button" id="button2">Button 2</button>
        <button class="general-button" id="button3">Button 3</button>
        <button class="general-button" id="button4">Button 4</button>
      </div>
    </div>

    <script>
      var imuDataAccel = {
        x: [],
        y: [],
        z: [],
        time: []
      };

      var imuDataGyro = {
        x: [],
        y: [],
        z: [],
        time: []
      };

      var layoutAccel = {
        title: 'Accelerometer Data',
        xaxis: { title: 'Time' },
        yaxis: { title: 'Acceleration (m/s^2)' },
        showlegend: true
      };

      var layoutGyro = {
        title: 'Gyroscope Data',
        xaxis: { title: 'Time' },
        yaxis: { title: 'Gyroscope (deg/s)' },
        showlegend: true
      };

      Plotly.newPlot('chart-accel', [{
        type: 'scatter',
        mode: 'lines+markers',
        name: 'X Accel',
        x: imuDataAccel.time,
        y: imuDataAccel.x,
      }, {
        type: 'scatter',
        mode: 'lines+markers',
        name: 'Y Accel',
        x: imuDataAccel.time,
        y: imuDataAccel.y,
      }, {
        type: 'scatter',
        mode: 'lines+markers',
        name: 'Z Accel',
        x: imuDataAccel.time,
        y: imuDataAccel.z,
      }], layoutAccel);

      Plotly.newPlot('chart-gyro', [{
        type: 'scatter',
        mode: 'lines+markers',
        name: 'X Gyro',
        x: imuDataGyro.time,
        y: imuDataGyro.x,
      }, {
        type: 'scatter',
        mode: 'lines+markers',
        name: 'Y Gyro',
        x: imuDataGyro.time,
        y: imuDataGyro.y,
      }, {
        type: 'scatter',
        mode: 'lines+markers',
        name: 'Z Gyro',
        x: imuDataGyro.time,
        y: imuDataGyro.z,
      }], layoutGyro);

      function updateGraph(data) {
        imuDataAccel.time.push(data.time);
        imuDataAccel.x.push(data.ax);
        imuDataAccel.y.push(data.ay);
        imuDataAccel.z.push(data.az);

        imuDataGyro.time.push(data.time);
        imuDataGyro.x.push(data.gx);
        imuDataGyro.y.push(data.gy);
        imuDataGyro.z.push(data.gz);

        if (imuDataAccel.time.length > 100) {
          imuDataAccel.time.shift();
          imuDataAccel.x.shift();
          imuDataAccel.y.shift();
          imuDataAccel.z.shift();

          imuDataGyro.time.shift();
          imuDataGyro.x.shift();
          imuDataGyro.y.shift();
          imuDataGyro.z.shift();
        }

        Plotly.update('chart-accel', {
          x: [imuDataAccel.time, imuDataAccel.time, imuDataAccel.time],
          y: [imuDataAccel.x, imuDataAccel.y, imuDataAccel.z],
        });

        Plotly.update('chart-gyro', {
          x: [imuDataGyro.time, imuDataGyro.time, imuDataGyro.time],
          y: [imuDataGyro.x, imuDataGyro.y, imuDataGyro.z],
        });
      }

      setInterval(() => {
        fetch('/get_imu_data')
          .then(response => response.json())
          .then(data => updateGraph(data));
      }, 100);  // Adjust to faster updates

      // Control button actions
      function sendControlCommand(direction) {
        fetch(`/action?direction=${direction}`)
          .then(response => response.text())
          .then(data => console.log(`Command sent: ${direction}`));
      }

      document.getElementById('up').addEventListener('click', () => sendControlCommand('up'));
      document.getElementById('down').addEventListener('click', () => sendControlCommand('down'));
      document.getElementById('left').addEventListener('click', () => sendControlCommand('left'));
      document.getElementById('right').addEventListener('click', () => sendControlCommand('right'));
      document.getElementById('stop').addEventListener('click', () => sendControlCommand('stop'));
    </script>
  </body>
</html>


)rawliteral";

// IMU data structure
struct IMUData {
  float ax, ay, az;
  float gx, gy, gz;
  unsigned long time;
};

// Route to fetch IMU data
static esp_err_t imu_data_handler(httpd_req_t *req) {
  IMUData data;

  // Read IMU data
  
  

  data.ax = mpu.getAccelerationX();
  data.ay = mpu.getAccelerationY();
  data.az = mpu.getAccelerationZ();
  data.gx = mpu.getRotationX();
  data.gy = mpu.getRotationY();
  data.gz = mpu.getRotationZ();
  data.time = millis();

  // Convert to JSON and send as response
  String jsonData = String("{\"ax\":") + data.ax + ",\"ay\":" + data.ay + ",\"az\":" + data.az +
                    ",\"gx\":" + data.gx + ",\"gy\":" + data.gy + ",\"gz\":" + data.gz +
                    ",\"time\":" + data.time + "}";

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, jsonData.c_str(), jsonData.length());

  return ESP_OK;
}

// Serve the main HTML page
static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, (const char*)HTML_PAGE, strlen(HTML_PAGE));
}

// Start HTTP server
void startServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_uri_t index_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = index_handler,
    .user_ctx = NULL
  };

  httpd_uri_t imu_uri = {
    .uri = "/get_imu_data",
    .method = HTTP_GET,
    .handler = imu_data_handler,
    .user_ctx = NULL
  };

  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_register_uri_handler(server, &index_uri);
    httpd_register_uri_handler(server, &imu_uri);
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize I2C communication
  Wire.begin();

  // Initialize MPU6050
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the HTTP server
  startServer();
}

void loop() {
  // The HTTP server runs in the background
}
