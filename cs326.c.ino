#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

LiquidCrystal_I2C lcd(0x27,16,2);
//LiquidCrystal_I2C lcd(0x3F,16,2);

DHT dht;
int buzzer = D4;
int dhtport = D5;
int motorL = D6;
int motorR = D7;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello");
  //Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)");

  dht.setup(D5);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello from LCD");
  lcd.setCursor(2,1);
  lcd.print("- Buzzer");

  pinMode(buzzer, OUTPUT);
  pinMode(motorL, OUTPUT);
  pinMode(motorR, OUTPUT);
  
}

void loop() {
  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  Serial.print(dht.getStatusString());
  Serial.print("\tHumidity : ");
  Serial.print(humidity,1);
  Serial.print("\t\tTemp C : ");
  Serial.print(temperature,1);
  Serial.println(dht.toFahrenheit(temperature), 1);  

  lcd.setCursor(0, 0);
  lcd.print("hum:     ");
  lcd.setCursor(4, 0);
  lcd.print(humidity);
  lcd.setCursor(9, 0);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Tem:     ");
  lcd.setCursor(4, 1);
  lcd.print(temperature);
  lcd.setCursor(9, 1);
  lcd.print("C");  

  //sound  
  digitalWrite(buzzer, LOW);
  delay(200);
  digitalWrite(buzzer, HIGH);
  delay(3000);
  
  //fan
  digitalWrite(motorL, LOW);
  digitalWrite(motorR, HIGH);
  delay(200);
  digitalWrite(motorL, HIGH);
  digitalWrite(motorR, LOW);
  delay(200);
  //delay(2000);
  
}
