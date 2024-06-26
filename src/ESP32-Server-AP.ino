// Ricardo K. Colon
// Open Sense v2
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "SensorLibrary.hh"
#define SEALEVELPRESSURE_HPA (1013.25)
#define TCAADDR 0x70

// WiFi Access Point Information
const char *ssid = "Open Sense";
const char *password = "password123";

// Intialize asyncronous server
AsyncWebServer server(80);

// String declarations
String Sensors[8], SecondarySensors[8];

// Custom Sensor Framework
SensorLibrary sensorLib;

// LED states
void APstarted(WiFiEvent_t event){
    digitalWrite(12, LOW);
    Serial.print("Acess point is now Ready");
}
void APstopped(WiFiEvent_t event){
    digitalWrite(12, HIGH);
    Serial.print("Acess point is unavailable");
}
void UserConnection(WiFiEvent_t event){
    digitalWrite(13, LOW);
    Serial.print("User has connected to access point");
}
void UserDisconnection(WiFiEvent_t event){
    digitalWrite(13, HIGH);
    Serial.print("User has disconnected from access point");
}

// Mux channel selector
void tcaselect(uint8_t i)
{
  if (i > 7)
    return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// Setup routes and filesystem
void setup()
{
  Serial.begin(115200);

  Wire.begin();
  
  // Change LED states
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  
  WiFi.onEvent(APstarted, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_START);
  WiFi.onEvent(APstopped, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STOP);
  WiFi.onEvent(UserConnection, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
  WiFi.onEvent(UserDisconnection, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
  
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

      tcaselect(i);
      delay(150);
      String primaryParamName = "sensor-" + String(i);
      String secondaryParamName = "secondary" + String(i);
      String result;

      // Check if the primary sensor parameter is provided
      if (request->hasParam(primaryParamName, true)) {
        String primaryParamValue = request->getParam(primaryParamName, true)->value();
        Sensors[i] = primaryParamValue;
        Serial.println(primaryParamName + ": " + primaryParamValue);

        if (request->hasParam(secondaryParamName, true)) {
          String secondaryParamValue = request->getParam(secondaryParamName, true)->value();
          SecondarySensors[i] = secondaryParamValue;
          Serial.println(secondaryParamName + ": " + secondaryParamValue);
      }
      
      if (Sensors[i] == "VEML7700") {
        result = sensorLib.Start_VEML7700_Sensor();
      } else if (Sensors[i] == "MCP9808") {
        result = sensorLib.Start_MCP9808_Sensor();
      } else if (Sensors[i] == "SGP30") {
        result = sensorLib.Start_SGP30_Sensor();
      } else if (Sensors[i] == "Seesaw") {
        result = sensorLib.Start_Seesaw_Sensor();
      } else if (Sensors[i] == "AGS02MA") {
        result = sensorLib.Start_AGS02MA_Sensor();
      } else if (Sensors[i] == "BME280") {
        result = sensorLib.Start_BME280_Sensor();
      }  else if (Sensors[i] == "VL53L4CD") {
        result = sensorLib.Start_VL53L4CD_Sensor();
      } else if (Sensors[i] == "ADXL343") {
        result = sensorLib.Start_ADXL343_Sensor();
      } else if (Sensors[i] == "TSL2591") {
        result = sensorLib.Start_TSL2591_Sensor();
      } else if (Sensors[i] == "MPL3115A2") {
        result = sensorLib.Start_MPL3115A2_Sensor();
      } 
      // Add other sensors here
      if (!result.isEmpty()) {
        errorMessage += result + (i+1)+ "\n"; // Accumulate error messages
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
      tcaselect(channel);
      // Find sesnsor and read the value; return a JSON string 
      if (primarySensor == "MCP9808") {
          float temp = sensorLib.Poll_MCP9808_Sensor();
          sensorData = "{\"primary\": \"MCP9808\", \"value\": " + String(temp) + ", \"unit\": \"°F\"}";
      } else if (primarySensor == "Seesaw") {
          sensorData = sensorLib.Poll_Seesaw_Sensor(secondarySensor);
      } else if (primarySensor == "SGP30") { 
          sensorData = sensorLib.Poll_SGP30_Sensor(secondarySensor);
      } else if (primarySensor == "ADXL343") {
           float* temp = sensorLib.Poll_ADXL343_Sensor();
          String str = String(temp[0]) + String(temp[1]) + String(temp[2]);
          sensorData = "{\"primary\": \"ADXL343\", \"value\": " + str + ", \"unit\": \"xyz\"}";
      } else if (primarySensor == "VL53L4CD") {
          float TOF = sensorLib.Poll_VL53L4CD_Sensor();
          sensorData = "{\"primary\": \"VL53L4CD\", \"value\": " + String(TOF) + ", \"unit\": \"mm\"}";
      } else if (primarySensor == "BME280") {
        sensorData = sensorLib.Poll_BME280_Sensor(secondarySensor);
      } else if (primarySensor == "TSL2591") {
          sensorData = sensorLib.Poll_TSL2591_Sensor(secondarySensor);
      } else if (primarySensor == "AGS02MA") {
          float tvoc = sensorLib.Poll_AGS02MA_Sensor();
          sensorData = "{\"primary\": \"AGS02MA\", \"value\": " + String(tvoc) + ", \"unit\": \"ppb\"}";
      } else if (primarySensor == "VEML7700") {
          float LUX = sensorLib.Poll_VEML7700_Sensor();
          sensorData = "{\"primary\": \"VEML7700\", \"value\": " + String(LUX) + ", \"unit\": \"lux\"}";
      } else if (primarySensor == "MPL3115A2") {
          sensorData = sensorLib.Poll_MPL3115A2_Sensor(secondarySensor);
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

}
