// Ricardo K. Colon
// Open Sense 
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "Adafruit_VEML7700.h"
#include <Adafruit_MPL3115A2.h>
#include "Adafruit_MCP9808.h"
#include "Adafruit_SGP30.h"

const char* ssid = "ESP32-Access-Point";
const char* password = "password";

AsyncWebServer server(80);

String Sensors[8], SecondarySensors[8];
Adafruit_VEML7700 veml = Adafruit_VEML7700();
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
Adafruit_SGP30 sgp;
Adafruit_MPL3115A2 baro;


#define TCAADDR 0x70

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void initVEML7700(uint8_t channel) {
  tcaselect(channel);
  veml.begin();
}
void initMCP9808(uint8_t channel) {
  tcaselect(channel);
  tempsensor.begin(0x18);
  tempsensor.setResolution(3);
  tempsensor.wake();
}
void initSGP30(uint8_t channel) {
  tcaselect(channel);
  sgp.begin();
  sgp.setIAQBaseline(0x8E68, 0x8F41); 
}

void initAllSensors() {
  for (int i = 0; i < 8; i++) { // Assuming 8 possible channels/sensors
    if (Sensors[i] == "VEML7700") {
      initVEML7700(i);
    } else if (Sensors[i] == "MCP9808") {
      initMCP9808(i);
    } else if (Sensors[i] == "SGP30") {
      initSGP30(i);
    } 
    // Add other sensors here
  }
}

float readMCP9808(uint8_t channel) {
  tcaselect(channel);
  // tempsensor.wake();
  float f = tempsensor.readTempF();
  // tempsensor.shutdown_wake(1);
  return f;
}
float readSeesaw(uint8_t channel) {
  return 50.0 + (random(65) / 10.0);
}
float readSGP30(uint8_t channel) {
  tcaselect(channel);
  sgp.IAQmeasure();
  float TVOC = sgp.TVOC;
  return TVOC;
}
float readADXL343(uint8_t channel) {
  return 50.0 + (random(15) / 10.0);
}
float readVL53L4CD(uint8_t channel) {
  return 50.0 + (random(80) / 10.0);
}
float readBME280(uint8_t channel) {
  return 50.0 + (random(40) / 10.0);
}
float readTSL2591(uint8_t channel) {
  return 50.0 + (random(30) / 10.0);
}
float readAGS02MA(uint8_t channel) {
  return 50.0 + (random(60) / 10.0);
}
float readVEML7700(uint8_t channel) {
  tcaselect(channel);
  float lux = veml.readLux(VEML_LUX_AUTO);
  return lux;
}
float readMPL3115A2(uint8_t channel) {
  tcaselect(channel);
    float temperature = baro.getTemperature();
  return temperature;
}

void setup() {
  Serial.begin(115200);

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
    for (int i = 0; i < 8; i++) {
      String primaryParamName = "sensor-" + String(i);
      String secondaryParamName = "secondary" + String(i);
      
      // Primary sensor
      if (request->hasParam(primaryParamName, true)) {
        String primaryParamValue = request->getParam(primaryParamName, true)->value();
        Sensors[i] = primaryParamValue;
        Serial.println(primaryParamName + ": " + primaryParamValue);
      } else {
        Serial.println(primaryParamName + " not found.");
      }
      
      // Secondary sensor
      if (request->hasParam(secondaryParamName, true)) {
        String secondaryParamValue = request->getParam(secondaryParamName, true)->value();
        SecondarySensors[i] = secondaryParamValue;
        Serial.println(secondaryParamName + ": " + secondaryParamValue);
      } else {
        Serial.println(secondaryParamName + " not found.");
      }
    }
    initAllSensors();
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
      int channel = request->getParam("channel")->value().toInt(); 
      delay(100); // Prevents preemptive reading; need this to prevent crashes
      
      // Find sesnsor and read the value; return a JSON string 
      if (primarySensor == "MCP9808") {
          float temp = readMCP9808(channel);
          sensorData = "{\"primary\": \"MCP9808\", \"value\": " + String(temp) + ", \"unit\": \"°F\"}";
      } else if (primarySensor == "Seesaw") {
          float humidity = readSeesaw(channel);
          sensorData = "{\"primary\": \"Seesaw\", \"value\": " + String(humidity) + ", \"unit\": \"°F\"}";
      } else if (primarySensor == "SGP30") {
          float TVOC = readSGP30(channel);
          sensorData = "{\"primary\": \"SGP30\", \"value\": " + String(TVOC) + ", \"unit\": \"TVOC\"}";
      } else if (primarySensor == "ADXL343") {
          float humidity = readADXL343(channel);
          sensorData = "{\"primary\": \"ADXL343\", \"value\": " + String(humidity) + ", \"unit\": \"xyz\"}";
      } else if (primarySensor == "VL53L4CD") {
          float humidity = readVL53L4CD(channel);
          sensorData = "{\"primary\": \"VL53L4CD\", \"value\": " + String(humidity) + ", \"unit\": \"mm\"}";
      } else if (primarySensor == "BME280") {
          float humidity = readBME280(channel);
          sensorData = "{\"primary\": \"BME280\", \"value\": " + String(humidity) + ", \"unit\": \"°F\"}";
      } else if (primarySensor == "TSL2591") {
          float humidity = readTSL2591(channel);
          sensorData = "{\"primary\": \"TSL2591\", \"value\": " + String(humidity) + ", \"unit\": \"lux\"}";
      } else if (primarySensor == "AGS02MA") {
          float humidity = readAGS02MA(channel);
          sensorData = "{\"primary\": \"AGS02MA\", \"value\": " + String(humidity) + ", \"unit\": \"TVOC\"}";
      } else if (primarySensor == "VEML7700") {
          float LUX = readVEML7700(channel);
          sensorData = "{\"primary\": \"VEML7700\", \"value\": " + String(LUX) + ", \"unit\": \"lux\"}";
      } else if (primarySensor == "MPL3115A2") {
          float temperature = readMPL3115A2(channel);
          sensorData = "{\"primary\": \"MPL3115A2\", \"value\": " + String(temperature) + ", \"unit\": \"mmHg\"}";
      }
      else {
          sensorData = "{\"error\": \"Unknown sensor\"}";
      }
    } else {
      sensorData = "{\"error\": \"No sensor specified\"}";
    }
    request->send(200, "application/json", sensorData);
  });

  // Start the Async server
  server.begin();
}

void loop() {
  // Using the noise from an unconnected analog pin for randomness.
  randomSeed(analogRead(32));
}
