// Ricardo K. Colon
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// Specific to my ETH SHLD 2
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

// Please ensure the files are named appropriately. See SD CARD FILE VIEWER
void loop() {
  EthernetClient client = server.available();
  if (client) {
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n' && request.endsWith("\r\n\r\n")) {
          // Match the request with the file names on the SD card
          // Call the link what it get's referenced as and open the 
          // file as it's dipslayed on the SD card
          if (request.indexOf("GET / ") != -1) {
            serveFile(client, "INDEX~1.HTM", "text/html");
          } else if (request.indexOf("GET /styles.css ") != -1) { 
            serveFile(client, "STYLES.CSS", "text/css");
          } else if (request.indexOf("GET /DATA.TXT ") != -1) {
            serveFile(client, "DATA.TXT", "text/plain");
            // For some reason the reference had to be the same as the
            // SD card file name
          } else if (request.indexOf("GET /BACKGR~1.PNG ") != -1) {
            serveFile(client, "BACKGR~1.PNG", "image/png");
          } else {
            send404(client);
          }
          break;
        }
      }
    }
    delay(1);
    client.stop();
  }
}

// Servers the file to the HTTP GET Request
void serveFile(EthernetClient &client, const char *filePath, const char *mimeType) {
  Serial.print("Serving file: ");
  Serial.println(filePath);
  File file = SD.open(filePath);
  if (file) {
    Serial.println("File opened successfully.");
    client.println(F("HTTP/1.1 200 OK"));
    client.print(F("Content-Type: "));
    client.println(mimeType);
    client.println(F("Connection: close")); // Ensure the connection is closed after completion
    client.println();
    while (file.available()) {
      client.write(file.read());
    }
    file.close();
  } else {
    send404(client);
  }
}

// If no GET request link is found, populate a 404
void send404(EthernetClient &client) {
  client.println(F("HTTP/1.1 404 Not Found"));
  client.println(F("Content-Type: text/html"));
  client.println(F("Connection: close")); // Ensure the connection is closed after completion
  client.println();
  client.println(F("<!DOCTYPE html><html><body><h1>404 Not Found</h1><p>The requested URL was not found on this server.</p></body></html>"));
}
