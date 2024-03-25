// Ricardo K. Colon
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// Ethernet and SD configuration
byte mac[] = {0xA8, 0x61, 0x0A, 0xAF, 0x18, 0xDF}; // Specific to my ETH SHLD 2
IPAddress ip(192, 168, 10, 11);
EthernetServer server(80);

// File names and MIME types stored in PROGMEM
const char indexFileName[] PROGMEM = "INDEX~1.HTM";
const char stylesFileName[] PROGMEM = "STYLES.CSS";
const char dataFileName[] PROGMEM = "DATA.TXT";
const char bckgFileName[] PROGMEM = "BACKGR~1.PNG";
const char graphFileName[] PROGMEM = "GRAPH.PNG";

const char textHtmlMimeType[] PROGMEM = "text/html";
const char textCssMimeType[] PROGMEM = "text/css";
const char textPlainMimeType[] PROGMEM = "text/plain";
const char imgMimeType[] PROGMEM = "image/png";

void setup() {
  Ethernet.init(10); 
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
          /* Serve files based on the request
          * Match the request with the file names on the SD card
          * Call the link what it get's referenced as and open the 
          * file as it's dipslayed on the SD card */
          if (request.indexOf("GET / ") != -1) {
            serveFile(client, indexFileName, textHtmlMimeType);
          } else if (request.indexOf("GET /styles.css ") != -1) {
            serveFile(client, stylesFileName, textCssMimeType);
          } else if (request.indexOf("GET /DATA.TXT ") != -1) {
            serveFile(client, dataFileName, textPlainMimeType);
            // For some reason the reference had to be the same as the
            // SD card file name
          } else if (request.indexOf("GET /BACKGR~1.PNG") != -1) {
            serveFile(client, bckgFileName, imgMimeType);
          } else if (request.indexOf("GET /GRAPH.PNG ") != -1) {
            serveFile(client, graphFileName, imgMimeType);
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

// Serves the file to the HTTP GET Request
void serveFile(EthernetClient &client, const char *filePathProgmem, const char *mimeTypeProgmem) {
  char filePath[30]; // Adjust size as needed
  char mimeType[30]; // Adjust size as needed
  strcpy_P(filePath, filePathProgmem); // Copy the file path from program memory to SRAM
  strcpy_P(mimeType, mimeTypeProgmem); // Copy the MIME type from program memory to SRAM

  Serial.print(F("Serving file: "));
  Serial.println(filePath);
  File file = SD.open(filePath);
  if (file) {
    Serial.println(F("File opened successfully."));
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
  client.println(F("Connection: close"));
  client.println();
  client.println(F("<!DOCTYPE html><html><body><h1>404 Not Found</h1><p>The requested URL was not found on this server.</p></body></html>"));
}
