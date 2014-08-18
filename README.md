About
===

This library has been written to support the Maxim MAX11210 24-bit A/D converter
as peripheral of the Spark module. Spark libraries can be used in the [Spark IDE
](https://www.spark.io/build).

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
  Spark.variable("Temp1", &temperature1, DOUBLE);
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

In a second example the configuration settings of the MAX11210 are changed by
sending commands through a function of the Spark module.

```bash
#include "maxim-max11210.h"

double sensor2 = 0;
double temperature2 = 0.0;

// Declare the peripheral A/D converter as type Max11210 from the Maxim namespace
Maxim::Max11210 adcMax11210;

void setup() {
  Spark.variable("Temp2", &temperature2, DOUBLE);
  Spark.function("Command", cmdFunction);
  adcMax11210.begin();
}

void loop() {
  sensor2 = adcMax11210.read();
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

Both examples can be found in the example project dual-tmp36.ino provided with
this library.

Circuit Diagram
==

See the breadboard view below as an example of combining the Spark module with
the ADC and a temperature sensor. 

![Breadboard](doc/breadboard_dual-tmp36.png)

Recommended Components
===

[Maxim MAX11210](http://datasheets.maximintegrated.com/en/ds/MAX11200-MAX11210.pdf)

[Analog Devices TMP36](http://www.analog.com/static/imported-files/data_sheets/TMP35_36_37.pdf)

[Adafruit TSSOP-16 breakout PCB](http://www.adafruit.com/products/1207)

Status
==

Debugging is ongoing. After manually performing a full system calibration according
to the procedure in the data sheet, valid temperature measurements are provided.
Manually apply 0.0 V to the AINP-pin during system offset calibration. Apply
3.3 V to the AINP-pin during the system gain calibration. This gives me the following
data in the calibration registers:

```bash
SCG  = 0xBFE318
SCO  = 0xFFFF74
SCGC = 0x7FFF6C
SCOC = 0xFFF4E9

```

A work-around has been implemented for the SPI safety in the firmware of the
Spark Core, which prevents reading the MISO to detect completion of a conversion.
Note the wires connecting !RDY/DOUT to both MISO and D0 of the Spark Core.

