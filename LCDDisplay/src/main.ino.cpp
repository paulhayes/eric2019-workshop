# 1 "c:\\users\\paul\\appdata\\local\\temp\\tmpk72xv8"
#include <Arduino.h>
# 1 "C:/Users/Paul/Workspace/Eric2019/LCDDisplay/src/main.ino"
#include <Arduino.h>
# 44 "C:/Users/Paul/Workspace/Eric2019/LCDDisplay/src/main.ino"
#include <LiquidCrystal.h>



const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void setup();
void loop();
#line 51 "C:/Users/Paul/Workspace/Eric2019/LCDDisplay/src/main.ino"
void setup() {

  lcd.begin(16, 2);

  delay(500);
  lcd.clear();
  lcd.print("test  ");
  lcd.setCursor(0,1);
  lcd.print("0000");
  Serial.begin(9600);


}

void loop() {

  if (Serial.available()) {

    delay(100);

    lcd.clear();

    while (Serial.available() > 0) {

      lcd.write(Serial.read());
    }
  }
}