#include <EEPROM.h> //permanent memory

class Memory {

private:
  int memAlok;
  int memUsed = 175 + 50;

  void report(String msg, bool offset = false) {
    if(offset) {Serial.println("");}
    Serial.println(msg);
  }

  int findFreeMem() {
    //check the occupied memory
    int start, end;
    String text;
    start = 0;
    for (int i = 1; i < 50 + 1; i++) { 
      int one = EEPROM.read(i);
      if (one == 255) {
        break;
      } else {
        if (start == 0) {start = i;}
        end = i;      
        text += String(char(one));    
      }
    }
    report ("memOccupied: "+ text, true);
    report ("start: " + String(start) + "; end: " + String(end));
    if (end == 50) {end = 0;}
    return end + 1;
  } 

public:
  
  Memory() {}

  void begin() {
    EEPROM.begin(memUsed);  
    report("EEPROM reading", true);
    //alokace paměti
    memAlok = EEPROM.read(0);
    if (memAlok > 50) {
      memAlok = 1;    
    } else if (memAlok == 255) {
      memAlok = findFreeMem();
    }
    report("memAlokFrom: " + String(memAlok)); 
    if (EEPROM.read(0) != memAlok) {
      clear();
      EEPROM.write(0, memAlok);
      EEPROM.commit();
    }
  }

  void clear() {
    bool change;
    for (int i = memAlok; i <= memUsed; i++) { 
      if (EEPROM.read(i) != 255) {
        EEPROM.write(i, 255);
        change = true;
      }
    }  
    EEPROM.write(0, 255);
    if(change) {EEPROM.commit();}
    report("EEPROM cleared"); 
  }

  int read(int pos) {
    return EEPROM.read(pos + memAlok);
  } 

  String read(int start, int end) {
    start = start + memAlok;
    end = end + memAlok;
    String text;
    for (int i = start; i < end + 1; i++) { 
      int one = EEPROM.read(i);
      if (one == 255) {
        break;
      } else {
        text += String(char(one));    
      }
    }
    return text;
  }

  int readAndCheck(int def, int pos, String text, int min, int max) {
    int value = read(pos);
    if (value != 255) {report(text + ": " + String(value));}
    if (value >= min and value <= max) {
      return value;
    } else {
      return def;
    }
  }

  String readAndCheck(String def, String text, int start, int end, bool password) {
    String value = read(start, end);
    if (value != "") {
      if (password == true) {
        report(text + ": *"); 
      } else {
        report(text + ": " + value); 
      }   
      return value;
    } else {
      return def;
    }   
  }

  void write(int value, int pos) {
    pos = pos + memAlok;
    if (EEPROM.read(pos) != value) {
      EEPROM.write(pos, value);
      EEPROM.commit();
      report("EEPROM_" + String(pos) + "(" + String(pos - memAlok) + "): " + String(value));     
    }
  }

  void write(String text, int start, int end) {
    start = start + memAlok;
    end = end + memAlok;
    for (int i = 0; i < end - start + 1; i++) { 
      byte one;
      if (i < text.length()) {
        one = byte(text.charAt(i));
      } else {
        one = 255;
      }
      if (EEPROM.read(i + start) != one) {
        EEPROM.write(i + start, one);  
      }
    }
    EEPROM.commit();
    report("EEPROM_" + String(start) + "-" + String(end) + "(" + String(start - memAlok) + "-" + String(end - memAlok) + "): " + text);   
  }

};
