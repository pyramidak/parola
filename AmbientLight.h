#include <SparkFun_VEML7700_Arduino_Library.h>
#include <Wire.h>
/************************************************************************************
//    FILE: Example1_getLux.ino
//  AUTHOR: Paul Clark
//    DATE: November 4th 2021
//     URL: http://librarymanager/All#SparkFun_VEML7700
//          https://www.sparkfun.com/products/18976
/************************************************************************************/
class AmbientLight {

private:
  #define pinA A0  //analog sensor
  VEML7700 light;
  unsigned long lastRead;
  int luxLast;
  int sensor;
  int averagePos;
  static const int sampleLength = 50;
  int averageVals[sampleLength];

  int avarage(int sample[], int length, bool zero) {
    long together = 0;
    int count = 0;
    for (int i = 0; i < length; i++) {
      if (sample[i] != 0 or zero == true) {
        count += 1;
        together += sample[i];  
      }
    }
    if (count == 0) {
      return 0;
    } else {
      return round(together / count);
    }
  }

public:
  String ProductName;
  bool connected;
  int interval;  //read interval in seconds
  int lux;

  AmbientLight(int timeInterval = 1) {
    interval = timeInterval;
  }

  void begin(int sensorAnalog) {
    sensor = sensorAnalog;
    connected = false;
    if (sensor == 1) {
      connected = true;
      ProductName = "LM393";
    } else if (sensor == 2) {
      connected = true;
      ProductName = "TEMT6000";   
    } else {
      Wire.begin();
      if (light.begin() == true) { 
        connected = true;
        ProductName = "VEML7700";
      }
    }
  }

  bool loop() {
    if (connected == true) {
      if (sensor == 1 or sensor == 2) {
        averageVals[averagePos] = analogRead(pinA);
        averagePos += 1;
        if(averagePos == sampleLength) {averagePos = 0;}
        lux = avarage(averageVals, sampleLength, false);
        if (sensor == 1) lux = 1024 - lux - 50;
        if (sensor == 2) lux = lux - 50;
        if (lux < 20) lux = 1;
      } else if (millis() - lastRead >= interval * 1000UL or lastRead == 0) {
        lastRead = millis();
        lux = round(light.getLux());
      } 
      if (abs(lux - luxLast) > 20) {
        luxLast = lux;
        return true;
      }
    }
    return false;
  }

};
