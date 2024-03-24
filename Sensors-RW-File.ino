#include <SPI.h>
#include <SD.h>
#include "Adafruit_VEML7700.h"
#include "Adafruit_seesaw.h"
#include "Adafruit_AGS02MA.h"
#include <Adafruit_MPL3115A2.h>

Adafruit_MPL3115A2 baro;

File myFile;
Adafruit_VEML7700 veml = Adafruit_VEML7700();
int counter = 0; // Initialize a counter for the data gathering iterations

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print(F("Initializing SD card..."));

  if (!SD.begin(4)) {
    Serial.println(F("initialization failed!"));
    while (1);
  }
  Serial.println(F("initialization done."));

  if (!veml.begin()) {
    Serial.println(F("VEML not found"));
  }
  if (!baro.begin()) {
    Serial.println(F("Could not find MPL3115A2!"));
  }
  // use to set sea level pressure for current location
  // this is needed for accurate altitude measurement
  // STD SLP = 1013.26 hPa
  //baro.setSeaPressure(1013.26);

  Serial.println(F("Please wait...Gathering data (~30s)"));
}

void loop() {
  // Prevents any operations if 10 or more data collections have been done
  if (counter >= 10) {
    myFile.close(); // Ensure the file is closed after writing
    Serial.println(F("Data collection complete. No further actions."));
    while(1);
  }
  // This block runs for counters 0 through 9
  myFile = SD.open("data3.txt", FILE_WRITE);
  if (myFile) {
    float lux = veml.readLux(VEML_LUX_AUTO);
    //float pressure = baro.getPressure();
    unsigned int time = millis();
    myFile.print(time); myFile.print(F(",")); myFile.print(lux);
    myFile.print(F(","));//myFile.print(pressure);
    myFile.println();
    myFile.close(); // Close the file after writing to ensure data is saved
    Serial.println(F("Data logged!")); // Confirmation message for debugging
    counter++; // Increment after a successful data log
    delay(2000); // Wait for 2 seconds before the next loop iteration
  } else {
    Serial.println(F("error opening data3.txt"));
    while(1);
  }
}
