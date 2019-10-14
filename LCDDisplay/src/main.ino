#include <Arduino.h>
#include <SerialCommand.h>
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

SerialCommand cmd;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // initialize the serial communications:
  delay(500);
  lcd.clear();
  Serial.begin(115200);

  cmd.addCommand("??",info);
  cmd.addCommand("print",print);
  cmd.addCommand("PRINT",print);
  cmd.addCommand("clear",clear);
  cmd.setDefaultHandler(error);

}

void info(){
  Serial.println("LCDDisplay v1.0");
}

void error(const char * command){
    Serial.print("ERROR: Unknown command ");
    Serial.println(command);
}

void clear(){
  lcd.clear();
  Serial.println("ok");
}

void print(){
  char * buf = cmd.next();
  char * endPtr;
  long line = strtol(buf, &endPtr, 10);

  if(endPtr != buf){
    buf = cmd.next();
  }
  if(line>1)
    line=0;
  lcd.setCursor(0,line);
  while(buf!=NULL){
    lcd.print( buf );
    lcd.print(" ");
    buf = cmd.next();

  }
  Serial.println("ok");
}

void loop() {
  cmd.readSerial();
}