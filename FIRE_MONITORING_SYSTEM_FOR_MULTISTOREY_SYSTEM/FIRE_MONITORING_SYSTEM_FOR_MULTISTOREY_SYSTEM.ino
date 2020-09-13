#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include "ThingSpeak.h"

#include "DHT.h"
#define DHTPIN D1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

char auth[] = "pAI35tdIaFRjnUOUmj7N3GxiCp3eVSBE";
char ssid[] = "Nammabantwal";
char pass[] = "1234@shivaranjan";

unsigned long myChannelNumber = 1125706;
const char * myWriteAPIKey = "RFJMIZZ86N8QWPJE";

WiFiClient  client;
BlynkTimer timer;

WidgetLCD lcd(V0);

int buzzer=D6,relay=D7,rval,fire=D2, fireval,ir=D3, irval, first_floor_flag=0,second_floor_flag=0,temperature,humidity;

void setup() {
  
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  Serial.println("FIRE MONITORING SYSTEM FOR MULTISTOREY BUILDINGS");
  
  pinMode(buzzer,OUTPUT);
  pinMode(relay,OUTPUT);
  pinMode(fire,INPUT);
  pinMode(ir,INPUT);
  
  Serial.println(F("DHTxx test!"));
  dht.begin();
  
  timer.setInterval(20000,upload_to_Thingspeak);
}
void first_floor(){
  
  fireval=digitalRead(fire);
  Serial.println("fire value is:"+String(fireval));
  
  if(fireval==0){
    
    digitalWrite(relay,LOW);
    delay(4000);
    Serial.println("FIRE IS DETECTED ON THE FIRST FLOOR");
    Blynk.virtualWrite(V1, "FIRE IS DETECTED ON THE FIRST FLOOR");
    first_floor_flag=1;
    
  }else{
    
    digitalWrite(relay,HIGH);
    delay(4000);
    first_floor_flag=0;
    Blynk.virtualWrite(V1, "FIRE IS NOT DETECTED ON THE FIRST FLOOR");
    
  }   
  //delay(4000);
}
void second_floor(){
  
  irval=digitalRead(ir);
  Serial.println("ir value is:"+String(irval));
  
  if(irval==0){
    
    digitalWrite(buzzer,LOW);
    Serial.println("FIRE IS DETECTED ON THE SECOND FLOOR");
    Blynk.virtualWrite(V2, "FIRE IS DETECTED ON THE SECOND FLOOR");
    second_floor_flag=1;
    delay(4000);
    
  }else{
    
    digitalWrite(buzzer,HIGH);
    second_floor_flag=0;
    Blynk.virtualWrite(V2, "FIRE IS NOT DETECTED ON THE SECOND FLOOR");
    delay(4000);
    
  }
  if(first_floor_flag==1 && second_floor_flag==1){
    
    digitalWrite(buzzer,LOW);
    digitalWrite(relay,LOW);
    Serial.println("FIRE IS DETECTED ON BOTH THE FLOOR");
    Blynk.virtualWrite(V3, "FIRE IS DETECTED ON BOTH THE FLOOR");
    Blynk.notify("FIRE IS DETECTED ON BOTH THE FLOOR");
    delay(4000);
    
  }else{
    
    digitalWrite(buzzer,HIGH);
    digitalWrite(relay,HIGH);
    Serial.println("FIRE IS NOT DETECTED ON BOTH THE FLOOR");
    Blynk.virtualWrite(V3, "FIRE IS NOT DETECTED ON BOTH THE FLOOR");
    delay(4000);
    
  }
}
void weather_update(){
  
   delay(2000);
   humidity = dht.readHumidity();
   temperature = dht.readTemperature();
   
   lcd.print(0,0, "Temperature:");
   lcd.print(12,0,temperature);
   lcd.print(14,0,"C");
   lcd.print(0,1, "Humidity:");
   lcd.print(10,1,humidity);
   //delay(2000);
}
  
void upload_to_Thingspeak(){
  
  ThingSpeak.setField(1,temperature);
  ThingSpeak.setField(2,humidity);
  
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
}

void loop() {
  
  Blynk.run();
  timer.run();
  weather_update();
  first_floor();
  second_floor();

}
