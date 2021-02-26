// bid heater => fans
// humidifier => fans
// high variance => fans

#include "DHT.h"
#include<cmath>

#define DHT1 2
#define DHT2 3
#define DHT3 4
#define DHT4 5
#define H_PAD 6
#define FANS 7
#define HUMID 8
#define HEAT 9

#define DHTTYPE DHT22

DHT dht1(DHT1, DHTTYPE);
DHT dht2(DHT2, DHTTYPE);
DHT dht3(DHT3, DHTTYPE);
DHT dht4(DHT4, DHTTYPE);

// Fermentation settings
const auto TARGET_TEMP = 70; // C
const auto TARGET_HUMID = 50; // percent


// Chamber settings
const auto CYCLE_LENGTH = 2000; // milliseconds
const auto MAX_SD_T = 10;
const auto MAX_SD_H = 10;

// Misc
bool h_pad = false;
bool fans = false;
bool h_pad = false;
bool h_pad = false;



void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
 
  pinMode(H_PAD, OUTPUT);
  pinMode(FANS, OUTPUT);
  pinMode(HUMID, OUTPUT);
  pinMode(HEAT, OUTPUT);
  dht1.begin(); 
  dht2.begin(); 
  dht3.begin(); 
  dht4.begin(); 
}

void loop() {
  delay(CYCLE_LENGTH);
  // h, t = humidity, temp() get from other code
  float t1 = dht1.readTemperature(); 
  float t2 = dht2.readTemperature(); 
  float t3 = dht3.readTemperature(); 
  float t4 = dht4.readTemperature(); 
  float h1 = dht1.readHumidity();
  float h2 = dht2.readHumidity();
  float h3 = dht3.readHumidity();
  float h4 = dht4.readHumidity();

  float avg_t = (t1 + t2 + t3 + t4)/4;
  float avg_h = (h1 + h2 + h3 + h4)/4;
  
  float t_sd = pow((pow(t1 - avg_t, 2) + pow(t2 - avg_t, 2) +
                pow(t3 - avg_t, 2) + pow(t4 - avg_t, 2))/4, 0.5);
  float h_sd = pow((pow(h1 - avg_h, 2) + pow(h2 - avg_h, 2) +
                pow(h3 - avg_h, 2) + pow(h4 - avg_h, 2))/4, 0.5);

  bool fan_new_state = 
                

}
