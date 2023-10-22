#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include "SmallFont.h"
#include "DoubleFont.h"

class MatrixDisplay {

private:
  // Parola display settings //
  #define HARDWARE_TYPE MD_MAX72XX::FC16_HW // type of device hardware https://majicdesigns.github.io/MD_MAX72XX/page_hardware.html 
  #define ZONE_UPPER  1
  #define ZONE_LOWER  0
  int blocks;
  MD_Parola *display;
  bool constructed;
  char message[100];
  int intensity = 1;
  int thisDevice;
  textEffect_t effect = PA_NO_EFFECT;

  void stringToMessage(String text) {
    text.toCharArray(message, text.length() + 1);
    message[text.length() + 1] = '\0';
  }

public:
  String start_text = "*C";// First text 
  int rows = 1;
  int columns = 1;
  
  MatrixDisplay() {}

  void begin(int device, String text = "") {
    //(HARDWARE_TYPE, DATA_PIN: D7, CLK_PIN: D5, CS_PIN: D6, MAX_DEVICES)
    blocks = rows * columns;
    thisDevice = device;
    if (device == 1) {
      display = new MD_Parola(HARDWARE_TYPE, D7, D5, D6, blocks);
    } else if (device == 2) {
      display = new MD_Parola(HARDWARE_TYPE, D4, D2, D3, blocks);
    } else {
      return;
    }
    constructed = true;
    if (device == 2) text = start_text;
    stringToMessage(text);
    // initialise the LED display
    display->begin(blocks);
    if (rows == 1) {
      display->setZone(ZONE_LOWER, 0, columns - 1);
      display->setFont(ZONE_LOWER, SmallFont);
      display->setCharSpacing(0);
      display->displayZoneText(ZONE_LOWER, message, PA_CENTER, 30, 0, PA_NO_EFFECT, PA_NO_EFFECT);  
    } else {
      // Set up zones for 2 halves of the display
      display->setZone(ZONE_LOWER, 0, columns - 1);
      display->setZone(ZONE_UPPER, columns, blocks - 1);
      display->setFont(ZONE_LOWER, DoubleFontLower);
      display->setFont(ZONE_UPPER, DoubleFontUpper);
      display->setCharSpacing(1);
      display->displayZoneText(ZONE_UPPER, message, PA_CENTER, 30, 0, PA_NO_EFFECT , PA_NO_EFFECT);
      display->displayZoneText(ZONE_LOWER, message, PA_CENTER, 30, 0, PA_NO_EFFECT , PA_NO_EFFECT);
    } 
    brightness(intensity);
    display->displayAnimate();  
  }

  String text(String value = "") {
    if (value != "") {
      int hasZero = value.indexOf("0");
      int hasDvoj = value.indexOf(":");
      if (hasZero == 0 and hasDvoj == 2) value = value.substring(1); //remove one leading zero from time
      int hasEffect = value.indexOf("|");
      if (hasEffect >= 0) {
        effect = selectEffect(value.substring(hasEffect + 1));
        value = value.substring(0, hasEffect);
      }
      if (effect != PA_NO_EFFECT and constructed == true) {        
        if (hasEffect < 1) effect = PA_NO_EFFECT;
        int speed = 150 / columns;
        if (speed < 30) speed = 30;
        if (effect == PA_SLICE) speed = 3;
        if (effect == PA_BLINDS or effect == PA_MESH) speed = 60;
        if (effect == PA_SCROLL_LEFT) display->displayClear();
        display->displayZoneText(ZONE_UPPER, message, PA_CENTER, speed, 0, effect, PA_NO_EFFECT);
        display->displayZoneText(ZONE_LOWER, message, PA_CENTER, speed, 0, effect, PA_NO_EFFECT);            
      }
      stringToMessage(value);
      if (constructed == true) {
        display->displayReset();
        display->displayAnimate();
        display->synchZoneStart();
        if (thisDevice == 2) digitalWrite(LED_BUILTIN, HIGH); //kvůli připojení na pin LED zhasínat
      }  
    }
    return String(message);
  }

  int brightness(int value = 256) {
    if (value >= 0 and value < 255) {
      if (value > 15) {value = value / 17;}
      intensity = value;
      if (constructed == true) {display->setIntensity(value);}
    }
    return intensity;
  }

  void loop() {
    if (constructed == false) {return;}
    display->displayAnimate();
    display->synchZoneStart();
  }

  textEffect_t selectEffect(String value) {
    if (value == "LEFT") {
      return PA_SCROLL_LEFT;
    } else if (value == "MESH") {
      return PA_MESH;
    } else if (value == "SLICE") {
      return PA_SLICE;
    } else if (value == "BLINDS") {
      return PA_BLINDS;
    } else if (value == "WIPE") {
      return PA_WIPE;              
    } else {
      return PA_NO_EFFECT;
    }
  }

};
