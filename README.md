About
===

This library has been written to support the Maxim MAX11210 24-bit A/D converter
as peripheral of the Spark module. Spark Libraries can be used in the [Spark IDE
](https://www.spark.io/build).

Example Usage
===

The code snippet below is an example of adding the 

```bash
#include "maxim-max11210.h"

long analogValue = 0;

Maxim::Max11210 adcMax11210;

void setup() {
  Spark.variable("analogValue", &analogValue, DOUBLE);
  adcMax11210.begin();
}

void loop() {
  analogValue = adcMax11210.read();
}
```

Recommended Components
===

[Maxim MAX11210](http://www.maximintegrated.com/en/products/MAX11210)

Circuit Diagram
==

See the schematic and breadboard view below are an example of combining the 
Spark module with the ADC and a temperature sensor. 

![Breadboard](./doc/fritzing_breadboard.png)
![Schematic](./doc/fritzing_schematic.png)

A schematic and breadboard view of how to wire up components with the library.
