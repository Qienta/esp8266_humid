#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <TridentTD_LineNotify.h>
#include "pitches.h"

//define for Line
#define SSID        "BinkEmb"  // ชื่อไวไฟ
#define PASSWORD    "3354776415"  // รหัสไวไฟ
#define LINE_TOKEN  "33VLT5MnHmkqi8QfbnRHicNptxx2On0ETICOrAmkomN"  // token line

//initial DHT22 sensor
#define DHTPIN 14 //(D5)
#define DHTTYPE DHT22
float humidity;
float temperature;

DHT dht(DHTPIN,DHTTYPE);

//line notify
bool noti = false;
bool noti2 = false;

//initial LED monitor
LiquidCrystal_I2C lcd(0x27,16,2);

//initial fan module
int motorL = 12; //(D6)
int motorR = 13; //(D7)

//initial sound module
int buzzer = 2; //(D4)
int melody[] = { 
	REST, NOTE_D4,
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_D5,
  NOTE_C5, 
  NOTE_A4,
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_F4, NOTE_GS4,
  NOTE_D4, 
  NOTE_D4,
  
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_D5,
  NOTE_F5, NOTE_E5,
  NOTE_DS5, NOTE_B4,
  NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_B4,
  NOTE_G4,
  NOTE_AS4
  
};

int durations[] = { 
	2, 4,
  4, 8, 4,
  2, 4,
  2, 
  2,
  4, 8, 4,
  2, 4,
  1, 
  4,
  
  4, 8, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1,
  4  
  
};

//declare function
void checkTemp();
void checkHum();
void send_stick(bool notify,String mes,int hoc);
void make_noise(bool work);
void fan_console(bool work);
void lcd_display();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello");

  //LCD monitor
  lcd.begin();
  lcd.setCursor(0,0);
  lcd.print("Hello LCD");
  lcd.setCursor(2,1);
  lcd.print("- connected");
  lcd.clear();

  //dht.setup(dhtport);
  dht.begin();

  //connect sound module
  pinMode(buzzer, OUTPUT);
  //connect fan module
  pinMode(motorL, OUTPUT);
  pinMode(motorR, OUTPUT);

  //connect to WiFi
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP()); 

  //connect LINE
  Serial.println(LINE.getVersion());
  LINE.setToken(LINE_TOKEN);

  //interrupt
  //attachInterrupt(digitalPinToInterrupt(DHTPIN),checkTemp,CHANGE);
  

}

void loop() {

  lcd_display();
  checkTemp();
  checkHum();
  

}

void lcd_display()
{
  float farenheit = dht.readTemperature(true);
  Serial.print("\tHumidity : ");
  Serial.print(humidity,1);
  Serial.print("\t\tTemp C : ");
  Serial.print(temperature,1);
  Serial.println(farenheit, 1);    

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

  delay(2000);
}

void checkTemp()
{
  temperature = dht.readTemperature();

  if(temperature > 26)
  {
    Serial.println("Temperature too HIGH");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("too HOT!!");

    //line notify
    send_stick(noti,"อากาศร้อน หนังสือกำลังจะละลาย",1);
    noti = true;

    //use fan
    fan_console(true);

    //sound activate
    make_noise(true);
  }
  else if(temperature < 20)
  {
    Serial.println("Temperature too LOW");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("too COLD!!");

    //line notify
    send_stick(noti,"หนาวเกิน หนังสือจะแข็งแหลว",2);
    noti = true;

    //not use fan
    fan_console(false);

    //sound activate
    make_noise(true);
  }
  else
  {
    noti = false;
    fan_console(false); 
    make_noise(false);
  }

}

void checkHum()
{
  humidity = dht.readHumidity();

  if(humidity > 60)
  {
    Serial.println("humidity too HIGH");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("too WET!!");

    //line notify
    send_stick(noti2,"ชุ่มฉ่ำเกิน จะจมน้ำแล้ว",4);
    noti2 = true;
  }
  else if(humidity < 40)
  {
    Serial.println("humidity too LOW");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("too DRY!!");

    //line notify
    send_stick(noti2,"แห้งเกิน จะกรอบแล้ว",3);
    noti2 = true;
  }
  else
  {
    noti2 = false;
  }
}

void send_stick(bool notify,String mes,int hoc)
{
  if(!notify)
  {
    if(hoc == 1)
    {
      //hot
      LINE.notifySticker(mes,11539,52114142);
    }
    else if(hoc == 2)
    {
      //cold
      LINE.notifySticker(mes,789,10894);
    }
    else if(hoc == 3)
    {
      //dry
      LINE.notifySticker(mes,466,2006);
    }
    else if(hoc == 4)
    {
      //soke
      LINE.notifySticker(mes,6359,11069854);      
    }
    
  }
}

void fan_console(bool work)
{
  if(work)
  {
    digitalWrite(motorL, LOW);
    digitalWrite(motorR, HIGH);
    delay(200);
    digitalWrite(motorL, HIGH);
    digitalWrite(motorR, LOW);
    delay(200);
  }
  else
  {
    digitalWrite(motorR, LOW);
    digitalWrite(motorL, LOW);
  }
}

void make_noise(bool work)
{
  if(work)
  {
    int size = sizeof(durations) / sizeof(int); 
    for (int note = 0; note < size; note++) 
    {
      int duration = 1000 / durations[note]; 
      tone(buzzer, melody[note], duration);

      int pauseBetweenNotes = duration * 1.30;
      delay(pauseBetweenNotes);
      noTone(buzzer);
    }
  }
  else
  {
    noTone(buzzer);    
  }
}
