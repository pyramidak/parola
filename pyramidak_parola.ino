#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "Memory.h"
#include "cMQTT.h"
#include "cWiFi.h"
#include "AmbientLight.h"
#include "MatrixDisplay.h"
#include "Thermistors.h"

WiFiServer server(80);
Memory mem;
cMQTT mqtt;
cWiFi wi_fi(&mem);
AmbientLight light;
MatrixDisplay display;
Thermistors termistor;

/// copyright Zdeněk Jantač, all rights reserved. This is NOT public open software.
///////////////////////////////////////////////////////////////////////////////////
/// What is new
/// 1.1.6 float temperature correction
/// 1.1.5 stop backlight flashing under fluorescent lamp
/// 1.1.4 wifi sleep mode fixed
/// 1.1.3 restart update fixed
/// 1.1.2 server update fixed
///////////////////////////////
////    Settings block    /////
///////////////////////////////
String deviceName      = "parola";
String firmwareVersion = "1.1.6";
  // Update settings //
String OTA_password    = "photosmart";
String update_server   = "192.168.0.100";
  // DEVICE settings //
int device        = 1; //1 = parola, 2 = mini display
int sensorAnalog  = 0; //lightmeter 1-LM393, 2-TEMT6000
int sensorDigital1= 0; //0-none, thermometer 1-DHT11, 2-DHT22, 3-DS18B20
String newMQTTpass, newWIFIpass, newOTApass;
  // PINs declaration //
#define pinA  A0 //analog sensor
#define pinAP D0 //WiFi AP switch
////////////////////////////////
//// Report to serial port  ////
////////////////////////////////
void report(String msg, bool offset = false) {
  if(offset) {Serial.println("");}
  Serial.println(msg);
}
///////////////////////////////
////      Setup block     /////
///////////////////////////////
void setup() {
  Serial.begin(115200); //9600, 115200
  delay(200);
  eeprom_begin();
  pinMode(pinAP, INPUT);  // Wifi AP
  pinMode(pinA, INPUT);   // Analog Sensor  
  mqtt.begin(deviceName, firmwareVersion);
  light.begin(sensorAnalog);
  termistor.begin(sensorDigital1);
  display.begin(device, firmwareVersion);

  if (mem.read(3) == 0 or wi_fi.ssid == "" or wi_fi.password == "" or (wi_fi.switchAP == 1 and digitalRead(pinAP) == HIGH)) {
    wi_fi.beginAP(); 
  } else {
    wi_fi.begin(deviceName);
  }
  update_Arduino_begin();
}
///////////////////////////////
////       Loop block     /////
///////////////////////////////
void loop() {
  
  if (wi_fi.connected() == true) {
    if (wi_fi.connectedJobs() == true) {
      server.begin();
      display.text(wi_fi.IP + "|LEFT");
    }
    if (wi_fi.APmode == false) {
      mqtt.loop(); 
      if (mqtt.callUpdate == true) update_Server_check();
      update_Arduino_handle(); 
    }
    web_server();
  }
  delay(10); //MUST delay to allow ESP8266 WIFI functions to run

  //light sensor + mqtt
  if (light.loop() == true or (light.connected and mqtt.reportSensors == true)) {
    mqtt.light(light.lux);
    if (light.connected) display.brightness(round(float(15) / 1000 * light.lux));
    //report("diodMax: " + String(semafor.diodMax) + "; lux: " + String(light.lux) + "; calculation: " + String(float(semafor.diodMax) / 1000 * light.lux));
  }
  //termistors
  if (termistor.loop() == true or (termistor.connected and mqtt.reportSensors == true)) {
    mqtt.temp(termistor.temperature, termistor.humidity); 
    int temp = round(termistor.temperature);
    if (device == 1) display.text(String(temp) + "°C");
    if (device == 2) display.text(String(temp));
  }
  //mqtt brightness command
  if (mqtt.diodBrightChange >= 0) {
    display.brightness(mqtt.diodBrightChange);
    mqtt.diodBrightChange = -1;
  }
  //mqtt text command
  if (mqtt.textChanged != "") {
    display.text(mqtt.textChanged);
    mqtt.textChanged = "";
  }
  display.loop();
  mqtt.reportSensors = false;
}


