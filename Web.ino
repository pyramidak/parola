String menu = "main";
String newFirmwareVersion, lockPass;
bool web_restartNeeded; // restart needed
////////////////////////////////////////////////////////////////////////////////////////////////
////       WEB REQUEST      ////////       WEB REQUEST      ////////       WEB REQUEST      ////
////////////////////////////////////////////////////////////////////////////////////////////////
void web_request(String &header) {
  //MAIN/////////////////////////////////////////////////////////////////////////////////
  if (header.indexOf("GET /main") >= 0) {
    menu = "main";
  } else if (header.indexOf("GET /restart") >= 0) {
    mqtt.disconnect();
    ESP.restart();
  } else if (header.indexOf("GET /reconnect") >= 0) {
    web_restartNeeded = false;
    wi_fi.reconnectNeeded = true;
    mqtt.reconnectNeeded = true;
  } else if (header.indexOf("GET /lock") >= 0) {
    menu = "lock";
  } else if (header.indexOf("GET /device") >= 0) {
    menu = "device";
  } else if (header.indexOf("GET /sensor") >= 0) {
    menu = "sensor";
  } else if (header.indexOf("GET /display") >= 0) {
    menu = "display";
  } else if (header.indexOf("GET /mqtt") >= 0) {
    menu = "mqtt";
  } else if (header.indexOf("GET /wifi") >= 0) {
    menu = "wifi";
  } else if (header.indexOf("GET /clear") >= 0) {
    menu = "clear";
  } else if (header.indexOf("GET /firmware") >= 0) {
    menu = "firmware";     
  //DISPLAY/////////////////////////////////////////////////////////////////////////////////////
  } else if (header.indexOf("?start_text=") >= 0) {
    display.text(setOption(display.start_text, "start_text", "start_text", 2, 31, 32, false));
  } else if (header.indexOf("?display_text=") >= 0) {
    display.text(findSubmit("display_text"));
  } else if (header.indexOf("?intensity=") >= 0) {
    display.brightness(setOption(display.brightness(), "intensity", "intensity", 0, 25, 23, false));            
  } else if (header.indexOf("?rows=") >= 0) {
    display.rows = setOption(display.rows, "rows", "rows", 1, 2, 9, true);
  } else if (header.indexOf("?columns=") >= 0) {
    display.columns = setOption(display.columns, "columns", "columns", 1, 20, 10, true);
  //TERMISTORS/////////////////////////////////////////////////////////////////////////////////////
  } else if (header.indexOf("?fix_temp=") >= 0) {
    float value = findSubmit("fix_temp").toFloat();
    if (value < 0) value = abs(value);
    if (value > 9.9) value = 9.9;
    int val = value * 10.0;
    mem.write(val, 166);
    termistor.fixTemperature = value;
    termistor.measure();
  } else if (header.indexOf("?fix_humi=") >= 0) {
    termistor.fixHumidity = setOption(termistor.fixHumidity, "fix_humi", "humidity correction", 0, 25, 167, false);                            
    termistor.measure();  
  //LOCK/////////////////////////////////////////////////////////////////////////////////////
  } else if (header.indexOf("GET /setlock") >= 0) {
    lockPass = "";
    menu = "main";
  } else if (header.indexOf("?unlock=") >= 0) {     
    lockPass = findSubmit("unlock");
  } else if (header.indexOf("?lock_pass=") >= 0) {
    lockPass = findSubmit("lock_pass");
    mem.write(lockPass, 170, 175);                 
  //DEVICE/////////////////////////////////////////////////////////////////////////////////////
  } else if (header.indexOf("?name=") >= 0) {
    deviceName = setOption(deviceName, "name", "device name", 15, 136, 150, true);
    wi_fi.deviceName = deviceName;
    mqtt.deviceName = deviceName;
  } else if (header.indexOf("?purpose=") >= 0) {
    device = setOption(device, "purpose", "device purpose", 1, 2, 1, true);          
  //SENSOR/////////////////////////////////////////////////////////////////////////////////////
  } else if (header.indexOf("?1analog=") >= 0) {
    sensorAnalog = setOption(sensorAnalog, "1analog", "analog sensor", 0, 2, 6, false);  
    light.begin(sensorAnalog);
  } else if (header.indexOf("?1digital=") >= 0) {
    sensorDigital1 = setOption(sensorDigital1, "1digital", "digital sensor", 0, 3, 7, false); 
    termistor.begin(sensorDigital1);    
  //MQTT/////////////////////////////////////////////////////////////////////////////////////
  } else if (header.indexOf("?server_mqtt=") >= 0) {
    mqtt.server = setOption(mqtt.server, "server_mqtt", "MQTT server", 25, 66, 90, true);
  } else if (header.indexOf("?user_mqtt=") >= 0) {
    mqtt.user = setOption(mqtt.user, "user_mqtt", "MQTT user", 15, 91, 105, true);
  } else if (header.indexOf("?pass_mqtt=") >= 0) {
    newMQTTpass = setOption(newMQTTpass, "pass_mqtt", "MQTT password", 15, 106, 120, true);
    mqtt.password = newMQTTpass;
  //WIFI/////////////////////////////////////////////////////////////////////////////////////
  } else if (header.indexOf("?ssid_wifi=") >= 0) {
    wi_fi.ssid = setOption(wi_fi.ssid, "ssid_wifi", "WIFI SSID", 15, 36, 50, true);
    mem.write(1, 3);
  } else if (header.indexOf("?pass_wifi=") >= 0) {
    newWIFIpass = setOption(newWIFIpass, "pass_wifi", "WIFI password", 15, 51, 65, true);
    wi_fi.password = newWIFIpass;
    mem.write(1, 3);
  } else if (header.indexOf("?pass_ota=") >= 0) {
    newOTApass = setOption(newOTApass, "pass_ota", "OTA password", 15, 121, 135, true);
  } else if (header.indexOf("?mode_wifi=") >= 0) {
    setOption(mem.read(3), "mode_wifi", "WIFI mode", 0, 1, 3, true);          
  } else if (header.indexOf("?ap_wifi=") >= 0) {
    wi_fi.switchAP = setOption(wi_fi.switchAP, "ap_wifi", "wifi switch exists", 0, 1, 4, false); 
  //FIRMWARE/////////////////////////////////////////////////////////////////////////////////////
  } else if (header.indexOf("?address_update=") >= 0) {
    update_server = setOption(update_server, "address_update", "server address", 15, 151, 165, false);      
  } else if (header.indexOf("?remote_update=") >= 0) {
    mqtt.updateStart = setOption(mqtt.updateStart, "remote_update", "remote update", 0, 1, 168, false);      
  //CLEAR/////////////////////////////////////////////////////////////////////////////////////
  } else if (header.indexOf("GET /save_clear") >= 0) {
    mem.clear();
    menu = "main";
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////
////        WEB PAGE        ////////        WEB PAGE        ////////        WEB PAGE        ////
////////////////////////////////////////////////////////////////////////////////////////////////
void web_page(WiFiClient &client, String &header) {
  web_createPage(client);
  // Web Page Heading
  client.println("<body><h3>pyramidak parola firmware " + firmwareVersion + "</h3>");
  client.println("<h1>" + deviceName + "</h1>");

  //LOCK/////////////////////////////////////////////////////////////////////////////////////
  if (mem.readAndCheck("", "weblock password", 170, 175, true) != lockPass) {
    createSubmit(client, "PASSWORD", lockPass, "", "unlock", 6, "access lock to firmware setting");
    return;
  }   
  //MAIN/////////////////////////////////////////////////////////////////////////////////////
  if (menu == "main") {
    createSubmit(client, "TEXT", display.text(), "New text", "display_text", display.columns * 2, "");    
    if (light.connected) {
      client.println("<p><b>Ambient Light sensor</b></br>");
      client.println("Sensor: " + light.ProductName + "</br>");
      client.println("<b>Light lux: " + String(light.lux) + "</b></p>");
    }
    if (termistor.connected) {
      client.println("<p><b>Thermistor sensor</b></br>");
      client.println("Sensor: " + termistor.ProductName + "</br>");
      if (termistor.SerialNumber != "") client.println("SN: " + termistor.SerialNumber + "</br>");
      client.println("<b>Temperature &#176;C: " + String(termistor.temperature) + "</b></br>");
      if (termistor.humidity != 0) {
        client.println("Humidity %: " + String(termistor.humidity) + "</p>");
      }
      createSubmit(client, "temperature correction", String(termistor.fixTemperature*-1), "0.0-9.9", "fix_temp", 3, "");
      createSubmit(client, "humidity correction", String(termistor.fixHumidity*-1), "0-25", "fix_humi", 2, "");
    }
    //MAIN MENU/////////////////////////////////////////////////////////////////////////////////////
    client.println("<p>MAIN MENU</p>");
    if (web_restartNeeded == true) {
      createMenu(client, "Restart", "restart");
      client.println("<p>RESTART NEEDED</p>");
    }
    createMenu(client, "Reload", "main");
    createMenu(client, "Lock", "lock");
    createMenu(client, "Device", "device");
    createMenu(client, "Display", "display");
    createMenu(client, "Sensor", "sensor");
    createMenu(client, "MQTT", "mqtt");
    createMenu(client, "WIFI", "wifi");
    createMenu(client, "Firmware", "firmware");
    createMenu(client, "Clear", "clear");
    createMenu(client, "Restart", "restart");
  //LOCK/////////////////////////////////////////////////////////////////////////////////////
  } else if (menu == "lock") {
    createSubmit(client, "PASSWORD", lockPass, "", "lock_pass", 6, "access lock to firmware setting");
    
    client.println("<p>LOCK MENU</p>");
    createMenu(client, "Lock", "setlock");
    createMenu(client, "Return", "main");
  //DISPLAY/////////////////////////////////////////////////////////////////////////////////////
  } else if (menu == "display") {
    if (device != 2) {
      createSubmit(client, "ROWS", display.rows, "1-2", "rows", 1, "");
      createSubmit(client, "COLUMNS", display.columns, "1-20", "columns", 2, "");
    }
    createSubmit(client, "BRIGHTNESS", display.brightness(), "0-15", "intensity", 2, "");
    if (device == 2) createSubmit(client, "START TEXT", display.start_text, "New text", "start_text", 2, ""); 

    client.println("<p>DISPLAY MENU</p>");
    createMenu(client, "Return", "main");    
  //DEVICE/////////////////////////////////////////////////////////////////////////////////////
  } else if (menu == "device") {
    createSubmit(client, "DEVICE PURPOSE", device, "1-2", "purpose", 1, "1-parola, 2-mini display");
    createSubmit(client, "DEVICE NAME", deviceName, "New name", "name", 15, "");
    
    client.println("<p>DEVICE MENU</p>");
    createMenu(client, "Return", "main");
    if (device == 1) {
      client.println("<p>Matrix Display: CLK D5, CS D6, DATA D7</p>");
    } else if (device == 2) {
      client.println("<p>Matrix Display: CLK D2, CS D3, DATA D4</p>");
    }
  //SENSOR/////////////////////////////////////////////////////////////////////////////////////
  } else if (menu == "sensor") {
    createSubmit(client, "ANALOG SENSOR", sensorAnalog, "0-2", "1analog", 1, "0-none, lightmeter 1-LM393, 2-TEMT6000");                 
    createSubmit(client, "DIGITAL SENSOR", sensorDigital1, "0-3", "1digital", 1, "0-none, thermometer 1-DHT11, 2-DHT22, 3-DS18B20");

    client.println("<p>SENSOR MENU</p>");
    createMenu(client, "Return", "main");
    client.println("SENSOR PIN:");
    client.println("<p>Analog A0, Digital D1</p>");
  //MQTT/////////////////////////////////////////////////////////////////////////////////////                            
  } else if (menu == "mqtt") {
    createSubmit(client, "BROKER", mqtt.server, "New broker", "server_mqtt", 15, "");
    createSubmit(client, "USER", mqtt.user, "New user", "user_mqtt", 15, "");
    createSubmit(client, "PASSWORD", newMQTTpass, "New pass", "pass_mqtt", 15, "");

    client.println("<p>MQTT MENU</p>");
    createMenu(client, "Return", "main"); 

    web_createMQTTmanual(client);             
  //WIFI///////////////////////////////////////////////////////////////////////////////////// 
  } else if (menu == "wifi") {
    createSubmit(client, "SSID", wi_fi.ssid, "New SSID", "ssid_wifi", 15, "");
    createSubmit(client, "PASSWORD", newWIFIpass, "New pass", "pass_wifi", 15, "");
    createSubmit(client, "NEXT START", mem.read(3), "New mode", "mode_wifi", 1, "0-Access Point, 1(2)-Client");
    createSubmit(client, "WIFI SWITCH", wi_fi.switchAP, "Exists", "ap_wifi", 1, "0-none, 1-WiFi AP");

    client.println("<p>WIFI MENU</p>");
    createMenu(client, "Return", "main");  
    client.println("switch PIN: D0");
  //FIRMWARE///////////////////////////////////////////////////////////////////////////////////// 
  } else if (menu == "firmware") {
    createSubmit(client, "UPDATE SERVER", update_server, "http://", "address_update", 15, "");
    createSubmit(client, "UPDATE START", mqtt.updateStart, "0-1", "remote_update", 15, "0 - manual only, 1 - mqtt command");
    createSubmit(client, "ARDUINO UPDATE", newOTApass, "New pass", "pass_ota", 15, "");

    client.println("<p>FIRMWARE MENU</p>");                                                     
    createMenu(client, "Return", "main");                             

    if (update_server != "") {
      if (header.indexOf("GET /update") >= 0) {
        client.println("<h3>update in progress...</br>");  
        client.println("...click on Return in a minute</h3>"); 
        client.println("<p>update " + update_Server_begin() + "</p>");    
      } else {
        String newVersion = update_Server_version();
        if (newVersion.length() > 5) {
          client.println("<h3>" + newVersion + "</h3>");                         
          newFirmwareVersion = "";
        } else { 
          newFirmwareVersion = newVersion;         
        }
        if (newFirmwareVersion != "") {
          if(newFirmwareVersion != firmwareVersion) {createMenu(client, "Update", "update");}
          client.println("<h3>firmware version on server " + newFirmwareVersion + "</h3>");
        }
      } 
    }          
  //CLEAR///////////////////////////////////////////////////////////////////////////////////// 
  } else if (menu == "clear") {
    client.println("<p>CLEAR MEMORY SETTINGS</p>");
    createMenu(client, "Confirm", "save_clear");
    createMenu(client, "Return", "main");                            
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////
////       MQTT MANUAL      ////////       MQTT MANUAL      ////////       MQTT MANUAL      ////
////////////////////////////////////////////////////////////////////////////////////////////////
void web_createMQTTmanual(WiFiClient client) {
  client.println("<p>Topics for central system of</br>");
  client.println("smart home as Home Assistant</p>");
  
  client.println("<p>DEVICE TOPICS</p>");
  
  client.println("<p>HA configuration:</br>");
  client.println("platform: mqtt</br>");

  client.println("<p>Commands:</br>");
  client.println("\"" + deviceName + "/restart/command\"</br>");
  client.println("\"" + deviceName + "/update/command\"</br>");
  client.println("\"" + deviceName + "/brightness/command\"</br>");
  client.println("\"" + deviceName + "/text/command\" payload: \"text|LEFT\"</br>");
  client.println("|effect:LEFT/MESH/BLINDS/WIPE/SLICE</br>");

  client.println("States:</br>");
  client.println("\"" + deviceName + "/status\"</br>");
  client.println("\"" + deviceName + "/ip/state\"</br>");
  client.println("\"" + deviceName + "/update/state\"</br>");
  client.println("\"" + deviceName + "/light\"</br>");
  client.println("\"" + deviceName + "/temperature\"</br>");
  client.println("\"" + deviceName + "/humidity\"</br>");

  client.println("<p>GLOBAL TOPICS</p>");

  client.println("<p>HA start - update devices states:</br>");
  client.println("\"pyramidak/ip/command\"</br>");
  client.println("\"pyramidak/update/command\"</br>");
}