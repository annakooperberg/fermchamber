#include "DHT.h"
#include <EEPROM.h>
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

// Fermentation starting settings
auto target_temp = 80.; // Fahrenheit
auto target_humid = 50.; // percent


// Chamber/Arduino settings
const auto CYCLE_LENGTH = 2000; // milliseconds
const auto MAX_SD_T = 10; // : empirically adjust these
const auto MAX_SD_H = 10;
const auto HEAT_MARGIN = 6;
const auto H_PAD_MARGIN = 3;
const auto HUMID_MARGIN = 3;
const auto MIN_TEMP = 5;
const auto MAX_TEMP = 200; // probably too hot
const auto MIN_HUMID = 0;
const auto MAX_HUMID = 100;
const auto MAX_ADDRESS = 512;
const auto MAX_DISPLAY = 5;


// Misc global variables
bool h_pad = false;
bool fans = false;
bool humid = false;
bool heat = false;

auto new_temp = target_temp;
auto new_humid = target_humid;
bool power = true;
bool set_temp = true;
int display_iters = 0;
bool display_on = false;

auto eeprom_address = 0;

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

  // display
  pinMode(DISP_POWER, OUTPUT);
  pinMode (RESET_PIN, OUTPUT);
  digitalWrite (RESET_PIN, HIGH);
  digitalWrite (RESET_PIN, LOW);
  digitalWrite(RESET_PIN, HIGH);
  lcd.clear();

  // sensors
  dht1.begin(); 
  dht2.begin(); 
  dht3.begin(); 
  dht4.begin(); 

  // read old values from EEPROM
  // address 0 := temp
  // address 1 := humidity
  auto stored_temp = EEPROM.read(0)
  if (stored_temp != 255){
    target_temp = stored_temp;
    new_temp = target_temp;
  }
  auto stored_humid = EEPROM.read(1)
  if (stored_humid != 255){
    target_humid = stored_humid;
    new_humid = target_humid;
  }

  // LEDs
  pinMode(LED_ON, OUTPUT);
  pinMode(LED_OFF, OUTPUT);
  pinMode(LED_HEAT, OUTPUT);
  pinMode(LED_H_PAD, OUTPUT);
  pinMode(LED_HUMID, OUTPUT);
  pinMode(LED_FANS, OUTPUT);

  digitalWrite(LED_ON, HIGH);
}

void loop() {
  // Pause betwee loops
  delay(CYCLE_LENGTH);

  // Check power button
  if (digitalRead(BPOWER) == HIGH){
    // If turned off
    if (power){
      digitalWrite(H_PAD, LOW)
      digitalWrite(FANS, LOW)
      digitalWrite(HUMID, LOW)
      digitalWrite(HEAT, LOW)
      display_text("Powering off...", "");
      digitalWrite(LED_ON, LOW);
      digitalWrite(LED_OFF, HIGH);
    } else{
      display_text("Powering on...", "");
      digitalWrite(LED_ON, HIGH);
      digitalWrite(LED_OFF, LOW);
    }
    power = !power;
  }
  
  if (!power){
    continue;
  }
  // Read temp + humidity sensors
  const float t1 = dht1.readTemperature(true); 
  const float t2 = dht2.readTemperature(true); 
  const float t3 = dht3.readTemperature(true); 
  const float t4 = dht4.readTemperature(true); 
  const float h1 = dht1.readHumidity();
  const float h2 = dht2.readHumidity();
  const float h3 = dht3.readHumidity();
  const float h4 = dht4.readHumidity();
  
  const int avg_t = (int) (t1 + t2 + t3 + t4)/4;
  const int avg_h = (int) (h1 + h2 + h3 + h4)/4;
  
  const int t_sd = (int) pow((pow(t1 - avg_t, 2) + pow(t2 - avg_t, 2) +
  pow(t3 - avg_t, 2) + pow(t4 - avg_t, 2))/4, 0.5);
  const int h_sd = (int) pow((pow(h1 - avg_h, 2) + pow(h2 - avg_h, 2) +
  pow(h3 - avg_h, 2) + pow(h4 - avg_h, 2))/4, 0.5);

  // Move between setting temperature and humidity
  if (digitalRead(BLEFT) == HIGH){
    set_temp = true;
    display_text(cat("Curr temp: ", avg_t), cat("Target temp: ", target_temp));
  }
  
  if (digitalRead(BRIGHT) == HIGH){
    set_temp = false;
    display_text(cat("Curr humid: ", avg_h), cat("Target humid: ", target_humid));
  }

  // Increase temp/humidity
  if (digitalRead(BUP) == HIGH){
    if (set_temp && new_temp < MAX_TEMP){
      new_temp++;
      display_text(cat("New temp: ", new_temp), cat("Target temp: ", target_temp));
    } else if (new_humid < MAX_HUMID){
      new_humid++;
      display_text(cat("New humid: ", new_humid), cat("Target humid: ", target_humid));
    }
  }

  // Decrease temp/humidity
  if (digitalRead(BDOWN) == HIGH){
    if (set_temp && new_temp > MIN_TEMP){
      new_temp--;
      display_text(cat("New temp: ", new_temp), cat("Target temp: ", target_temp));
    } else if(new_humid > MIN_HUMID) {
      new_humid--;
      display_text(cat("New humid: ", new_humid), cat("Target humid: ", target_humid));
    }
  }

  // Set target to be new value
  if (digitalRead(BENTER) == HIGH){
    if (set_temp){
      target_temp = new_temp;
      EEPROM.update(0, target_temp);
      display_text(cat("Set temp:", target_temp), "");
    } else {
      target_humid = new_humid;
      EEPROM.update(1, target_humid);
      display_text(cat("Set humid:", target_humid), "");
    }
  }

  // Determine if heaters/humidifiers should turn on
  bool new_h_pad = get_state(h_pad, TARGET_TEMP, H_PAD_MARGIN);
  bool new_humid = get_state(humid, TARGET_HUMID, HUMID_MARGIN);
  bool new_heat = get_state(heat, TARGET_HEAT, HEAT_MARGIN);

  // Determine if fans should turn on
  bool new_fans = new_heat || new_humid;
  if ((t_sd > MAX_SD_T) || (h_sd > MAX_SD_H)){
    new_fans = true;
  }

  // Turn on/off elements
  h_pad = set_pin(new_h_pad, h_pad, H_PAD, LED_H_PAD);
  fans = set_pin(new_fans, fans, FANS, LED_FANS);
  humid = set_pin(new_humid, humid, HUMID, LED_HUMID);
  heat = set_pin(new_heat, heat, HEAT, LED_HEAT);

  // Record current state
  record_state(avg_t, avg_h);

  // Reset display if it hasn't changed
  reset_display(set_temp, avg_t, avg_h, target_temp, target_humid);
}

// HELPER FUNCTIONS

// Set a pin to a (digital) setting. Only writes to the pin if setting has changed
bool set_pin(bool new_set, bool old_set, int pin, int led_pin){
  if (new_set != old_set){
    if (new_set){
      digitalWrite(pin, HIGH);
      digitalWrite(led_pin, HIGH);      
    } else {
      digitalWrite(pin, LOW);
      digitalWrite(led_pin, LOW);
    }
  }
  return new_set;
}

// Calculate if current temp/humidity is outside of the margin
bool get_state(float current, float target, float margin){
  if ((target - current) >= margin){
    return true;
  }
  return false;
}

// Store current temperature and humidity each iteration
void record_state(float temp, float humid){
  int rounded_temp = (int) temp;
  int rounded_humid = (int) humid;
  EEPROM.update(eeprom_address, rounded_temp);
  incr_address();
  EEPROM.update(eeprom_address, rounded_humid);
  incr_address();
}

// Increment eeprom address
int incr_address(){
  eeprom_address++;
  eeprom_address %= MAX_ADDRESS;
  // Skip addresses 0, 1
  if (eeprom_address < 2){
    eeprom_address = 2;
  }
}

// Display text
void display_text(String row1, int row2){
  if (display_on){
    lcd.clear();
  }
  lcd.print(row1);
  lcd.setCursor(0, 1);
  lcd.print(row2);
  digitalWrite(DISP_POWER, HIGH);
  display_iters = 0;
  display_on = true;
}

// Clear the display if it hasn't changed for MAX_DISPLAY iterations
void reset_display(bool temp, int avg_t, int avg_h, int target_temp, int target_humid){
  if (display_on && display_iters >= MAX_DISPLAY){
    digitalWrite(DISP_POWER, LOW);
    lcd.clear();
    if (temp){
      lcd.print(cat("Curr temp: ", avg_t));
      lcd.setCursor(0, 1);
      lcd.print(cat("Target temp: ", target_temp));
    } else {
      lcd.print(cat("Curr humid: ", avg_h));
      lcd.setCursor(0, 1);
      lcd.print(cat("Target humid: ", target_humid));
    }
    display_iters = 0;
    display_on = false;
  }
}

// Concat string and int
String cat(String str, int i){
    return str + i;
}
