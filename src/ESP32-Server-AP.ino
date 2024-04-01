#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>  // Use SPIFFS instead of LittleFS

const char* ssid = "ESP32-Access-Point";
const char* password = "password"; // Choose a secure password

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Mounting SPIFFS failed.");
    delay(1000); // Ensure message is sent over Serial before restarting
    ESP.restart(); // Restart to re-initialize SPIFFS
  }

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
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Listen for incoming clients
  if (!client) {
    return;
  }

  Serial.println("New Client.");
  while (!client.available()) {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  client.flush();

  if (request.indexOf("GET / ") >= 0) {
    sendFile(client, "/index.html", "text/html");
  } else if (request.indexOf("GET /dataPage.html") >= 0) {
    sendFile(client, "/dataPage.html", "text/html");
  } else if (request.indexOf("GET /styles_config.css") >= 0) {
    sendFile(client, "/styles_config.css", "text/css");
  } else if (request.indexOf("GET /styles_data.css") >= 0) {
    sendFile(client, "/styles_data.css", "text/css");
  } else if (request.indexOf("GET /data.txt") >= 0) {
    sendFile(client, "/data.txt", "text/plain"); // Corrected the MIME type for .txt
  } else if (request.indexOf("GET /graph.png") >= 0) {
    sendFile(client, "/graph.png", "image/png");}
//  } else if (request.indexOf("GET /background.png") >= 0) {
//    sendFile(client, "/background.png", "image/png");
//  }

  client.stop();
  Serial.println("Client Disconnected.");
}

void sendFile(WiFiClient &client, const String &path, const String &contentType) {
  File file = SPIFFS.open(path, FILE_READ);
  if (!file) {
    Serial.print("File not found: ");
    Serial.println(path);
    // It's important to send a HTTP 404 error if the file is not found
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/plain");
    client.println();
    client.println("404: File Not Found");
    return;
  }

  client.printf("HTTP/1.1 200 OK\nContent-Type: %s\n\n", contentType.c_str());
  while (file.available()) {
    client.write(file.read());
  }
  file.close();
}
