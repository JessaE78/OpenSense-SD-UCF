// Ricardo K. Colon
// Open Sense v2
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "Adafruit_VEML7700.h"
#include <Adafruit_BME280.h>
#include <Adafruit_MPL3115A2.h>
#include "Adafruit_MCP9808.h"
#include "Adafruit_SGP30.h"
#include "Adafruit_seesaw.h"
#include "AGS02MA.h"
#define SEALEVELPRESSURE_HPA (1013.25)
#define TCAADDR 0x70

// WiFi Access Point Information
const char *ssid = "ESP32-Access-Point";
const char *password = "password";

// Intialize asyncronous server
AsyncWebServer server(80);

// String declarations
String Sensors[8], SecondarySensors[8];

// Object declarations
Adafruit_VEML7700 veml = Adafruit_VEML7700();
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
Adafruit_SGP30 sgp;
Adafruit_MPL3115A2 baro;
Adafruit_seesaw ss;
AGS02MA AGS(26);
Adafruit_BME280 bme;

// Mux channel selector
void tcaselect(uint8_t i)
{
  if (i > 7)
    return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// Initialization functions
String initVEML7700(uint8_t channel)
{
  tcaselect(channel);
  if (!veml.begin())
  {
    return "Failed to initialize VEML7700 on channel " + String(channel);
  }
  return ""; // Empty string indicates success
}
String initMCP9808(uint8_t channel)
{
  tcaselect(channel);
  if (!tempsensor.begin(0x18))
  {
    return "Failed to initialize MCP9808 on channel " + String(channel);
  }
  tempsensor.setResolution(3); // Set resolution as needed
  tempsensor.wake();           // Wake up the sensor
  return "";
}

String initSGP30(uint8_t channel)
{
  tcaselect(channel);
  if (!sgp.begin())
  {
    return "Failed to initialize SGP30 on channel " + String(channel);
  }
  sgp.setIAQBaseline(0x8E68, 0x8F41); // Set baseline as needed
  return "";
}

String initSeesaw(uint8_t channel)
{
  tcaselect(channel);
  if (!ss.begin(0x36))
  { // Assume address 0x36 for seesaw device
    return "Failed to initialize Seesaw on channel " + String(channel);
  }
  return "";
}

String initAGS02MA(uint8_t channel)
{
  tcaselect(channel);
  if (!AGS.begin())
  {
    return "Failed to initialize AGS02MA on channel " + String(channel);
  }
  AGS.setPPBMode(); // Assuming the setPPBMode does not need a return value check
  return "";
}

String initBME280(uint8_t channel)
{
  tcaselect(channel);
  if (!bme.begin())
  {
    return "Failed to initialize BME280 on channel " + String(channel);
  }
  return "";
}

// Provide the readings of the specified sensor/sensor mode
String readSeesaw(uint8_t channel, String sensorMode)
{
  tcaselect(channel);
  float data = -1;
  String valueJSON;
  if (sensorMode == "Capacitive")
  {
    uint16_t capread = ss.touchRead(0);
    valueJSON = "{\"primary\": \"Seesaw\", \"value\": " + String(capread) + ", \"unit\": \"Cap.\"}";
  }
  else
  {
    data = ss.getTemp();
    valueJSON = "{\"primary\": \"Seesaw\", \"value\": " + String(data) + ", \"unit\": \"°F\"}";
  }
  return valueJSON;
}
String readSGP30(uint8_t channel, String sensorMode)
{
  tcaselect(channel);
  sgp.IAQmeasure();
  float data = -1;
  String valueJSON;
  if (sensorMode == "eCO2")
  {
    data = sgp.eCO2;
    valueJSON = "{\"primary\": \"SGP30\", \"value\": " + String(data) + ", \"unit\": \"ppm\"}";
  }
  else
  {
    data = sgp.TVOC;
    valueJSON = "{\"primary\": \"SGP30\", \"value\": " + String(data) + ", \"unit\": \"ppb\"}";
  }
  return valueJSON;
}
String readBME280(uint8_t channel, String sensorMode)
{
  tcaselect(channel);
  float data = -1;
  String valueJSON;
  if (sensorMode == "Pressure")
  {
    data = bme.readPressure() / 100.0F;
    valueJSON = "{\"primary\": \"BME280\", \"value\": " + String(data) + ", \"unit\": \"hPa\"}";
  }
  else if (sensorMode == "Altitude")
  {
    data = bme.readAltitude(SEALEVELPRESSURE_HPA);
    valueJSON = "{\"primary\": \"BME280\", \"value\": " + String(data) + ", \"unit\": \"m\"}";
  }
  else if (sensorMode == "Humidity")
  {
    data = bme.readHumidity();
    valueJSON = "{\"primary\": \"BME280\", \"value\": " + String(data) + ", \"unit\": \"%\"}";
  }
  else
  {
    data = bme.readTemperature();
    valueJSON = "{\"primary\": \"BME280\", \"value\": " + String(data) + ", \"unit\": \"°C\"}";
  }
  return valueJSON;
}
float readMCP9808(uint8_t channel)
{
  tcaselect(channel);
  // tempsensor.wake();
  float temp = tempsensor.readTempF();
  // tempsensor.shutdown_wake(1);
  return temp;
}
float readMPL3115A2(uint8_t channel)
{
  tcaselect(channel);
  float temperature = baro.getTemperature();
  return temperature;
}
float readVEML7700(uint8_t channel)
{
  tcaselect(channel);
  float lux = veml.readLux(VEML_LUX_AUTO);
  return lux;
}
float readAGS02MA(uint8_t channel)
{
  uint32_t value = AGS.readPPB();
  return value;
}
float readTSL2591(uint8_t channel)
{
  return 50.0 + (random(30) / 10.0);
}
float readADXL343(uint8_t channel)
{
  return 50.0 + (random(15) / 10.0);
}
float readVL53L4CD(uint8_t channel)
{
  return 50.0 + (random(80) / 10.0);
}

// Setup routes and filesystem
void setup()
{
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // List files in SPIFFS (for verification, can be removed later)
  Serial.println("Listing files:");
  File root = SPIFFS.open("/", FILE_READ);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
    }
    else
    {
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
  server.on("/configureSensors", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    String errorMessage;
    for (int i = 0; i < 8; i++) {
      String primaryParamName = "sensor-" + String(i);
      String result;

      // Check if the primary sensor parameter is provided
      if (request->hasParam(primaryParamName, true)) {
        String primaryParamValue = request->getParam(primaryParamName, true)->value();
        Sensors[i] = primaryParamValue;

        // Attempt to initialize the sensor and collect any error messages
        if (Sensors[i] == "VEML7700") {
          result = initVEML7700(i);
        }
        // Extend initialization to other sensors based on your setup
        else if (Sensors[i] == "MCP9808") {
          result = initMCP9808(i);
        }
        else if (Sensors[i] == "SGP30") {
          result = initSGP30(i);
        }
        else if (Sensors[i] == "Seesaw") {
          result = initSeesaw(i);
        }
        else if (Sensors[i] == "AGS02MA") {
          result = initAGS02MA(i);
        }
        else if (Sensors[i] == "BME280") {
          result = initBME280(i);
        }
        // Add more sensors as necessary...

        if (!result.isEmpty()) {
          errorMessage += result + "\n"; // Accumulate error messages
        }
      } else {
        errorMessage += primaryParamName + " parameter not found.\n";
      }
    }

    if (!errorMessage.isEmpty()) {
      request->send(500, "text/plain", errorMessage); // Send error message if there are any
    } else {
      request->send(200, "text/plain", "All sensors initialized successfully");
    } });

  // Define server routes to serve files
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  server.on("/dataPage.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/dataPage.html", "text/html"); });

  server.on("/styles_config.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/styles_config.css", "text/css"); });

  server.on("/styles_data.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/styles_data.css", "text/css"); });

  server.on("/data.txt", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/data.txt", "text/plain"); });

  server.on("/graph.png", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/graph.png", "image/png"); });

  server.on("/background.png", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/background.png", "image/png"); });

  // Route to load chart.umd.js
  server.on("/chart.umd.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/chart.umd.js", "application/javascript"); });

  // Sensor data endpoint
  server.on("/get-sensor-data", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String sensorData;
    if (request->hasParam("primary")) {
      String primarySensor = request->getParam("primary")->value();
      String secondarySensor = request->getParam("secondary")->value();
      int channel = request->getParam("channel")->value().toInt(); 
      delay(150); // Prevents preemptive reading; need this to prevent crashes
      
      // Find sesnsor and read the value; return a JSON string 
      if (primarySensor == "MCP9808") {
          float temp = readMCP9808(channel);
          sensorData = "{\"primary\": \"MCP9808\", \"value\": " + String(temp) + ", \"unit\": \"°F\"}";
      } else if (primarySensor == "Seesaw") {
          sensorData = readSeesaw(channel, secondarySensor);
      } else if (primarySensor == "SGP30") { 
          sensorData = readSGP30(channel, secondarySensor);
      } else if (primarySensor == "ADXL343") {
          float humidity = readADXL343(channel);
          sensorData = "{\"primary\": \"ADXL343\", \"value\": " + String(humidity) + ", \"unit\": \"xyz\"}";
      } else if (primarySensor == "VL53L4CD") {
          float humidity = readVL53L4CD(channel);
          sensorData = "{\"primary\": \"VL53L4CD\", \"value\": " + String(humidity) + ", \"unit\": \"mm\"}";
      } else if (primarySensor == "BME280") {
        sensorData = readBME280(channel, secondarySensor);
      } else if (primarySensor == "TSL2591") {
          float humidity = readTSL2591(channel);
          sensorData = "{\"primary\": \"TSL2591\", \"value\": " + String(humidity) + ", \"unit\": \"lux\"}";
      } else if (primarySensor == "AGS02MA") {
          float tvoc = readAGS02MA(channel);
          sensorData = "{\"primary\": \"AGS02MA\", \"value\": " + String(tvoc) + ", \"unit\": \"ppb\"}";
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
    request->send(200, "application/json", sensorData); });

  // Start the Async server
  server.begin();
}

void loop()
{
  // Using the noise from an unconnected analog pin for randomness.
  randomSeed(analogRead(32));
}
