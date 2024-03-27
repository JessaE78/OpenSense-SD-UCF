////////////////////////////////////////////////////////////////////////////////////////////////
// Wrapper Library for containing all the sensor information and what is neccessary to poll/begin
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor Libraries
// Description:
// All libraries included with supported sensors along with any declrations needed by library above
////////////////////////////////////////////////////////////////////////////////////////////////
#include "Adafruit_MCP9808.h"   // MCP9808 Temp Sensor
#include "Adafruit_SGP30.h"     // SGP30 TVOC/eC02 air quality sensor
#include "Adafruit_seesaw.h"    // Stemma Soil Sensor capacitive moisture sensor
#include <Adafruit_ADXL343.h>   // ADXL343 Triple-axis Accelerometer
#include <vl53l4cd_class.h>     // VL53L4CD Time of Flight Sensor. Had a lot of libraries inserted must check this sensor works
#define DEV_I2C Wire            // Used with VL53L4CD
#include <Adafruit_BME280.h>    // BME280 Temp Humidity Sensor
#define SEALEVELPRESSURE_HPA (1013.25) // For the BME280 may have to change
#include "Adafruit_TSL2591.h"   // TSL2591 Range Digital Light Sensor
#include "Adafruit_AGS02MA.h"   // AGS02MA TVOC Gas Sensor
#include "Adafruit_VEML7700.h"  // VEML7700 Lux Sensor
#include <Adafruit_MPL3115A2.h> // MPL3115A2 Altimeter
////////////////////////////////////////////////////////////////////////////////////////////////
// Object Declarations
// Descriptiion:
// These are object declarations taken from example projects. Some names have been changed. 
////////////////////////////////////////////////////////////////////////////////////////////////
Adafruit_MCP9808 mcp9808 = Adafruit_MCP9808();
Adafruit_SGP30 sgp30;
Adafruit_seesaw stemmasoil;
Adafruit_ADXL343 adxl343 = Adafruit_ADXL343(12345); // Pass in a number for the sensor identifier (for your use later)
VL53L4CD sensor_vl53l4cd_sat(&DEV_I2C, A1);
Adafruit_BME280 bme280;
Adafruit_TSL2591 tsl2591 = Adafruit_TSL2591(2591); // Pass in a number for the sensor identifier (for your use later)
Adafruit_AGS02MA ags02ma;
Adafruit_VEML7700 veml7700 = Adafruit_VEML7700();
Adafruit_MPL3115A2 mpl3115a2;
////////////////////////////////////////////////////////////////////////////////////////////////
// Sensors ID
// Description:
// These are subject to change once a discussion is talked about getting data from website.
////////////////////////////////////////////////////////////////////////////////////////////////
#define Not_Selected      0
#define MCP9808_Sensor    1
#define SGP30_Sensor      2
#define ADXL343_Sensor    3
#define Seesaw_Sensor     4
#define VL53L4CD_Sensor   5
#define BME280_Sensor     6
#define TSL2591_Sensor    7
#define AGS02MA_Sensor    8
#define VEML7700_Sensor   9
#define MPL3115A2_Sensor  10
////////////////////////////////////////////////////////////////////////////////////////////////
// Sensors i2c Speed
// Description: 
// Some i2c sensors operate at a lower speed like the AGS02MA so when communicating 
// we must check speed
////////////////////////////////////////////////////////////////////////////////////////////////
int i2c_Speed = 100000; //This is the basic speed of 100kHz
#define default_Speed    100000
#define MCP9808_Speed    100000
#define SGP30_Speed      100000
#define ADXL343_Speed    100000
#define Seesaw_Speed     100000
#define VL53L4CD_Speed   100000
#define BME280_Speed     100000
#define TSL2591_Speed    100000
#define AGS02MA_Speed    20000
#define VEML7700_Speed   100000
#define MPL3115A2_Speed  100000
////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor Functions to initialize Must add code here
// Input : None, some may need though havent completed
// Output: Will output debugging message and should output something to website
////////////////////////////////////////////////////////////////////////////////////////////////
void Start_MCP9808_Sensor(){

  delay(1000); // Needed for intial temperature conversion
  if(!mcp9808.begin()){
    //Send message to website error connecting to sensor
    Serial.print("Error connecting to MCP9808 Temperature Sensor");
  }else{
    mcp9808.wake(); // Wake up sensor to read
    mcp9808.setResolution(3); // Sets resolution mode of reading reference below
      // Mode Resolution SampleTime
      //  0    0.5°C       30 ms
      //  1    0.25°C      65 ms
      //  2    0.125°C     130 ms
      //  3    0.0625°C    250 ms
  }
  // Turn on green light next to sensor maybe???
}
void Start_SGP30_Sensor(){}
void Start_ADXL343_Sensor(){}
void Start_Seesaw_Sensor(){}
void Start_VL53L4CD_Sensor(){}
void Start_BME280_Sensor(){}
void Start_TSL2591_Sensor(){}
void Start_AGS02MA_Sensor(){
  Wire.setClock(20000);
  if(!ags02ma.begin())
  {
    Serial.println("Couldn't find AGS20MA sensor, this device needs 10k resistors on SCL SDA!"); //Needs 10k pullup
  }
  // Turn on green light??
  Wire.setClock(default_Speed);
}
void Start_VEML7700_Sensor(){}
void Start_MPL3115A2_Sensor(){}

////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor Functions for Polling
// Input: 
// Potenially add input of if we want to poll for C or F?? or some sensors have multiple readings
// like tmp humid and pressure
// Output: 
////////////////////////////////////////////////////////////////////////////////////////////////
float Poll_MCP9808_Sensor(){ 
  // If power issues occure can shutdown this after getting reading
  return mcp9808.readTempC();
}
int Poll_SGP30_Sensor(){ }
int Poll_ADXL343_Sensor(){}
int Poll_Seesaw_Sensor(){}
int Poll_VL53L4CD_Sensor(){}
int Poll_BME280_Sensor(){}
int Poll_TSL2591_Sensor(){}
uint32_t Poll_AGS02MA_Sensor(){
  Wire.setClock(20000);
  uint32_t tmp = ags02ma.getTVOC();
  Wire.setClock(default_Speed);
  return tmp;
}
int Poll_VEML7700_Sensor(){}
int Poll_MPL3115A2_Sensor(){}

