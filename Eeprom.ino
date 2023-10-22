//Memory:
//  0: memAlok
//  1: device
//  2: sleep mode
//  3: wifi ready
//  4: WiFi AP switch
//  5: analogSW
//  6: sensorAnalog
//  7: sensorDigital
//  8: sensorDigital2
//  9: device rows
//  10: device columns
//  11-22:
// 23: brightness display
// 24-30: 
// 31-32: display start text
// 33-35: 
// 36-50: wifi name
// 51-65: wifi password
// 66-90: mqtt server
// 91-105: mqtt user
//106-120: mqtt password
//121-135: ota password
//136-150: device name
//151-165: server address
//166: temperature correction
//167: humidity correction
//168: remote update off
//169:
//170-175: web password

void eeprom_begin() { 
  mem.begin();
  device = mem.readAndCheck(device, 1, "devicePurpose", 1, 2);
  report("wifi: " + String(mem.read(3)) );
  wi_fi.switchAP = mem.readAndCheck(wi_fi.switchAP, 4, "wifiSwitch", 0, 2);
  mqtt.updateStart = mem.readAndCheck(1, 168, "updateRemote", 0, 1);
  sensorAnalog = mem.readAndCheck(sensorAnalog, 6, "sensorAnalog", 0, 2);
  sensorDigital1 = mem.readAndCheck(sensorDigital1, 7, "sensorDigital", 0, 3);
  if (device == 1) {
    display.rows = mem.readAndCheck(1, 9, "displayRows", 1, 2);
    display.columns = mem.readAndCheck(1, 10, "displayColumns", 1, 20);
  } else if (device == 2) {
    display.start_text = mem.readAndCheck(display.start_text, "displayText", 31, 32, false);
  }
  display.brightness(mem.readAndCheck(display.brightness(), 23, "brightnessDisplay", 0, 15));
  wi_fi.ssid = mem.readAndCheck(wi_fi.ssid, "wifiSSID", 36, 50, false);
  wi_fi.password = mem.readAndCheck(wi_fi.password, "wifiPass", 51, 65, true);
  if (wi_fi.password != "") newWIFIpass = "*";
  mqtt.server = mem.readAndCheck(mqtt.server, "MQTTserver", 66, 90, false);
  mqtt.user = mem.readAndCheck(mqtt.user, "MQTTuser", 91, 105, false);
  mqtt.password = mem.readAndCheck(mqtt.password, "MQTTpass", 106, 120, true);
  if (mqtt.password != "") newMQTTpass = "*";
  OTA_password = mem.readAndCheck(OTA_password, "OTApass", 121, 135, true);
  if (OTA_password != "") newOTApass = "*";
  deviceName = mem.readAndCheck(deviceName, "deviceName", 136, 150, false);
  update_server = mem.readAndCheck(update_server, "updateServer", 151, 165, false);
  termistor.fixTemperature = mem.readAndCheck(20, 166, "fixTemperature", 0, 99) / 10.0;
  termistor.fixHumidity = mem.readAndCheck(termistor.fixHumidity, 167, "fixHumidity", 0, 25);
}


