// SensorLibrary.cpp
#include "SensorLibrary.hh"

SensorLibrary::SensorLibrary() {
    // Constructor code, if necessary
}

String SensorLibrary::Start_MCP9808_Sensor() {
    if (!mcp9808.begin(0x18)) {
        return "Failed to initialize MCP9808 on channel ";
    } else {
        mcp9808.setResolution(3); // Sets resolution mode of reading
        mcp9808.wake();           // Wake up sensor to read
    }
    return "";
}

String SensorLibrary::Start_SGP30_Sensor() {
    if (!sgp30.begin()) {
        return "Failed to initialize SGP30 on channel ";
    }
    sgp30.setIAQBaseline(0x8E68, 0x8F41); // Set baseline as needed
    return "";
}

String SensorLibrary::Start_ADXL343_Sensor() {
    if (!adxl343.begin()) {
        return "Failed to initialize ADXL343 on channel ";
    } else {
        adxl343.setRange(ADXL343_RANGE_16_G);
    }
    return "";
}

String SensorLibrary::Start_Seesaw_Sensor() {
    if (!stemmasoil.begin(0x36)) {
        return "Failed to initialize Seesaw on channel ";
    }
    return "";
}

String SensorLibrary::Start_BME280_Sensor() {
    if (!bme280.begin()) {
        return "Failed to initialize BME280 on channel ";
    }
    return "";
}

String SensorLibrary::Start_TSL2591_Sensor() {
    if (!tsl2591.begin()) {
        return "Failed to initialize TSL2591 on channel ";
    }
    tsl2591.setGain(TSL2591_GAIN_MED); // 25x gain
    tsl2591.setTiming(TSL2591_INTEGRATIONTIME_300MS);
    return "";
}

String SensorLibrary::Start_AGS02MA_Sensor() {
    Wire.setClock(20000);
    if (!ags02ma.begin()) {
        return "Failed to initialize AGS02MA on channel ";
    }
    Wire.setClock(default_Speed);
    return "";
}

String SensorLibrary::Start_VEML7700_Sensor() {
    if (!veml7700.begin()) {
        return "Failed to initialize VEML7700 on channel ";
    } else {
        veml7700.getGain();
        veml7700.getIntegrationTime();
        veml7700.setLowThreshold(10000);
        veml7700.setHighThreshold(20000);
        veml7700.interruptEnable(false);
    }
    return "";
}

String SensorLibrary::Start_MPL3115A2_Sensor() {
    if (!mpl3115a2.begin()) {
        return "Failed to initialize MPL3115A2 on channel ";
    }
    mpl3115a2.setSeaPressure(SEALEVELPRESSURE_HPA);
    return "";
}

float SensorLibrary::Poll_MCP9808_Sensor() {
    float temp = mcp9808.readTempF();
    return temp;
}

String SensorLibrary::Poll_SGP30_Sensor(String choice) {
    float data = -1;
    String valueJSON;

    if (!sgp30.IAQmeasure()) {
        Serial.println("Measurement failed");
    }
    if (choice == "eCO2") {
        data = sgp30.eCO2;
        valueJSON = "{\"primary\": \"SGP30\", \"value\": " + String(data) + ", \"unit\": \"ppm\"}";
    } else {
        data = sgp30.TVOC;
        valueJSON = "{\"primary\": \"SGP30\", \"value\": " + String(data) + ", \"unit\": \"ppb\"}";
    }
    return valueJSON;
}

float* SensorLibrary::Poll_ADXL343_Sensor() {
    float *array = new float[3]; // Allocate array of values that will be x,y,z
    sensors_event_t event;
    adxl343.getEvent(&event);
    array[0] = event.acceleration.x;
    array[1] = event.acceleration.y;
    array[2] = event.acceleration.z;
    return array;
}

String SensorLibrary::Poll_Seesaw_Sensor(String choice) {
    float data = -1;
    String valueJSON;
    if (choice == "Capacitive") {
        uint16_t capread = stemmasoil.touchRead(0);
        valueJSON = "{\"primary\": \"Seesaw\", \"value\": " + String(capread) + ", \"unit\": \"Cap.\"}";
    } else {
        data = stemmasoil.getTemp();
        valueJSON = "{\"primary\": \"Seesaw\", \"value\": " + String(data) + ", \"unit\": \"°F\"}";
    }
    return valueJSON;
}

String SensorLibrary::Poll_BME280_Sensor(String choice) {
    float data = -1;
    String valueJSON;

    if (choice == "Pressure") {
        data = bme280.readPressure() / 100.0F;
        valueJSON = "{\"primary\": \"BME280\", \"value\": " + String(data) + ", \"unit\": \"hPa\"}";
    } else if (choice == "Altitude") {
        data = bme280.readAltitude(SEALEVELPRESSURE_HPA);
        valueJSON = "{\"primary\": \"BME280\", \"value\": " + String(data) + ", \"unit\": \"m\"}";
    } else if (choice == "Humidity") {
        data = bme280.readHumidity();
        valueJSON = "{\"primary\": \"BME280\", \"value\": " + String(data) + ", \"unit\": \"%\"}";
    } else {
        data = bme280.readTemperature();
        valueJSON = "{\"primary\": \"BME280\", \"value\": " + String(data) + ", \"unit\": \"°C\"}";
    }
    return valueJSON;
}

String SensorLibrary::Poll_TSL2591_Sensor(String choice) {
    float data = -1;
    String valueJSON;

    uint32_t lum = tsl2591.getFullLuminosity();
    uint16_t ir = lum >> 16, full = lum & 0xFFFF;

    if (choice == "Visible") {
        data = full - ir;
        valueJSON = "{\"primary\": \"TSL2591\", \"value\": " + String(data) + ", \"unit\": \"vis\"}";
    } else if (choice == "Fullspectrum") {
        data = full;
        valueJSON = "{\"primary\": \"TSL2591\", \"value\": " + String(data) + ", \"unit\": \"fsp\"}";
    } else if (choice == "Infrared") {
        data = ir;
        valueJSON = "{\"primary\": \"TSL2591\", \"value\": " + String(data) + ", \"unit\": \"inf\"}";
    } else if (choice == "Lux") {
        data = tsl2591.calculateLux(full, ir);
        valueJSON = "{\"primary\": \"TSL2591\", \"value\": " + String(data) + ", \"unit\": \"lux\"}";
    }
    return valueJSON;
}

float SensorLibrary::Poll_AGS02MA_Sensor() {
    Wire.setClock(20000);
    uint32_t tmp = ags02ma.getTVOC();
    Wire.setClock(default_Speed);
    return tmp;
}

float SensorLibrary::Poll_VEML7700_Sensor() {
    return veml7700.readLux();
}

String SensorLibrary::Poll_MPL3115A2_Sensor(String choice) {
    float data = -1;
    String valueJSON;

    if (choice == "Pressure") {
        data = mpl3115a2.getPressure();
        valueJSON = "{\"primary\": \"MPL3115A2\", \"value\": " + String(data) + ", \"unit\": \"hPa\"}";
    } else if (choice == "Altitude") {
        data = mpl3115a2.getAltitude();
        valueJSON = "{\"primary\": \"MPL3115A2\", \"value\": " + String(data) + ", \"unit\": \"m\"}";
    } else if (choice == "Temperature") {
        data = mpl3115a2.getTemperature();
        valueJSON = "{\"primary\": \"MPL3115A2\", \"value\": " + String(data) + ", \"unit\": \"°C\"}";
    }
    return valueJSON;
}
