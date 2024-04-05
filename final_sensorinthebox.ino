#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <TridentTD_LineNotify.h>
#include "pitches.h"

//define for Line
#define SSID        "true_home2G_C9C"  // ชื่อไวไฟ
#define PASSWORD    "B4509C9C"  // รหัสไวไฟ
#define LINE_TOKEN  "zEsaNdL79AlPDZZ3yFo6Ry6HO3k3Ucu1fZ3uBDps0Fi"  // token line

//initial LED monitor
LiquidCrystal_I2C lcd(0x27,16,2);

//initial DHT22 sensor
#define DHTPIN 14 //(D5)
#define DHTTYPE DHT22

DHT dht(DHTPIN,DHTTYPE);
/*
DHT dht;
int dhtport = 14;
*/
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
  NOTE_AS4,
   
  NOTE_D5, NOTE_AS4,
  NOTE_D5, NOTE_AS4,
  NOTE_DS5, NOTE_D5,
  NOTE_CS5, NOTE_A4,
  NOTE_AS4, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_D4,
  NOTE_D5, 
  REST, NOTE_AS4,  
  
  NOTE_D5, NOTE_AS4,
  NOTE_D5, NOTE_AS4,
  NOTE_F5, NOTE_E5,
  NOTE_DS5, NOTE_B4,
  NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_AS4,
  NOTE_G4
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
  4,
   
  2, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1, 
  4, 4,  
  
  2, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1
};


//line notify
bool noti = false;

//declare function
int isCold(float tem);
void fan_console(bool work);
void send_noti(bool notify);
void make_noise(bool work);

void setup() {

  //Serial monitor
  Serial.begin(115200);
  Serial.println("Hello");

  //LCD monitor
  lcd.begin();
  lcd.setCursor(0,0);
  lcd.print("Hello LCD");
  lcd.setCursor(2,1);
  lcd.print("- connected");
  lcd.clear();

  //connect to DHT22 sensor
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
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP()); 

  //Line
  Serial.println(LINE.getVersion());

  LINE.setToken(LINE_TOKEN);
   /*
  LINE.notifySticker("Setup Hello",6359,11069861);
  */

  //attach interrupt
  
}

void loop() {
/*
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();  
  float farenheit = dht.toFahrenheit(temperature);
*/
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();  
  float farenheit = dht.readTemperature(true);

  if(isnan(humidity) || isnan(temperature) || isnan(farenheit))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(1000);
    return;
  }
  else
  {
    lcd.noBlink();

    //output to serial
    //Serial.print(dht.getStatusString());
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
    //delay(dht.getMinimumSamplingPeriod());
    delay(1000);

    int temp_now = isCold(temperature);

    if(temp_now == 1)
    {
      //too cold
      lcd.clear();
      //line notify
      //send_noti(noti,"หนาวมาก หนังสือกำลังจะถูกแช่",2);   
      fan_console(false);
      //make_noise(true);
    }
    else if(temp_now == 2)
    {
      //normal temp

      //output to LCD   
      
      noti = false;
      fan_console(false); 
      //make_noise(false);   
    }
    else if(temp_now == 3)
    {
      //too hot
      lcd.clear();
      //line notify
      //send_noti(noti,"อากาศร้อน หนังสือกำลังจะละลาย",1);
      fan_console(true);
      //make_noise(true);
    }
  }

  


}

int isCold(float tem)
{
  //temperater in Celcius
  if(tem < 20.0)
  {
    //lcd output
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Warning!!");    
    lcd.setCursor(0, 1);
    lcd.print("Too cold");    

    /*lcd.blink();    
    delay(3000);*/
    lcd.clear();

    //sound module
    
    //abnormal
    return 1;
  }
  else if (tem > 26.0)
  {
    //lcd output
    lcd.setCursor(0, 0);
    lcd.print("Warning!!");    
    lcd.setCursor(0, 1);
    lcd.print("Too hot");

    lcd.blink();
    delay(500);
    lcd.noBlink();
    delay(500);

    //sound module

    //abnormal
    return 3;
  }
  else
  {
    //normal
    lcd.noBlink();
    return 2;
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
    digitalWrite(motorR, LOW);
  }
}

void send_noti(bool notify, String mes,int hoc)
{
  if(!notify & hoc == 1)
  {
    //hot
    LINE.notifySticker(mes,11539,52114142);
    noti = true;
  }
  else if(!notify & hoc ==2)
  {
    //cold
    LINE.notifySticker(mes,789,10894);
    noti = true;    
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