#include <PubSubClient.h> //mqtt
#include <sps30.h> //struct sps_values
//#pragma once

class cMQTT {

private:
  WiFiClient *client;
  PubSubClient mqtt;
  String firmwareVersion;
  unsigned long lastReport;//time of last mqtt connection status report
  unsigned long lastConnect;  //mqtt connection try
  // MQTT TOPICs //
  String device_update_global = "pyramidak/update/command";
  String sensor_global        = "pyramidak/sensor/command";
  String device_restart       = "/restart/command";
  String device_update_command= "/update/command";
  String device_update_state  = "/update/state";
  String device_ip_state      = "/ip/state";
  String mqtt_status          = "/status";
  String text_command         = "/text/command"; 
  String brightness_command   = "/brightness/command";
  String ambient_light        = "/light";
  String temperature_state    = "/temperature";
  String humidity_state       = "/humidity";

  bool connected() {
    return mqtt.connected();
  }

  void connect() {
    // Attempt to connect
    if (mqtt.connect(deviceName.c_str(), user.c_str(), password.c_str())) {
      mqtt.publish((deviceName + mqtt_status).c_str(), "offline");
      report("MQTT was offline."); 
      
      mqtt.subscribe((deviceName + device_restart).c_str());
      report("restart subscribed");

      mqtt.subscribe((deviceName + device_update_command).c_str());
      mqtt.subscribe(device_update_global.c_str());
      update(firmwareVersion); 
      report("update subscribed");

      reportSensors = true;
      mqtt.subscribe(sensor_global.c_str());
      report("sensors subscribed");

      mqtt.subscribe((deviceName + brightness_command).c_str());
      report("brightness subscribed");

      mqtt.subscribe((deviceName + text_command).c_str());
      report("text subscribed");

      mqtt.publish((deviceName + mqtt_status).c_str(), "online");
      report("MQTT connected to " + server, true);  
      
    } else {
      report("MQTT connn.failed: " + String(mqtt.state()));
    }
  }

  void report(String msg, bool offset = false) {
    if(offset) {Serial.println("");}
    Serial.println(msg);
  }

public:
  // MQTT settings //
  String server     = "192.168.0.181";
  String user       = "";//mqtt_pc";
  String password   = "";//photosmart";
  String deviceName;
  bool callUpdate;
  bool reconnectNeeded; // force reconnect
  bool reportSensors;
  int updateStart;      //web server update start: 0 - manual only, 1 - mqtt command
  int diodBrightChange = -1;
  String textChanged;

  cMQTT() {}

  void begin(String deviceName_, String firmwareVersion_) {
    deviceName = deviceName_;
    firmwareVersion = firmwareVersion_;
    client = new WiFiClient();
    //mqtt = new PubSubClient();
    mqtt.setClient(*client);
    mqtt.setServer(server.c_str(), 1883);
    mqtt.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
  }

  void callback(char* topic, byte* payload, unsigned int length) {
    //convert topic to string to make it easier to work with
    String topicStr = topic; 
    String payloadStr = "";
    for (int i = 0; i < length; i++) { payloadStr = payloadStr + (char)payload[i]; }
      
    report("Message arrived:", true);  
    report("topic: " + topicStr);  
    report("payload: " + payloadStr);  

    if(topicStr == (deviceName + device_restart)) {
      disconnect();
      ESP.restart();
    }

    if(topicStr == (deviceName + device_update_command) or topicStr == device_update_global) {
      if (updateStart == 1) callUpdate = true;
    }

    if(topicStr == sensor_global) {
      update(firmwareVersion);
      reportSensors = true; 
    }

    if(topicStr == (deviceName + text_command)) textChanged = payloadStr; 

    if(topicStr == (deviceName + brightness_command)) diodBrightChange = payloadStr.toInt(); 
  }

  void disconnect() {
    reconnectNeeded = false;
    lastReport = 0;
    lastConnect = 0;
    mqtt.publish((deviceName + mqtt_status).c_str(), "offline");
    report("MQTT status: offline");
    mqtt.disconnect();
    mqtt.setServer(server.c_str(), 1883);
  }

  void loop() {
    if (reconnectNeeded == true) disconnect();
    if (mqtt.connected() == true) {
      if (millis() - lastReport >= 60*1000UL or lastReport == 0) {  
        lastReport = millis();  
        mqtt.publish((deviceName + mqtt_status).c_str(), "online");
        report("MQTT status: online");
      }
      mqtt.loop();    
    } else {
      if (millis() - lastConnect >= 10*1000UL or lastConnect == 0) {
        lastConnect = millis();
        connect();
      }
    }
  }

  void update(String version) { 
    mqtt.publish((deviceName + device_ip_state).c_str(), WiFi.localIP().toString().c_str()); 
    mqtt.publish((deviceName + device_update_state).c_str(), version.c_str());
    report("update state and IP address published: " + version);            
  }

  void light(int value) {
    if (mqtt.connected() == true and value > 0) { 
      mqtt.publish((deviceName + ambient_light).c_str(), String(value).c_str());
      report("ambient light published: " + String(value));
    }
  }

  void temp(float temperature, int humidity) { 
    if (mqtt.connected() == true) { 
      if (temperature != 0) { 
        mqtt.publish((deviceName + temperature_state).c_str(), String(temperature).c_str());    
        report("temperature published: " + String(temperature));
      }
      if (humidity != 0) { 
        mqtt.publish((deviceName + humidity_state).c_str(), String(humidity).c_str());    
        report("humidity published: " + String(humidity));
      }
    }
  }

};