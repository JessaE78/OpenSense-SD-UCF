#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "ESP32-Access-Point";
const char* password = "password";

AsyncWebServer server(80);

String Sensors[8];

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

  // Handling the form submission
  server.on("/configureSensors", HTTP_POST, [](AsyncWebServerRequest *request){
    // Process each expected sensor selection
    for (int i = 1; i <= 8; i++) { // Assuming up to 8 sensors
      String paramName = "sensor-" + String(i);
      if (request->hasParam(paramName, true)) {
        String paramValue = request->getParam(paramName, true)->value();
        Serial.println(paramName + ": " + paramValue);
        Sensors[i - 1 ] = paramValue;
      } else {
        Serial.println(paramName + " not found.");
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

  // Start server
  server.begin();
}

void loop() {
  // Nothing needed here
}
