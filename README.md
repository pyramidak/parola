# pyramidak parola firmware
DIY smart home the easy way
https://firm.jantac.net/hardware-en/
1) About pyramidak led display PCB with schema that you can simply order from https://jlcpcb.com
2) Full manual about pyramidak firmware, what all you can set for display.
3) How to easily connect pyramidak firmware to open central system like Home Assistant.



Big thanks to these authors who made some functions of the pyramidak firmware easier for me, 
the list of libraries that need to be imported into the Arduino IDE:

#include <MD_Parola.h>
https://github.com/MajicDesigns/MD_Parola

#include <MD_MAX72xx.h>
https://github.com/MajicDesigns/MD_MAX72XX

#include <DHTesp.h>
https://github.com/markruys/arduino-DHT

#include <OneWire.h>
https://github.com/PaulStoffregen/OneWire

#include <DallasTemperature.h>
https://github.com/milesburton/Arduino-Temperature-Control-Library

#include <SensirionI2CSht4x.h>
https://github.com/Sensirion/arduino-i2c-sht4x

#include <SparkFun_VEML7700_Arduino_Library.h>
https://github.com/sparkfun/SparkFun_VEML7700_Arduino_Library
