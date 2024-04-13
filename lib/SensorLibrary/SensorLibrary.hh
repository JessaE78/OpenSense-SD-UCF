// SensorLibrary.hh
#ifndef SENSORLIBRARY_HH
#define SENSORLIBRARY_HH

#include "Adafruit_MCP9808.h"
#include "Adafruit_SGP30.h"
#include "Adafruit_seesaw.h"
#include <Adafruit_ADXL343.h>
//#include <vl53l4cd_class.h>
#include <Adafruit_BME280.h>
#include "Adafruit_TSL2591.h"
#include "Adafruit_AGS02MA.h"
#include "Adafruit_VEML7700.h"
#include <Adafruit_MPL3115A2.h>
#include <Wire.h>
#include <Arduino.h>
////////////////////////////////////////////////////////////////////////////////////////////////
// #ifdef i2c_Speed = 100000; // This is the basic speed of 100kHz
// #endif
#define default_Speed 100000
#define MCP9808_Speed 100000
#define SGP30_Speed 100000
#define ADXL343_Speed 100000
#define Seesaw_Speed 100000
#define VL53L4CD_Speed 100000
#define BME280_Speed 100000
#define TSL2591_Speed 100000
#define AGS02MA_Speed 20000
#define VEML7700_Speed 100000
#define MPL3115A2_Speed 100000
////////////////////////////////////////////////////////////////////////////////////////////////

#define SEALEVELPRESSURE_HPA (1013.25)

class SensorLibrary {
public:
    // Constructor
    SensorLibrary();
    
    // Initialization methods
    String Start_MCP9808_Sensor();
    String Start_SGP30_Sensor();
    String Start_ADXL343_Sensor();
    String Start_Seesaw_Sensor();
    String Start_BME280_Sensor();
    String Start_TSL2591_Sensor();
    String Start_AGS02MA_Sensor();
    String Start_VEML7700_Sensor();
    String Start_MPL3115A2_Sensor();

    // Polling methods
    float Poll_MCP9808_Sensor();
    String Poll_SGP30_Sensor(String choice);
    float* Poll_ADXL343_Sensor();
    String Poll_Seesaw_Sensor(String choice);
    String Poll_BME280_Sensor(String choice);
    String Poll_TSL2591_Sensor(String choice);
    float Poll_AGS02MA_Sensor();
    float Poll_VEML7700_Sensor();
    String Poll_MPL3115A2_Sensor(String choice);

private:
    // Sensor instances
    Adafruit_MCP9808 mcp9808 = Adafruit_MCP9808();
    Adafruit_SGP30 sgp30;
    Adafruit_seesaw stemmasoil;
    Adafruit_ADXL343 adxl343 = Adafruit_ADXL343(12345); // Pass in a number for the sensor identifier (for your use later)
    // VL53L4CD sensor_vl53l4cd_sat;
    Adafruit_BME280 bme280;
    Adafruit_TSL2591 tsl2591  = Adafruit_TSL2591(2591);;
    Adafruit_AGS02MA ags02ma;
    Adafruit_VEML7700 veml7700 = Adafruit_VEML7700();;
    Adafruit_MPL3115A2 mpl3115a2;
};

#endif