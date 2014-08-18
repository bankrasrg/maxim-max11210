/*
  Example dual-tmp36.ino will read the temperature using two TMP36 temperature sensors. One sensor
  will be read by the analog input pin of the Spark module. The signal of the second sensor will
  be read by a high resolution A/D converter. Both measurements will be converted to temperatures
  in degrees Celsius and made available to the outside world.

  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/Sensor1?access_token={ACCESS_TOKEN}
  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/Sensor2?access_token={ACCESS_TOKEN}   
  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/Temp1?access_token={ACCESS_TOKEN}
  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/Temp2?access_token={ACCESS_TOKEN}
  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/SCG?access_token={ACCESS_TOKEN}
  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/SCO?access_token={ACCESS_TOKEN}
  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/SCGC?access_token={ACCESS_TOKEN}
  curl --get https://api.spark.io/v1/devices/{DEVICE_ID}/SCOC?access_token={ACCESS_TOKEN}
  
  curl https://api.spark.io/v1/devices/{DEVICE_ID}/Command?access_token={ACCESS_TOKEN} -d args="SELFCAL"
*/

#include "maxim-max11210.h"

int sensor1 = 0;
double temperature1 = 0.0;
double sensor2 = 0.0;
double temperature2 = 0.0;
double selfcalgain = 0;
double selfcaloffset = 0;
double syscalgain = 0;
double syscaloffset = 0;

// Declare the peripheral A/D converter as type Max11210 from the Maxim namespace
Maxim::Max11210 adcMax11210;

void setup() {
  Spark.variable("Sensor1", &sensor1, INT);
  Spark.variable("Temp1", &temperature1, DOUBLE);
  Spark.variable("Sensor2", &sensor2, DOUBLE);
  Spark.variable("Temp2", &temperature2, DOUBLE);
  Spark.variable("SCG", &selfcalgain, DOUBLE);
  Spark.variable("SCO", &selfcaloffset, DOUBLE);  
  Spark.variable("SCGC", &syscalgain, DOUBLE);
  Spark.variable("SCOC", &syscaloffset, DOUBLE);  
  Spark.function("Command", cmdFunction);
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

int cmdFunction(String command) {
  if (command == "SELFCAL") {
    adcMax11210.setDisableSelfCalGain(false);
    adcMax11210.setDisableSelfCalOffset(false);
    adcMax11210.selfCal();
    selfcalgain = adcMax11210.getSelfCalGain();        
    selfcaloffset = adcMax11210.getSelfCalOffset();
    return 1;
  }
  if (command == "SYSCALOFFSET") {
    // System level calibration should only be performed when a zero-scale signal can be presented to the ADC.
    // Give a rate command afterwards to reinitiate conversion.
    adcMax11210.setDisableSysOffset(false);
    adcMax11210.sysOffsetCal();
    syscaloffset = adcMax11210.getSysOffsetCal();
    return 1;
  }
  if (command == "SYSCALGAIN") {
    // System level calibration should only be performed when a full-scale signal can be presented to the ADC.
    // Give a rate command afterwards to reinitiate conversion.      
    adcMax11210.setDisableSysGain(false);
    adcMax11210.sysGainCal();
    syscalgain = adcMax11210.getSysGainCal();            
    return 1;
  }
  if (command == "RATE1") {
    adcMax11210.setRate(MAX11210_RATE1);
    return 1;
  }
  if (command == "RATE2_5") {
    adcMax11210.setRate(MAX11210_RATE2_5);
    return 1;
  }
  if (command == "RATE5") {
    adcMax11210.setRate(MAX11210_RATE5);
    return 1;
  }  
  if (command == "RATE10") {
    adcMax11210.setRate(MAX11210_RATE10);
    return 1;
  }
  if (command == "RATE15") {
    adcMax11210.setRate(MAX11210_RATE15);
    return 1;
  }  
  if (command == "RATE30") {
    adcMax11210.setRate(MAX11210_RATE30);
    return 1;
  }  
  if (command == "RATE60") {
    adcMax11210.setRate(MAX11210_RATE60);
    return 1;
  }
  if (command == "RATE120") {
    adcMax11210.setRate(MAX11210_RATE120);
    return 1;
  }
  if (command == "GAIN1") {
    adcMax11210.setGain(MAX11210_GAIN1);
    return 1;
  }
  if (command == "GAIN2") {
    adcMax11210.setGain(MAX11210_GAIN2);
    return 1;
  }
  if (command == "GAIN4") {
    adcMax11210.setGain(MAX11210_GAIN4);
    return 1;
  }
  if (command == "GAIN8") {
    adcMax11210.setGain(MAX11210_GAIN8);
    return 1;
  }
  if (command == "GAIN16") {
    adcMax11210.setGain(MAX11210_GAIN16);
    return 1;
  }
  return -1;
}
