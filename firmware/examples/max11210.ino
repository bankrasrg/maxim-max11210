#include "maxim-max11210.h"

long analogValue = 0;

Maxim::Max11210 adcMax11210;

void setup() {
  Spark.variable("analogValue", &analogValue, DOUBLE);
  Spark.function("testFunction", testFunction);
  adcMax11210.begin();
}

void loop() {
  analogValue = adcMax11210.read();
}

int testFunction(String command) {
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
