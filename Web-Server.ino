#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

byte mac[] = {0xA8, 0x61, 0x0A, 0xAF, 0x18, 0xDF};
IPAddress ip(192, 168, 10, 11); 
EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());

  if (!SD.begin(4)) {
    Serial.println("Initialization of SD card failed!");
    return;
  }
  Serial.println("SD card initialized.");
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n' && request.endsWith("\r\n\r\n")) {
          // Check for specific file requests
          if (request.indexOf("GET /graph.html") != -1) {
            serveFile(client, "GRAPH~1.HTM", "text/html");
          } else if (request.indexOf("GET /CHARTU~1.JS") != -1) {
            serveFile(client, "CHARTU~1.JS", "application/javascript");
          } else {
            sendHTMLContent(client);
          }
          break;
        }
      }
    }
    delay(1);
    client.stop();
  }
}

void serveFile(EthernetClient &client, const char *filePath, const char *mimeType) {
  File file = SD.open(filePath);
  if (file) {
    client.println(F("HTTP/1.1 200 OK"));
    client.print(F("Content-Type: "));
    client.println(mimeType);
    client.println();
    while (file.available()) {
      client.write(file.read());
    }
    file.close();
  } else {
    client.println(F("HTTP/1.1 404 Not Found"));
    client.println(F("Content-Type: text/html"));
    client.println();
    client.println(F("<!DOCTYPE html><html><body>404: File Not Found</body></html>"));
  }
}

void sendHTTPHeaders(EthernetClient &client) {
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/html"));
  client.println(F("Connection: close"));
  client.println();
}

void sendHTMLContent(EthernetClient &client) {
  sendHTTPHeaders(client);
  client.println(F("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Sensor Data</title></head><body><h1>Welcome to the Sensor Data Page</h1><p>Click below to view the graph.</p><a href=\"/graph.html\">View Graph</a></body></html>"));
}
