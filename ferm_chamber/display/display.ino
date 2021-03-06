#include <LiquidCrystal.h>

LiquidCrystal lcd(48, 49, 50, 51, 52, 53);
const int reset_pin = 9;
const int transistor = 8;

void setup() {
  pinMode (transistor, OUTPUT);
  pinMode (reset_pin, OUTPUT);
  digitalWrite (reset_pin, HIGH);
  digitalWrite (reset_pin, LOW);
  digitalWrite(reset_pin, HIGH);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Clears the LCD screen
  lcd.clear();
  // Print a message to the LCD.
  lcd.print(" Hello world!");

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
}

void loop() {
  digitalWrite (transistor, HIGH);
  delay(1000);
  digitalWrite (transistor, LOW);
  delay(1000);
}
