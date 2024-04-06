// Ricardo K. Colon
// Open Sense 
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "ESP32-Access-Point";
const char* password = "password";

AsyncWebServer server(80);

String Sensors[8], SecondarySensors[8];

// Function to simulate sensor data based on sensor type
String simulateSensorData(const String& sensorType) {
    if (sensorType == "MCP9808") {
        // Simulate a temperature reading for MCP9808
        return "{\"sensor\":\"MCP9808\", \"temperature\":" + String(random(20, 30)) + "}";
    } else if (sensorType == "SGP30") {
        // Simulate air quality readings for SGP30
        return "{\"sensor\":\"SGP30\", \"TVOC\":" + String(random(100, 500)) + ", \"eCO2\":" + String(random(400, 1000)) + "}";
    }
    // I need to add more sensors
    else {
        // Default case for unspecified or unknown sensor
        return "{\"sensor\":\"Unknown\", \"message\":\"No data available\"}";
    }
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

  server.on("/get-sensor-data", HTTP_GET, [](AsyncWebServerRequest *request){
      String sensorType = request->hasParam("primary") ? request->getParam("primary")->value() : "None";
      String sensorData = simulateSensorData(sensorType);
      request->send(200, "application/json", sensorData);
  });

  // Start server
  server.begin();
}

void loop() {
  // Using noise from an unconnected analog pin for randomness.
  randomSeed(analogRead(32));
}
