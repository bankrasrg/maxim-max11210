/*
  Example dual-tmp36.ino will read the temperature using two TMP36 temperature sensors. One sensor
  will be read by the analog input pin of the Spark module. The signal of the second sensor will
  be read by a high resolution A/D converter. Both measurements will be converted to temperatures
  in degrees Celsius and made available to the outside world.

  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/Sensor1?access_token={ACCESS_TOKEN}
  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/Sensor2?access_token={ACCESS_TOKEN}  
  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/Temp1?access_token={ACCESS_TOKEN}
  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/Temp2?access_token={ACCESS_TOKEN}
*/

#include "maxim-max11210.h"

int sensor1 = 0;
double temperature1 = 0.0;
double sensor2 = 0;
double temperature2 = 0.0;

// Declare the peripheral A/D converter as type Max11210 from the Maxim namespace
Maxim::Max11210 adcMax11210;

void setup() {
  Spark.variable("Sensor1", &sensor1, INT);
  Spark.variable("Temp1", &temperature1, DOUBLE);
  Spark.variable("Sensor2", &sensor2, DOUBLE);
  Spark.variable("Temp2", &temperature2, DOUBLE);
  pinMode(A7, INPUT);
  adcMax11210.begin();
}

void loop() {
  sensor1 = analogRead(A7);
  sensor2 = adcMax11210.read();
  
  // TMP36: 0.5 V offset voltage, 10 mV/degC, 750 mV at 25 degC
  // 12-bit A/D for sensor 1: (2^12 - 1) = 4095 steps
  // 24-bit A/D for sensor 2: (2^24 - 1) = 1677215 steps 
  temperature1 = (3.3 * sensor1 / 4095.0 - 0.5) * 100.0;
  temperature2 = (3.3 * sensor2 / 16777215.0 - 0.5) * 100.0;
}
