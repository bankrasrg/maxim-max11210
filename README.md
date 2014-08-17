About
===

This library has been written to support the Maxim MAX11210 24-bit A/D converter
as peripheral of the Spark module. Spark libraries can be used in the [Spark IDE
](https://www.spark.io/build).

Status
===

Debugging is ongoing. The self-calibration of the ADC seems to result in a value
0 in the SCGC register, which will make it impossible to get valid data out.
With disabled self-calibration registers, values are read from the data register.
A work-around has been implemented for the SPI safety in the firmware of the
Spark Core, which prevents reading the MISO to detect completion of a conversion.

Example Usage
===

The code snippet below shows how to add the high resolution A/D converter to the 
example for [measuring the temperature](http://docs.spark.io/examples/#measuring-the-temperature). 

```bash
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
```

In a second example the configuration settings of the MAX11210 are changed by sending commands through a function of the Spark module.

```bash
#include "maxim-max11210.h"

long sensor2 = 0;
long temperature2 = 0;

// Declare the peripheral A/D converter as type Max11210 from the Maxim namespace
Maxim::Max11210 adcMax11210;

void setup() {
  Spark.variable("Temp2", &temperature2, DOUBLE);
  Spark.function("testFunction", testFunction);
  adcMax11210.begin();
}

void loop() {
  sensor2 = adcMax11210.read();
  temperature2 = (3.3 * sensor2 / 16777215.0 - 0.5) * 100.0;
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
  if (command == "GAIN1") {
    adcMax11210.setGain(MAX11210_GAIN1);
    return 1;
  }
  if (command == "GAIN2") {
    adcMax11210.setGain(MAX11210_GAIN2);
    return 1;
  }
  // Simply add other commands here ...
  return -1;
}
```

Recommended Components
===

[Maxim MAX11210](http://datasheets.maximintegrated.com/en/ds/MAX11200-MAX11210.pdf)

[Analog Devices TMP36](http://www.analog.com/static/imported-files/data_sheets/TMP35_36_37.pdf)

[Adafruit TSSOP-16 breakout PCB](http://www.adafruit.com/products/1207)

Circuit Diagram
==

See the schematic and breadboard view below are an example of combining the 
Spark module with the ADC and a temperature sensor. 

![Breadboard](doc/breadboard_dual-tmp36.png)

A schematic and breadboard view of how to wire up components with the library.
