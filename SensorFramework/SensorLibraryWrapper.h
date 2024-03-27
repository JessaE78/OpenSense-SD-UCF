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
void Start_SGP30_Sensor(){
  if (! sgp30.begin()){
    Serial.println("Sensor not found :(");
  }
}
void Start_ADXL343_Sensor(){
   if(!adxl343.begin())
  {
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
  }else{
    adxl343.setRange(ADXL343_RANGE_16_G);
  }
}
void Start_Seesaw_Sensor(){
   if (!stemmasoil.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
  }
}
void Start_VL53L4CD_Sensor(){
  // Configure VL53L4CD satellite component.
  sensor_vl53l4cd_sat.begin();

  // Switch off VL53L4CD satellite component.
  sensor_vl53l4cd_sat.VL53L4CD_Off();

  //Initialize VL53L4CD satellite component.
  sensor_vl53l4cd_sat.InitSensor();

  // Program the highest possible TimingBudget, without enabling the
  // low power mode. This should give the best accuracy
  sensor_vl53l4cd_sat.VL53L4CD_SetRangeTiming(200, 0);

  // Start Measurements
  sensor_vl53l4cd_sat.VL53L4CD_StartRanging();
}
void Start_BME280_Sensor(){
  if (!bme280.begin()) {
    Serial.println("ERROR! BME280 not found");
  }
}
void Start_TSL2591_Sensor(){ //Has advanced options
  if (tsl2591.begin()) 
  {
    Serial.println(F("Found a TSL2591 sensor"));
  } 
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl2591.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl2591.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl2591.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  // tsl2591.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl2591.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  tsl2591.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl2591.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl2591.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl2591.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)
}
void Start_AGS02MA_Sensor(){
  Wire.setClock(20000);
  if(!ags02ma.begin())
  {
    Serial.println("Couldn't find AGS20MA sensor, this device needs 10k resistors on SCL SDA!"); //Needs 10k pullup
  }
  // Turn on green light??
  Wire.setClock(default_Speed);
}
void Start_VEML7700_Sensor(){
  if (!veml7700.begin()) {
    Serial.println("veml7700 sensor not found");
  }else{
    veml7700.getGain();
    veml7700.getIntegrationTime();
    veml7700.setLowThreshold(10000);
    veml7700.setHighThreshold(20000);
    veml7700.interruptEnable(false);
  }
}
void Start_MPL3115A2_Sensor(){
  if (!mpl3115a2.begin()) {
    Serial.println("Could not find mpl3115a2. Check wiring.");
  }
  mpl3115a2.setSeaPressure(1013.26);
}

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
float Poll_SGP30_Sensor(char* choice){
  if (! sgp30.IAQmeasure()) {
    Serial.println("Measurement failed");
  }
  if(choice == "TVOC")return sgp30.TVOC;
  else if (choice == "eCO2") return sgp30.eCO2;
}
float* Poll_ADXL343_Sensor(){
  float* array = (float*)malloc(3 * sizeof(float)); //Allocate array of values that will be x,y,z
  sensors_event_t event;
  adxl343.getEvent(&event);
  array[0] = event.acceleration.x;
  array[1] = event.acceleration.y;
  array[2] = event.acceleration.z;
  return array;
}
uint16_t Poll_Seesaw_Sensor(){
  return stemmasoil.touchRead(0);
}
uint16_t Poll_VL53L4CD_Sensor(){
  VL53L4CD_Result_t results;
  sensor_vl53l4cd_sat.VL53L4CD_ClearInterrupt();
  sensor_vl53l4cd_sat.VL53L4CD_GetResult(&results);
  return results.distance_mm;
}
float Poll_BME280_Sensor(char* choice){
  if(choice == "Temperature"){
    return bme280.readTemperature();
  }else if(choice == "Pressure"){
    return (bme280.readPressure() / 100.0F);
  }else if(choice == "Altitude"){
    return bme280.readAltitude(SEALEVELPRESSURE_HPA);
  }else if(choice == "Humidity"){
    return bme280.readHumidity();
  }
}
uint16_t Poll_TSL2591_Sensor(char* choice){
  uint32_t lum = tsl2591.getFullLuminosity();
  uint16_t ir = lum >> 16 , full = lum & 0xFFFF;
  if(choice == "Visible"){
    return (full-ir);
  }else if(choice == "Fullspectrum"){
    return full;
  }else if(choice == "Infrared"){
    return ir;
  }else if(choice == "Lux"){
    return tsl2591.calculateLux(full, ir);
  }
}
uint32_t Poll_AGS02MA_Sensor(){
  Wire.setClock(20000);
  uint32_t tmp = ags02ma.getTVOC();
  Wire.setClock(default_Speed);
  return tmp;
}
float Poll_VEML7700_Sensor(){
  return veml7700.readLux();
}
float Poll_MPL3115A2_Sensor(char* choice){
  if(choice == "Pressure"){
    return mpl3115a2.getPressure();
  }else if(choice == "Altitude"){
    return mpl3115a2.getAltitude();
  }else if(choice == "Temperature"){
    return mpl3115a2.getTemperature();
  }
}

