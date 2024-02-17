/*
-- Group 21
-- EEL 4914 - Web Server and Sensor integration
-- 11/15/2023
*/
#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"
#include "Adafruit_seesaw.h"
#include "Adafruit_AGS02MA.h"

// Sensor definitions
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// Library dependent objects;
Adafruit_seesaw ss;
Adafruit_AGS02MA ags;

// Ethernet settings
byte mac[] = {0xA8, 0x61, 0x0A, 0xAF, 0x17, 0x52};  // MAC address of Ehternet Shield
byte ip[] = {192, 168, 10, 11};                     // Static IP address assigned by the group
byte subnet[] = {255, 255, 255, 0};                 // Subnet Mask by default
EthernetServer server(80);                          // Port 80

// Set up before the loop
void setup() {

  Serial.begin(115200);                             // Set the baud rate to 115200

  // Begin a the humidity sensor
  dht.begin();

  // Find the Soil Sensor and attempt to begin; if not retry
  if (!ss.begin(0x36)) {
    Serial.println(F("Seesaw not found"));
    while (true) delay(1);
  }

  // Find the Gas Sensor and attempt to begin; if not retry
  if (!ags.begin(&Wire, 0x1A)) {
    Serial.println(F("AGS02MA not found"));
    while (true) yield();
  }

  // Initialize Ethernet setup
  Ethernet.begin(mac, ip, subnet);
  // Begin Ethernet driven Web Server
  server.begin();
}

// To poll the Embedded system
void loop() {

  // Begin a client connection
  EthernetClient client = server.available();

  /* The following determines reads information to the client until a blank line
   * is reached, it will set a boolean variable and perfrom this indefinitely.
  */
  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
          sendHTTPHeaders(client);
          sendHTMLContent(client);
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
}

// To send HTTP protocol information to the client
void sendHTTPHeaders(EthernetClient &client) {
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/html"));
  client.println(F("Connection: close"));
  client.println();
}

// Responsible for sending HTML through HTTP along with sensor information
void sendHTMLContent(EthernetClient &client) {
  client.println(F("<!DOCTYPE html>"));
  client.println(F("<html>"));
  client.println(F("<head>"));
  client.println(F("<meta http-equiv=\"refresh\" content=\"2\"/>")); // Refresh every 2 seconds
  client.println(F("<title>Open Sense (v2) Homepage</title>"));
  client.println(F("<style>body {font-family: monaco, Consolas; background-color: rgba(0, 0, 0); color: red;}</style>"));
  client.println(F("</head>"));
  client.println(F("<body>"));
  client.println(F("<h1>Open Sense (v2) Sensor Data</h1>"));

// Buffer to hold the converted float strings
  char tempStr[16]; 
  char humStr[16];
  char soilTempStr[16];

  // Convert float to string using dtostrf()
  dtostrf(dht.readTemperature(true), 5, 2, tempStr); // 5 is the width, 2 is the precision
  dtostrf(dht.readHumidity(), 5, 2, humStr);
  dtostrf(ss.getTemp(), 5, 2, soilTempStr);

  // Use the converted strings in the client.print() statements
  client.print(F("<p>Temp: "));
  client.print(tempStr);
  client.println(F(" F</p>"));

  client.print(F("<p>Humidity: "));
  client.print(humStr);
  client.println(F("%</p>"));

  client.print(F("<p>Soil Temp: "));
  client.print(soilTempStr);
  client.println(F(" C</p>"));

  client.print(F("<p>Soil Cap: "));
  client.print(ss.touchRead(0));
  client.println(F("</p>"));

  client.print(F("<p>Gas Res: "));
  client.print(ags.getGasResistance());
  client.println(F(" Ohms</p>"));

  client.print(F("<p>TVOC: "));
  client.print(ags.getTVOC());
  client.println(F(" ppb</p>"));

  client.println(F("</body></html>"));
}
