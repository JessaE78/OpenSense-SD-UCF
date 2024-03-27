//Jordan Bartlett
// Basic framework for sensor integration with either wifi or ethernet. 

// Sensors currently
////////////////////////////////////////////////
// MCP9808 Temp Sensor
// SGP30 TVOC/eC02 air quality sensor
// Stemma Soil Sensor capacitive moisture sensor
// ADXL343 Triple-axis Accelerometer
// VL53L4CD Time of Flight Sensor
// BME280 Temp Humidity Sensor
// TSL2591 Range Digital Light Sensor
// AGS02MA TVOC Gas Sensor
// VEML7700 Lux Sensor
// MPL3115A2 Altimeter Must double check if its that one
////////////////////////////////////////////////

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
// Custom header with all the sensor information
#include "SensorLibraryWrapper.h"
// Mux address
#define TCAADDR 0x70

// Temporary variables that when recieved from website
bool update_sensor_button = false;
bool start_poll = false; // Have it so website starts the polling
bool polling = false; // To indicate if we are polling
bool stop = false; // Indicates an event to stop polling
const char* Channel1 = "Channel 1";
const char* Channel2 = "Channel 2";
const char* Channel3 = "Channel 3";
const char* Channel4 = "Channel 4";
const char* Channel5 = "Channel 5";
const char* Channel6 = "Channel 6";
const char* Channel7 = "Channel 7";
const char* Channel8 = "Channel 8";
int channels[8];
//Time tracking variables
unsigned long poll_rate = 1000; // 1 second
unsigned long startMillis,currentMillis;

void setup() {
  // Will need to add any setup code for serial, sd card, website (wifi/ethernet)
  Serial.begin(115200); 
  
}

void loop() {
  // Assuming loop will be added here for when someone connects

  // Within that loop or whever Update Sensor button is clicked on website
  if(update_sensor_button){
    // When update button is update channels
    updateChannels();
    beginSensors();
  }
  // Start polling
  if(start_poll){
      startMillis = millis();
      polling = true; // Start polling
  }
  // If we are supposed to be polling and elapsed time has happened output
  if(polling == true){
    start_poll = false; // Dont reset the time
    currentMillis = millis(); // Get current time to compare
    if (currentMillis - startMillis >= poll_rate)  //If poll rate time has passed
    {
      pollSensors(); // Poll the sensors
      startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
    }
  }

  // Stop polling if stop event is raised
  if (stop == true){
    polling = false;
    polling = false;
  }
  
    
  
  
}
// Call this to update channels from website
void updateChannels(){
  // Way to recieve data would look like
  /*
    channel[0] = request->getParam(Channel1)->value();
    channel[1] = request->getParam(Channel2)->value();
    channel[2] = request->getParam(Channel3)->value();
    channel[3] = request->getParam(Channel4)->value();
    channel[4] = request->getParam(Channel5)->value();
    channel[5] = request->getParam(Channel6)->value();
    channel[6] = request->getParam(Channel7)->value();
    channel[7] = request->getParam(Channel8)->value();
  */
}
// Call this after updating channels to initilize them all
void beginSensors(){
  //Loop through the 8 channels and depending on channel id begin diff sensor
  for(int i = 0; i<8; i++){
    channelSelect(i); // Make sure we are actually selecting channel before communicating
    switch(channels[i]){
      case Not_Selected: break; // No sensor on channel so dont begin
      case MCP9808_Sensor: Start_MCP9808_Sensor(); break; 
      case SGP30_Sensor: Start_SGP30_Sensor(); break;      
      case ADXL343_Sensor:  Start_ADXL343_Sensor(); break;
      case Seesaw_Sensor:  Start_Seesaw_Sensor(); break;
      case VL53L4CD_Sensor:  Start_VL53L4CD_Sensor(); break; 
      case BME280_Sensor:  Start_BME280_Sensor(); break; 
      case TSL2591_Sensor:  Start_TSL2591_Sensor(); break; 
      case AGS02MA_Sensor:  Start_AGS02MA_Sensor(); break; 
      case VEML7700_Sensor:  Start_VEML7700_Sensor(); break;
      case MPL3115A2_Sensor:  Start_MPL3115A2_Sensor(); break;
      default: break;
    }
  }
}
// Call this when wanting to poll sensors
void pollSensors(){
  //Loop through the 8 channels and depending on channel id begin diff sensor
  for(int i = 0; i<8; i++){
    channelSelect(i); // Make sure we are actually selecting channel before communicating
    switch(channels[i]){
      case Not_Selected: break; // No sensor on channel so dont begin
      case MCP9808_Sensor: Poll_MCP9808_Sensor(); break; 
      case SGP30_Sensor: Poll_SGP30_Sensor(); break;      
      case ADXL343_Sensor:  Poll_ADXL343_Sensor(); break;
      case Seesaw_Sensor:  Poll_Seesaw_Sensor(); break;
      case VL53L4CD_Sensor:  Poll_VL53L4CD_Sensor(); break; 
      case BME280_Sensor:  Poll_BME280_Sensor(); break; 
      case TSL2591_Sensor:  Poll_TSL2591_Sensor(); break; 
      case AGS02MA_Sensor:  Poll_AGS02MA_Sensor(); break; 
      case VEML7700_Sensor:  Poll_VEML7700_Sensor(); break;
      case MPL3115A2_Sensor:  Poll_MPL3115A2_Sensor(); break;
      default: break;
    }
  }
}

// Function for selecting channel on mux
void channelSelect(uint8_t i) {
  if (i > 7){
  Serial.print("Mux channel out of range. Must restart board"); while(1);
  }
  Wire.setClock(100000);
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}