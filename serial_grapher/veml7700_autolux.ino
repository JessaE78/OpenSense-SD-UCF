/* VEML7700 Auto Lux Example
 *
 * This example sketch demonstrates reading lux using the automatic
 * method which adjusts gain and integration time as needed to obtain
 * a good reading. A non-linear correction is also applied if needed.
 *
 * See Vishy App Note "Designing the VEML7700 Into an Application"
 * Vishay Document Number: 84323, Fig. 24 Flow Chart
 */

#include "Adafruit_VEML7700.h"

Adafruit_VEML7700 veml = Adafruit_VEML7700();
unsigned long currTime = 0;
double timeInSeconds = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  // Serial.println("Adafruit VEML7700 Auto Lux Test");

  if (!veml.begin()) {
    // Serial.println("Sensor not found");
    while (1);
  }
  // Serial.println("Sensor found");
}

void loop() {
  // to read lux using automatic method, specify VEML_LUX_AUTO
  float lux = veml.readLux(VEML_LUX_AUTO);

  currTime = millis();
  timeInSeconds = currTime / 1000.0;

  Serial.print(timeInSeconds);
  Serial.print(" - "); Serial.println(lux); 
  // Serial.println("s");
  //Serial.println(lux);


  delay(500);
}