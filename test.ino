#include "DHT.h"
#include <LiquidCrystal.h>


#define DHT1 4
#define DHT2 5
#define DHT3 6
#define DHT4 7
#define H_PAD 43
#define FANS 45
#define HUMID 44
#define HEAT 42
#define BUP 32
#define BDOWN 33
#define BLEFT 30
#define BRIGHT 31
#define BPOWER 35
#define BENTER 34
#define LED_ON 36
#define LED_OFF 37
#define LED_HEAT 38
#define LED_H_PAD 39
#define LED_HUMID 40
#define LED_FANS 41
#define DISP_RS 48
#define DISP_E 49
#define DISP_DAT_4 50
#define DISP_DAT_5 51
#define DISP_DAT_6 52
#define DISP_DAT_7 53
#define DISP_POWER 8
#define RESET_PIN 9

#define DHTTYPE DHT22

// Temp + humid detectors
DHT dht1(DHT1, DHTTYPE);
DHT dht2(DHT2, DHTTYPE);
DHT dht3(DHT3, DHTTYPE);
DHT dht4(DHT4, DHTTYPE);

// Display object
LiquidCrystal lcd(DISP_RS, DISP_E, DISP_DAT_4, DISP_DAT_5, DISP_DAT_6, DISP_DAT_7);

void setup() {
  Serial.begin(9600);
  
  // outputs
  pinMode(H_PAD, OUTPUT);
  pinMode(FANS, OUTPUT);
  pinMode(HUMID, OUTPUT);
  pinMode(HEAT, OUTPUT);
  
  // buttons
  pinMode(BUP, INPUT);
  pinMode(BDOWN, INPUT);
  pinMode(BLEFT, INPUT);
  pinMode(BRIGHT, INPUT);
  pinMode(BPOWER, INPUT);
  pinMode(BENTER, INPUT);

  // enable pull up resistor
  digitalWrite(BUP, HIGH);
  digitalWrite(BDOWN, HIGH);
  digitalWrite(BLEFT, HIGH);
  digitalWrite(BRIGHT, HIGH);
  digitalWrite(BPOWER, HIGH);
  digitalWrite(BENTER, HIGH);
  
  // display
  pinMode(DISP_POWER, OUTPUT);
  pinMode (RESET_PIN, OUTPUT);

  // disconnect screen for no reason
  digitalWrite (RESET_PIN, HIGH);
  digitalWrite (RESET_PIN, LOW);
  digitalWrite(RESET_PIN, HIGH);

  lcd.clear();

  // sensors
  dht1.begin(); 
  dht2.begin(); 
  dht3.begin(); 
  dht4.begin(); 

  // LEDs
  pinMode(LED_ON, OUTPUT);
  pinMode(LED_OFF, OUTPUT);
  pinMode(LED_HEAT, OUTPUT);
  pinMode(LED_H_PAD, OUTPUT);
  pinMode(LED_HUMID, OUTPUT);
  pinMode(LED_FANS, OUTPUT);
}

void wait_until_press(int pin){
    val = digitalRead(pin);
    while(val == HIGH){
        delay(1);
        val = digitalRead(pin);
    }
}

void loop(){
  digitalWrite(LED_ON, HIGH);
  digitalWrite(LED_OFF, HIGH);
  digitalWrite(LED_H_PAD, HIGH);
  digitalWrite(LED_HEAT, HIGH);
  digitalWrite(LED_HUMID, HIGH);
  digitalWrite(LED_FANS, HIGH);
  
  lcd.clear();
  lcd.print(" Press left");
  wait_until_press(BLEFT);
  
  lcd.clear();
  lcd.print(" Press right");
  wait_until_press(BRIGHT);

  lcd.clear();
  lcd.print(" Press up");
  wait_until_press(BUP);

  lcd.clear();
  lcd.print(" Press down");
  wait_until_press(BDOWN);

  lcd.clear();
  lcd.print(" Press enter");
  wait_until_press(BENTER);

  lcd.clear();
  lcd.print(" Press power");
  wait_until_press(BPOWER);
  
  lcd.print(" On");
  lcd.clear();

  digitalWrite(HEAT, HIGH);
  digitalWrite(H_PAD, HIGH);
  digitalWrite(HUMID, HIGH);
  digitalWrite(FANS, HIGH);

  delay(3000);

  digitalWrite(HEAT, LOW);
  digitalWrite(H_PAD, LOW);
  digitalWrite(HUMID, LOW);
  digitalWrite(FANS, LOW);

  lcd.clear();
  lcd.print("Target T: 78.0");
}