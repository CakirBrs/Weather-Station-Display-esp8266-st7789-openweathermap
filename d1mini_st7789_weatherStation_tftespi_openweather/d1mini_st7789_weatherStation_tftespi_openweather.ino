#include <SPI.h>
#include <TFT_eSPI.h>
#include "gothicFont.h"
#include "gothicFont24.h"
#include "gothicFont12.h"
#include "gothicFont16.h"
#include "i01d.h"
#include "i02d.h"
#include "i03d.h"
#include "i04n2x.h"
#include <stdlib.h>
#include <string.h>
#include "thermoM25x25.h"
#include "humid25x25.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
TFT_eSprite sprite2 = TFT_eSprite(&tft);
TFT_eSprite sprite3 = TFT_eSprite(&tft);
TFT_eSprite barSprite = TFT_eSprite(&tft);

const char* ssid = "Wifi ID"; 
const char* password = "Wifi Password"; 
String openWeatherMapApiKey = "API Key";
String city = "City";
String countryCode = "Country";
String continent="Continenet"; //Europe,America...
String timeZoneCity= "Time Zone City"; //London,Berlin,Rome,Istanbul,Helsinki...

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
String jsonBuffer;

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  sprite.createSprite(240,100);
  sprite.fillSprite(TFT_BLACK);
  sprite.drawString("Connecting",0,0);
  sprite.pushSprite(0,0,TFT_BLACK);
  sprite.deleteSprite();

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  tft.fillScreen(TFT_BLACK); //clearing display
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  printTemplate();

}


String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric&lang=tr";

String tempT="";
String tempTold="1";

String tempH="";
String tempHold="1";

String tmpp;
String tmppold="1";

String tmpp2;
String tmpp2old="1";

int idW=0;
int idWold=10;

int day_of_week=0;
int day_of_weekold=10;

String description="";
String descriptionold="1";

String feelsLike="";
String feelsLikeold="1";

String tempMax="";
String tempMaxold="1";

String tempMin="";
String tempMinold="1";

String windSpeed="";
String windSpeedold="1";



void loop() {
  if(WiFi.status()== WL_CONNECTED){

      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
      jsonBuffer = "";
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      tempT=JSON.stringify(myObject["main"]["temp"]);
      Serial.println(tempT);
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      tempH=JSON.stringify(myObject["main"]["humidity"]);
      Serial.println(tempH);
      
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
      Serial.print("weather: ");
      Serial.println(myObject["weather"]);
      JSONVar myObjectweather =myObject["weather"][0];
      Serial.println(myObjectweather);
      Serial.print("icon: ");
      Serial.println(myObjectweather["icon"]);
      idW=(JSON.stringify(myObjectweather["id"])).toInt();


      description=(JSON.stringify(myObjectweather["description"]));
      feelsLike=(JSON.stringify(myObject["main"]["feels_like"]));
      tempMax=(JSON.stringify(myObject["main"]["temp_max"]));
      tempMin=(JSON.stringify(myObject["main"]["temp_min"]));
      windSpeed=(JSON.stringify(myObject["wind"]["speed"]));

      String serverPath2 = "http://worldtimeapi.org/api/timezone/"+continent+"/"+timeZoneCity;
      jsonBuffer = httpGETRequest(serverPath2.c_str()); 
      Serial.println(jsonBuffer);
      myObject = JSON.parse(jsonBuffer);
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("datetime: ");
      Serial.println(myObject["datetime"]);
      String timee = JSON.stringify(myObject["datetime"]);
      day_of_week = (JSON.stringify(myObject["day_of_week"])).toInt();
      
      Serial.println(myObject["datetime"]);
      Serial.println(timee);
      tmpp=timee.substring(12,17);
      tmpp2=timee.substring(9,11)+"."+timee.substring(6,8)+"."+timee.substring(1,5);
    }
    else {
      Serial.println("WiFi Disconnected");
    }


    if ((!description.equals(descriptionold))||(idW!=idWold)){
      weatherImage(135 , 0,idW,description);
      idWold=idW;
      descriptionold=description;
    }
    if (!tempMax.equals(tempMaxold))
    {
      tempMaxold=tempMax;
      bigTextTest16short(tempMax, 100, 40);
    }

    if (!tempMin.equals(tempMinold))
    {
      bigTextTest16short(tempMin, 100, 55);
      tempMinold=tempMin;
    }
    if (!tmpp.equals(tmppold))
    {
      tmppold=tmpp;
      bigTextTest48(tmpp, 10, 75);
    }

    if (!tmpp2.equals(tmpp2old)){
      bigTextTest24(tmpp2, 10, 120);
      tmpp2old=tmpp2;
    }
      
    if (!tempT.equals(tempTold))
    {
      tempTold=tempT;
      barCreate(40,180,(tempT.toInt()*100/40));
      bigTextTest16(tempT+"°C", 145, 180);
    }
    if (!tempH.equals(tempHold))
    {
      tempHold=tempH;
      barCreate(40,210,tempHold.toInt());
      bigTextTest16(tempH+"%", 145, 210); 
    }
    if (day_of_week!=day_of_weekold){ //Days of week texts are in Turkish Language.

      day_of_weekold=day_of_week;
      String dow;
      if(day_of_week==1){
        dow="Pazartesi";
      }
      else if(day_of_week==2){
        dow="Sali";
      }
      else if(day_of_week==3){
        dow="Carsamba";
      }
      else if(day_of_week==4){
        dow="Persembe";
      }
      else if(day_of_week==5){
        dow="Cuma";
      }
      else if(day_of_week==6){
        dow="Cumartes";
      }
      else if(day_of_week==7){
        dow="Pazar";
      }
      bigTextTest16(dow, 10, 145);
    }
    delay(10000);
}



String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}



void weatherImage(int x, int y,int id,String s){
  sprite.createSprite(100,100);
  sprite2.createSprite(100,100);
  sprite.fillSprite(TFT_BLACK);
  sprite2.fillSprite(TFT_BLACK);
  sprite.setSwapBytes(true);
  if(id==800){//use switch case and add other images
    sprite.pushImage(0,0,100,100,i01d);//i01d
    
  }
  else if(id==801){//burda switch case kullan
    sprite.pushImage(0,0,100,100,i02d);//i02d
    
  }
  else if(id==802){//burda switch case kullan
    sprite.pushImage(0,0,100,100,i03d);//i02d
    
  }
  else if(id==803 || id==804){//burda switch case kullan
    sprite.pushImage(0,0,100,100,i04n2x);//i02d
    
  }

  sprite.pushSprite(x,y,TFT_BLACK); 
  sprite.pushToSprite(&sprite2, 0, 0,TFT_BLACK);
  sprite2.pushSprite(x,y);
  sprite2.deleteSprite();
  sprite.fillSprite(TFT_BLACK);
  sprite.loadFont(gothicFont24);
  sprite.fillRoundRect(  0, 0,  100, 30, 10, TFT_WHITE);
  sprite.setTextColor(TFT_BLACK, TFT_WHITE);
  String hava;
  if(id==800){
    hava="Clear";
  }else{
    hava="Clouds";
  }
  sprite.drawString(hava,10,5);
  sprite.pushSprite(x,y+80);
  sprite.fillSprite(TFT_BLACK);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.unloadFont();
  sprite.loadFont(gothicFont16);
  sprite.drawString(s,10,5);
  sprite.pushSprite(x,y+110);
  sprite.deleteSprite();
}


void tempImage(int x, int y){
  sprite.createSprite(50,50);
  sprite.fillSprite(TFT_BLACK);
  sprite.pushSprite(x,y);
  sprite.setSwapBytes(true);
  sprite.pushImage(0,0,25,25,thermoM25x25);
  sprite.pushSprite(x,y,TFT_BLACK);
  sprite.deleteSprite();
}

void humidImage(int x, int y){
  sprite.createSprite(50,50);
  sprite.fillSprite(TFT_BLACK);
  sprite.pushSprite(x,y);
  sprite.setSwapBytes(true);
  sprite.pushImage(0,0,25,25,humid25x25);
  sprite.pushSprite(x,y,TFT_BLACK);
  sprite.deleteSprite();
}

void barCreate(int x, int y,int perc){
  barSprite.createSprite(100,11);
  barSprite.fillSprite(TFT_BLACK);
  barSprite.fillRoundRect(  0, 0,  100, 10, 5, TFT_WHITE);
  barSprite.fillRoundRect(  3, 1,  ((100-6)*perc)/100, 8, 4, TFT_RED);

  barSprite.pushSprite(x,y);
  barSprite.deleteSprite();
}

void bigTextTest24(String s, int x, int y){
  
  sprite3.createSprite(120,25);
  sprite3.fillSprite(TFT_BLACK);
  sprite3.loadFont(gothicFont24);
  sprite3.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite3.drawString(s,0,0);
  sprite3.pushSprite(x,y); 
  sprite3.unloadFont();
  sprite3.deleteSprite();
}

void bigTextTest48(String s, int x, int y){
  
  sprite.createSprite(120,45);
  sprite.fillSprite(TFT_BLACK);
  sprite.loadFont(gothicFont);
  sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
  sprite.drawString(s,0,0);
  sprite.unloadFont();
  sprite.pushSprite(x,y);
  sprite.deleteSprite();
}

void bigTextTest12(String s, int x, int y){
  
  sprite.createSprite(100,20);
  sprite.fillSprite(TFT_BLACK);
  sprite.loadFont(gothicFont12);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString(s,0,0);
  sprite.unloadFont();
  sprite.pushSprite(x,y);
  sprite.deleteSprite();
}

void bigTextTest16(String s, int x, int y){
  
  sprite.createSprite(140,20);
  sprite.fillSprite(TFT_BLACK);
  sprite.loadFont(gothicFont16);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString(s,0,0);
  sprite.unloadFont();
  sprite.pushSprite(x,y);
  sprite.deleteSprite();
}

void bigTextTest16short(String s, int x, int y){
  
  sprite.createSprite(40,20);
  sprite.fillSprite(TFT_BLACK);
  sprite.loadFont(gothicFont16);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString(s,0,0);
  sprite.unloadFont();
  sprite.pushSprite(x,y);
  sprite.deleteSprite();
}

void printTemplate(){
  bigTextTest24(city, 10, 15);
  bigTextTest16("Max Temp:", 10, 40); 
  bigTextTest16("Min Temp:", 10, 55); 
  tempImage(10, 170); 
  humidImage(10, 200); 
}