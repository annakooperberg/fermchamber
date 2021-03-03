#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 4, 5, 6, 7);
const int transistor = 2;

void setup() {
  pinMode (transistor, OUTPUT);
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
