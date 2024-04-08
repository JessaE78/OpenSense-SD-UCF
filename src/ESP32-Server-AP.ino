// Ricardo K. Colon
// Open Sense 
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "Adafruit_VEML7700.h"
#include <Adafruit_MPL3115A2.h>
#include "Adafruit_MCP9808.h"


const char* ssid = "ESP32-Access-Point";
const char* password = "password";

AsyncWebServer server(80);

String Sensors[8], SecondarySensors[8];
Adafruit_VEML7700 veml = Adafruit_VEML7700();
Adafruit_MPL3115A2 baro;
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();


#define TCAADDR 0x70

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

float readMCP9808() {
  tcaselect(2);
  tempsensor.wake();
  float f = tempsensor.readTempF();
  tempsensor.shutdown_wake(1);
  return f;
}
float readSeesaw() {
  // Simulate reading 
  return 50.0 + (random(65) / 10.0);
}
float readSGP30() {
  // Simulate reading 
  return 50.0 + (random(90) / 10.0);
}
float readADXL343() {
  // Simulate reading 
  return 50.0 + (random(15) / 10.0);
}
float readVL53L4CD() {
  // Simulate reading 
  return 50.0 + (random(80) / 10.0);
}
float readBME280() {
  // Simulate reading 
  return 50.0 + (random(40) / 10.0);
}
float readTSL2591() {
  // Simulate reading 
  return 50.0 + (random(30) / 10.0);
}
float readAGS02MA() {
  // Simulate reading 
  return 50.0 + (random(60) / 10.0);
}
float readVEML7700() {
  tcaselect(1);
  float lux = veml.readLux(VEML_LUX_AUTO);

  return lux;
}
float readMPL3115A2() {
  float temperature = baro.getTemperature();
  return temperature;
}

void setup() {
  Serial.begin(115200);

  tcaselect(1);
  veml.begin();

  // tcaselect(2);
  // // baro.begin();
  // // baro.setSeaPressure(1013.26);
  // tempsensor.begin(0x18);
  // tempsensor.setResolution(3);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // List files in SPIFFS (for verification, can be removed later)
  Serial.println("Listing files:");
  File root = SPIFFS.open("/", FILE_READ);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }

  // Configure as Access Point
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

// Handling the form submission for both primary and secondary sensors
  server.on("/configureSensors", HTTP_POST, [](AsyncWebServerRequest *request){
    // Process each expected primary and secondary sensor selection
    for (int i = 1; i <= 8; i++) {
      String primaryParamName = "sensor-" + String(i);
      String secondaryParamName = "secondary" + String(i);
      
      // Primary sensor
      if (request->hasParam(primaryParamName, true)) {
        String primaryParamValue = request->getParam(primaryParamName, true)->value();
        Sensors[i - 1] = primaryParamValue;
        Serial.println(primaryParamName + ": " + primaryParamValue);
      } else {
        Serial.println(primaryParamName + " not found.");
      }
      
      // Secondary sensor
      if (request->hasParam(secondaryParamName, true)) {
        String secondaryParamValue = request->getParam(secondaryParamName, true)->value();
        SecondarySensors[i - 1] = secondaryParamValue;
        Serial.println(secondaryParamName + ": " + secondaryParamValue);
      } else {
        Serial.println(secondaryParamName + " not found.");
      }
    }
    request->send(200, "text/plain", "Configurations Received");
  });

  // Define server routes to serve files
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/dataPage.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/dataPage.html", "text/html");
  });

  server.on("/styles_config.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/styles_config.css", "text/css");
  });

  server.on("/styles_data.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/styles_data.css", "text/css");
  });

  server.on("/data.txt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/data.txt", "text/plain");
  });

  server.on("/graph.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/graph.png", "image/png");
  });

  server.on("/background.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/background.png", "image/png");
  });

// Sensor data endpoint
  server.on("/get-sensor-data", HTTP_GET, [](AsyncWebServerRequest *request) {
    String sensorData;
    if (request->hasParam("primary")) {
      String primarySensor = request->getParam("primary")->value();
      // Example: Determine the sensor type and fetch its data
      if (primarySensor == "MCP9808") {
          float temp = readMCP9808();
          // Adjust "sensor" key to "primary" to match your JavaScript expectations
          sensorData = "{\"primary\": \"MCP9808\", \"value\": " + String(temp) + "}";
      } else if (primarySensor == "Seesaw") {
          float humidity = readSeesaw();
          // Adjust "sensor" key to "primary" to match your JavaScript expectations
          sensorData = "{\"primary\": \"Seesaw\", \"value\": " + String(humidity) + "}";
      } else if (primarySensor == "SGP30") {
          float humidity = readSGP30();
          // Adjust "sensor" key to "primary" to match your JavaScript expectations
          sensorData = "{\"primary\": \"SGP30\", \"value\": " + String(humidity) + "}";
      } else if (primarySensor == "ADXL343") {
          float humidity = readADXL343();
          // Adjust "sensor" key to "primary" to match your JavaScript expectations
          sensorData = "{\"primary\": \"ADXL343\", \"value\": " + String(humidity) + "}";
      } else if (primarySensor == "VL53L4CD") {
          float humidity = readVL53L4CD();
          // Adjust "sensor" key to "primary" to match your JavaScript expectations
          sensorData = "{\"primary\": \"VL53L4CD\", \"value\": " + String(humidity) + "}";
      } else if (primarySensor == "BME280") {
          float humidity = readBME280();
          // Adjust "sensor" key to "primary" to match your JavaScript expectations
          sensorData = "{\"primary\": \"BME280\", \"value\": " + String(humidity) + "}";
      } else if (primarySensor == "TSL2591") {
          float humidity = readTSL2591();
          // Adjust "sensor" key to "primary" to match your JavaScript expectations
          sensorData = "{\"primary\": \"TSL2591\", \"value\": " + String(humidity) + "}";
      } else if (primarySensor == "AGS02MA") {
          float humidity = readAGS02MA();
          // Adjust "sensor" key to "primary" to match your JavaScript expectations
          sensorData = "{\"primary\": \"AGS02MA\", \"value\": " + String(humidity) + "}";
      } else if (primarySensor == "VEML7700") {
          float LUX = readVEML7700();
          // Adjust "sensor" key to "primary" to match your JavaScript expectations
          sensorData = "{\"primary\": \"VEML7700\", \"value\": " + String(LUX) + "}";
      } else if (primarySensor == "MPL3115A2") {
          float temperature = readMPL3115A2();
          // Adjust "sensor" key to "primary" to match your JavaScript expectations
          sensorData = "{\"primary\": \"MPL3115A2\", \"value\": " + String(temperature) + "}";
      }
      else {
          sensorData = "{\"error\": \"Unknown sensor\"}";
      }
    } else {
      sensorData = "{\"error\": \"No sensor specified\"}";
    }
    request->send(200, "application/json", sensorData);
  });

  // Start server
  server.begin();
}

void loop() {
  // Using noise from an unconnected analog pin for randomness.
  randomSeed(analogRead(32));
}
