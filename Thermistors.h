#include <DHTesp.h> //senzor teploty DHT22
#include <OneWire.h> //wire senzoru DS18B20
#include <DallasTemperature.h> //senzor teploty DS18B20
#include <SensirionI2CSht4x.h>
#include <Wire.h>

class Thermistors {

private:
  const byte pinD1 = D1;
  DHTesp dht;
  DallasTemperature *dsb;
  int sensor;
  float temperatureLast;
  int humidityLast;
  unsigned long lastRead; 
  OneWire wire1; //gpio
  SensirionI2CSht4x sht4x;

public:
  float fixTemperature;
  int fixHumidity;
  float temperature;
  int humidity;
  String ProductName;
  String SerialNumber;
  int quality; //quality of air
  bool connected;
  int interval; //read interval in seconds

  Thermistors(int timeInterval = 2) {
    interval = timeInterval;
  }

  void begin(int sensorDigital) {
    sensor = sensorDigital;
    connected = false;
    if (sensor == 1) {
      wire1.begin(pinD1);
      dht.setup(pinD1, DHTesp::DHT11);
      ProductName = "DHT11";
      connected = true;
    } else if (sensor == 2) {
      wire1.begin(pinD1);
      dht.setup(pinD1, DHTesp::DHT22);
      ProductName = "DHT22";
      connected = true;
    } else if (sensor == 3) {
      wire1.begin(pinD1);
      dsb = new DallasTemperature(&wire1);
      dsb->begin();
      ProductName = "DS18B20";
      connected = true;
    } else {
      // SENSIRION
      Wire.begin();
      sht4x.begin(Wire);
      uint16_t error;
      uint32_t serialNumber;
      error = sht4x.serialNumber(serialNumber);
      if (error) {
        Serial.print("SHT40 Not connected.");
      } else {
        connected = true;
        Serial.print("SHT40 connected.");
        ProductName = "SHT40";
        SerialNumber = String(serialNumber);
      }
    } 
  }

  bool loop() {
    if (connected == false) return false;
    if (millis() - lastRead >= interval*1000UL or lastRead == 0) {
      lastRead = millis();
      if (measure() == true) return true;
    }    
    return false;
  }

  bool measure() {
    if (connected == false) {return false;}
    float temp;
    float humid;
    if (sensor == 1 or sensor == 2) {
      temp = dht.getTemperature();  
      humid = dht.getHumidity();
    } else if (sensor == 3) {
      dsb->requestTemperatures();
      temp = dsb->getTempCByIndex(0);
    } else {
      uint16_t error;
      error = sht4x.measureHighPrecision(temp, humid);
      if (error) return false;
    }  
    if (temp > -99 and temp < 99) {
      temperature = (round(temp * 10)/10) - fixTemperature;
    }
    if (humid > 0 and humid < 100) {
      humidity = round(humid) - fixHumidity;
    }
    if (temperature != temperatureLast or humidity != humidityLast) {
      temperatureLast = temperature;
      humidityLast = humidity;
      return true;  
    }  
    return false;
  }

};
