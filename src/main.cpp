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
      body { font-family: Arial; text-align: center; margin: 0 auto; padding-top: 30px; }
      #chart-accel, #chart-gyro { 
        width: 45%; 
        height: 300px; 
        margin: 20px auto;
      }
      .chart-container {
        display: flex;
        justify-content: space-between;
        padding: 20px;
      }
      .button-container {
        margin-top: 20px;
      }
      .button {
        background-color: #2f4468;
        border: none;
        color: white;
        padding: 15px 30px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 18px;
        margin: 10px;
        cursor: pointer;
      }
    </style>
  </head>
  <body>
    <h1>ESP32 IMU Data and Car Control</h1>
    
    <!-- IMU Data Plots -->
    <div class="chart-container">
      <div id="chart-accel"></div>
      <div id="chart-gyro"></div>
    </div>

    <!-- Control Buttons -->
    <div class="button-container">
      <button class="button" onclick="sendControlCommand('forward')">Forward</button>
      <button class="button" onclick="sendControlCommand('backward')">Backward</button>
      <button class="button" onclick="sendControlCommand('left')">Left</button>
      <button class="button" onclick="sendControlCommand('right')">Right</button>
      <button class="button" onclick="sendControlCommand('stop')">Stop</button>
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
        yaxis: { title: 'Acceleration (m/s²)' },
        showlegend: true
      };

      var layoutGyro = {
        title: 'Gyroscope Data',
        xaxis: { title: 'Time' },
        yaxis: { title: 'Gyroscope (°/s)' },
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

        if (imuDataAccel.time.length > 50) {
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

      // Fetch IMU data every 10ms for a faster stream
      setInterval(() => {
        fetch('/get_imu_data')
          .then(response => response.json())
          .then(data => updateGraph(data));
      }, 10);  // 10ms for faster updates

      // Send command to ESP32 to control the motors
      function sendControlCommand(command) {
        fetch(`/action?go=${command}`)
          .then(response => response.text())
          .then(data => console.log(`Command sent: ${command}`))
          .catch(error => console.error('Error sending command:', error));
      }
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
