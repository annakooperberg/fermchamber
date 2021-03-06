// TODO: LEDs display backlight

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
#define LED_G 36
#define LED_R 37
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

#define DHTTYPE DHT22

// Temp + humid detectors
DHT dht1(DHT1, DHTTYPE);
DHT dht2(DHT2, DHTTYPE);
DHT dht3(DHT3, DHTTYPE);
DHT dht4(DHT4, DHTTYPE);

// Display object TODO: fix inputs
LiquidCrystal lcd(DISP_RS, DISP_E, DISP_DAT_4, DISP_DAT_5, DISP_DAT_6, DISP_DAT_7);

// Fermentation starting settings
auto target_temp = 80.; // Fahrenheit
auto target_humid = 50.; // percent


// Chamber/Arduino settings
const auto CYCLE_LENGTH = 2000; // milliseconds
const auto MAX_SD_T = 10; // TODO: empirically adjust these
const auto MAX_SD_H = 10;
const auto HEAT_MARGIN = 6;
const auto H_PAD_MARGIN = 3;
const auto HUMID_MARGIN = 3;
const auto MIN_TEMP = 5;
const auto MAX_TEMP = 200; // probably too hot
const auto MIN_HUMID = 0;
const auto MAX_HUMID = 100;
const auto MAX_ADDRESS = 512; // TODO: check this
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
  Serial.println("Starting...");
  
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
  lcd.clear();
  pinMode(DISP_POWER, OUTPUT);
  
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
      display_text("Powering off...");
    } else{
      display_text("Powering on...");
    }
    power = !power;
  }
  
  if (!power){
    continue;
  }

  // Move between setting temperature and humidity
  if (digitalRead(BLEFT) == HIGH && !set_temp){
    set_temp = true;
    display_num("New temp:", new_temp);
  }
  
  if (digitalRead(BRIGHT) == HIGH && set_temp){
    set_temp = false;
    display_num("New humid:", new_humid);
    
  }

  // Increase temp/humidity
  if (digitalRead(BUP) == HIGH){
    if (set_temp && new_temp < MAX_TEMP){
      new_temp++;
      display_num("New temp:", new_temp);
    } else if (new_humid < MAX_HUMID){
      new_humid++;
      display_num("New humid:", new_humid);
    }
  }

  // Decrease temp/humidity
  if (digitalRead(BDOWN) == HIGH){
    if (set_temp && new_temp > MIN_TEMP){
      new_temp--;
      display_num("New temp:", new_temp);
    } else if(new_humid > MIN_HUMID) {
      new_humid--;
      display_num("New humid:", new_humid);
    }
  }

  // Set target to be new value
  if (digitalRead(BENTER) == HIGH){
    if (set_temp){
      target_temp = new_temp;
      EEPROM.update(0, target_temp);
      display_num("Set temp:", target_temp);
    } else {
      target_humid = new_humid;
      EEPROM.update(1, target_humid);
      display_num("Set humid:", target_humid);
    }
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
  
  const float avg_t = (t1 + t2 + t3 + t4)/4;
  const float avg_h = (h1 + h2 + h3 + h4)/4;
  
  const float t_sd = pow((pow(t1 - avg_t, 2) + pow(t2 - avg_t, 2) +
  pow(t3 - avg_t, 2) + pow(t4 - avg_t, 2))/4, 0.5);
  const float h_sd = pow((pow(h1 - avg_h, 2) + pow(h2 - avg_h, 2) +
  pow(h3 - avg_h, 2) + pow(h4 - avg_h, 2))/4, 0.5);

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
  h_pad = set_pin(new_h_pad, h_pad, H_PAD);
  fans = set_pin(new_fans, fans, FANS);
  humid = set_pin(new_humid, humid, HUMID);
  heat = set_pin(new_heat, heat, HEAT);

  // Record current state
  record_state(avg_t, avg_h);

  // Reset display if it hasn't changed
  reset_display();
}

// HELPER FUNCTIONS

// Set a pin to a (digital) setting. Only writes to the pin if setting has changed
bool set_pin(bool new_set, bool old_set, int pin){
  if (new_set != old_set){
    if (new_set){
      digitalWrite(pin, HIGH);
    } else {
      digitalWrite(pin, LOW);
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

// Display text and number
void display_num(String text, int num){
  if (display_on){
    lcd.clear();
  }
  lcd.print(text);
  lcd.setCursor(0, 1);
  lcd.print(num);
  display_iters = 0;
  display_on = true;
}

// Display text
void display_text(String text){
  if (display_on){
    lcd.clear();
  }
  lcd.print(text);
  display_iters = 0;
  display_on = true;
}

// Clear the display if it hasn't changed for MAX_DISPLAY iterations
void reset_display(){
  if (display_on && display_iters >= MAX_DISPLAY){
    lcd.clear();
    display_iters = 0;
    display_on = false;
  }
}
