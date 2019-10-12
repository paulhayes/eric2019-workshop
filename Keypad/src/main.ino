#include "Arduino.h"
#include "Keypad.h"

// 3     4     5     6     7     8     9
// COL2, ROW1, COL1, ROW4, COL3, ROW3, ROW2
// ROW2, ROW3, COL3, ROW4, COL1, ROW1, COL2
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {8, 3, 4, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 9, 5}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
    Serial.begin(9600);
}

void loop(){
    char key = keypad.getKey();
  
  if (key != NO_KEY){
    Serial.println(key);
  }
}