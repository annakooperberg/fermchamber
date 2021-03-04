// bid heater => fans
// humidifier => fans
// high variance => fans

#include "DHT.h"
#include <LiquidCrystal.h>

#define DHT1 2
#define DHT2 3
#define DHT3 4
#define DHT4 5
#define H_PAD 6
#define FANS 7
#define HUMID 8
#define HEAT 9
#define BUP 10
#define BDOWN 10
#define BLEFT 11
#define BRIGHT 12
#define BPOWER 12
#define BENTER 12

#define DHTTYPE DHT22

DHT dht1(DHT1, DHTTYPE);
DHT dht2(DHT2, DHTTYPE);
DHT dht3(DHT3, DHTTYPE);
DHT dht4(DHT4, DHTTYPE);

// Fermentation starting settings
auto target_temp = 70.; // C
auto target_humid = 50.; // percent


// Chamber settings
const auto CYCLE_LENGTH = 2000; // milliseconds
const auto MAX_SD_T = 10;
const auto MAX_SD_H = 10;
const auto HEAT_MARGIN = 6;
const auto H_PAD_MARGIN = 3;
const auto HUMID_MARGIN = 3;

// Misc
bool h_pad = false;
bool fans = false;
bool humid = false;
bool heat = false;

auto new_temp = target_temp;
auto new_humid = target_humid;
bool power = true;
bool set_temp = true;

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
  
  // sensors
  dht1.begin(); 
  dht2.begin(); 
  dht3.begin(); 
  dht4.begin(); 
}

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

bool get_state(float current, float target, float margin){
  if ((target - current) >= margin){
    return true;
  }
  return false;
}

void loop() {
  delay(CYCLE_LENGTH);
  
  if (digitalRead(BPOWER) == HIGH){
    if (power){
      digitalWrite(H_PAD, LOW)
      digitalWrite(FANS, LOW)
      digitalWrite(HUMID, LOW)
      digitalWrite(HEAT, LOW)
    }
    power = !power;
  }
  
  if (!power){
    continue;
  }
  
  if (digitalRead(BLEFT) == HIGH && !set_temp){
    set_temp = true;
  }
  
  if (digitalRead(BRIGHT) == HIGH && set_temp){
    set_temp = false;
  }
  
  if (digitalRead(BUP) == HIGH){
    if (set_temp){
      new_temp++;
    } else {
      new_humid++;
    }
  }
  
  if (digitalRead(BDOWN) == HIGH){
    if (set_temp){
      new_temp--;
    } else {
      new_humid--;
    }
  }
  
  if (digitalRead(BENTER) == HIGH){
    if (set_temp){
      target_temp = new_temp;
    } else {
      target_humid = new_humid;
    }
  }
  
  const float t1 = dht1.readTemperature(); 
  const float t2 = dht2.readTemperature(); 
  const float t3 = dht3.readTemperature(); 
  const float t4 = dht4.readTemperature(); 
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
  
  bool new_h_pad = get_state(h_pad, TARGET_TEMP, H_PAD_MARGIN);
  bool new_humid = get_state(humid, TARGET_HUMID, HUMID_MARGIN);
  bool new_heat = get_state(heat, TARGET_HEAT, HEAT_MARGIN);
  
  bool new_fans = new_heat || new_humid
  if ((t_sd > MAX_SD_T) || (h_sd > MAX_SD_H)){
    new_fans = true;
  }
  
  h_pad = set_pin(new_h_pad, h_pad, H_PAD);
  fans = set_pin(new_fans, fans, FANS);
  humid = set_pin(new_humid, humid, HUMID);
  heat = set_pin(new_heat, heat, HEAT);
}
