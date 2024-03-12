/*
-- Group 21
-- EEL 4914 - Sensor Tests
-- 03/12/2024
*/

#include <SPI.h>
#include "Adafruit_AGS02MA.h"   // TVOC Sensor - AGS02MA
#include <Adafruit_MPL3115A2.h> // Temp Sensor - MPL3115A2
#include "Adafruit_VEML7700.h"  // Lux  Sensor - VEML7700
#include "Adafruit_seesaw.h"    // Soil Sensor - Stemma Seesaw

// For AGS02MA
Adafruit_AGS02MA ags;
int TVOC_seen = 0;

// For temp sensor
Adafruit_MPL3115A2 baro;
int TEMP_seen = 0;

// For lux sensor
Adafruit_VEML7700 veml = Adafruit_VEML7700();
int LUX_seen = 0;

// Soil sensor
Adafruit_seesaw seesaw_sensor;
int SOIL_seen = 0;

//For MUX
void tcaselect(uint8_t i);

//Sensor locations for MUX
#define TVOC_ch 1
#define TEMP_ch 2
#define LUX_ch  3

// Set up before the loop
void setup() {
  Serial.begin(115200);
  
  //Check we have connection to sensor
  if (! ags.begin(&Wire, 0x1A)) 
  {
    Serial.println("Couldn't find AGS20MA sensor, check your wiring and pullup resistors!"); //Needs 10k pullup
  } else {
    Serial.println("Found AGS20MA sensor.");
    TVOC_seen = 1;
  }

  //Check TEMP sesnor
  if (!baro.begin())
  {
    Serial.println("Could not find Temperature/Barometric sensor. Check wiring.");
  } else {
    //Set sea level pressure
    baro.setSeaPressure(1019.5);
    Serial.println("Found Temperature/Barometric sensor.");
    TEMP_seen = 1;
  }
  
  //Check LUX sensor
  if (!veml.begin())
  {
    Serial.println("Lux sensor not found");
  } else
  {
    Serial.println("Lux sensor found");
    LUX_seen = 1;
    Serial.print(F("Gain: "));
    switch (veml.getGain()) {
      case VEML7700_GAIN_1: Serial.println("1"); break;
      case VEML7700_GAIN_2: Serial.println("2"); break;
      case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
      case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
    }

    Serial.print(F("Integration Time (ms): "));
    switch (veml.getIntegrationTime()) {
      case VEML7700_IT_25MS: Serial.println("25"); break;
      case VEML7700_IT_50MS: Serial.println("50"); break;
      case VEML7700_IT_100MS: Serial.println("100"); break;
      case VEML7700_IT_200MS: Serial.println("200"); break;
      case VEML7700_IT_400MS: Serial.println("400"); break;
      case VEML7700_IT_800MS: Serial.println("800"); break;
    }

    veml.setLowThreshold(10000);
    veml.setHighThreshold(20000);
    veml.interruptEnable(false);
  }

  if (!seesaw_sensor.begin())
  {
    Serial.println("Soil sensor not found");
  } else {
    SOIL_seen = 1;
    Serial.println("Soil sensor found");
  }
}

// To poll the Embedded system
void loop() {
    
  //Only run if TVOC was seen
  if(TVOC_seen == 1)
  {
    tcaselect(TVOC_ch);
    Wire.setClock(10000);
    uint32_t resistance = ags.getGasResistance();
    uint32_t tvoc = ags.getTVOC();

    if (resistance == 0) {
      Serial.println(F("Failure reading resistance, I2C communications issue?"));
    } else {
      float kohm = resistance / 1000.0;
      Serial.print(F("Gas resistance: "));
      Serial.print(kohm);
      Serial.println(" Kohms");
    }

    if (tvoc == 0) {
      Serial.println(F("Failure reading TVOC, I2C communications issue?"));
    } else {
      Serial.print(F("TVOC: "));
      Serial.print(tvoc);
      Serial.println(" ppb");
    }
  }

  //Only run if TEMP was seen
  if(TEMP_seen == 1)
  {
    tcaselect(TEMP_ch);
    float pressure = baro.getPressure();
    float altitude = baro.getAltitude();
    float temperature = baro.getTemperature();

    Serial.println("-----------------");
    Serial.print("pressure = "); Serial.print(pressure); Serial.println(" hPa");
    Serial.print("altitude = "); Serial.print(altitude); Serial.println(" m");
    Serial.print("temperature = "); Serial.print(temperature); Serial.println(" C");
  }

  //Only run if LUX is connected
  if(LUX_seen == 1)
  {
    tcaselect(LUX_ch);
    Serial.print("raw ALS: "); Serial.println(veml.readALS());
    Serial.print("raw white: "); Serial.println(veml.readWhite());
    Serial.print("lux: "); Serial.println(veml.readLux());
  }

  // Only run if SOIL is detected
  if(SOIL_seen == 1)
  {
    float tempC = ss.getTemp();
    uint16_t capread = ss.touchRead(0);

    Serial.print("Temperature: "); Serial.print(tempC); Serial.println("*C");
    Serial.print("Capacitive: "); Serial.println(capread);
  }

  delay(250);
}

//Helper Function for the Mux
#define TCAADDR 0x70

void tcaselect(uint8_t i) {
  if (i > 7){
  Serial.print("Mux channel out of range."); return;
  }
  Wire.setClock(100000);
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

