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
#include "i09d.h"
#include "i10d.h"
#include "i11d.h"
#include "i13d.h"
#include "i50d.h"
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
TFT_eSprite bigTextTest16shorts = TFT_eSprite(&tft);
TFT_eSprite bigTextTest16s = TFT_eSprite(&tft);
TFT_eSprite bigTextTest12s = TFT_eSprite(&tft);
TFT_eSprite descrption = TFT_eSprite(&tft);


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
      bigTextTest16short(tempMax+"°C", 80, 40);
    }

    if (!tempMin.equals(tempMinold))
    {
      bigTextTest16short(tempMin+"°C", 80, 55);
      tempMinold=tempMin;
    }
    if (!tmpp.equals(tmppold))
    {
      tmppold=tmpp;
      bigTextTest48(tmpp+"°C", 10, 75);
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
      
      switch(day_of_week){
        case 0:
          dow="Pazar";
          break;
        case 1:
          dow="Pazartesi";
          break;
        case 2:
          dow="Sali";
          break;
        case 3:
          dow="Carsamba";
          break;
        case 4:
          dow="Persembe";
          break;
        case 5:
          dow="Cuma";
          break;
        case 6:
          dow="Cumartesi";
          break;
        default:
          dow="ERROR!";
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
  
  switch(id){
    case 800:
      sprite.pushImage(0,0,100,100,i01d);//i01d
      break;
    case 801:
      sprite.pushImage(0,0,100,100,i02d);//i02d
      break;
    case 802:
      sprite.pushImage(0,0,100,100,i03d);//i02d
      break;
    case 803:
      sprite.pushImage(0,0,100,100,i04n2x);//i02d
      break;
    case 804:
      sprite.pushImage(0,0,100,100,i04n2x);//i02d
      break;
    case 520:
    case 521:
    case 522:
    case 531:
      sprite.pushImage(0,0,100,100,i09d);//i02d
      break;
    case 500:
    case 501:
    case 502:
    case 503:
    case 504:
      sprite.pushImage(0,0,100,100,i10d);//i02d
      break;
    case 200:
    case 201:
    case 202:
    case 210:
    case 211:
    case 212:
    case 221:
    case 230:
    case 231:
    case 232:
      sprite.pushImage(0,0,100,100,i11d);//i02d
      break;
    case 511:
    case 600:
    case 601:
    case 602:
    case 611:
    case 612:
    case 613:
    case 615:
    case 616:
    case 620:
    case 621:
    case 622:
      sprite.pushImage(0,0,100,100,i13d);//i02d
      break;
    case 700:
    case 711:
    case 721:
    case 731:
    case 741:
    case 751:
    case 761:
    case 762:
    case 771:
    case 781:
      sprite.pushImage(0,0,100,100,i50d);//i02d
      break;
    
  }

  sprite.pushSprite(x,y); //,TFT_BLACK
  sprite.pushToSprite(&sprite2, 0, 0);//,TFT_BLACK
  sprite2.pushSprite(x,y);
  sprite2.deleteSprite();
  sprite.deleteSprite();

  descrption.createSprite(100,50);
  descrption.fillSprite(TFT_BLACK);
  descrption.loadFont(gothicFont24);
  descrption.fillRoundRect(  0, 0,  100, 30, 10, TFT_WHITE);
  descrption.setTextColor(TFT_BLACK, TFT_WHITE);
  String hava;
  if(id==800){
    hava="Clear";
  }else{
    hava="Clouds";
  }
  descrption.setTextDatum(MC_DATUM);
  descrption.drawString(hava,50,15);
  descrption.pushSprite(x,y+80);
  descrption.fillSprite(TFT_BLACK);
  descrption.setTextColor(TFT_WHITE, TFT_BLACK);
  descrption.unloadFont();
  descrption.deleteSprite();
  descrption.createSprite(100,50);
  descrption.loadFont(gothicFont16);
  descrption.setTextDatum(MC_DATUM);
  descrption.drawString(s,50,15);
  descrption.pushSprite(x,y+110);
  descrption.deleteSprite();
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
  
  bigTextTest12s.createSprite(100,15);
  bigTextTest12s.fillSprite(TFT_BLACK);
  bigTextTest12s.loadFont(gothicFont12);
  bigTextTest12s.setTextColor(TFT_WHITE, TFT_BLACK);
  bigTextTest12s.drawString(s,0,0);
  bigTextTest12s.unloadFont();
  bigTextTest12s.pushSprite(x,y);
  bigTextTest12s.deleteSprite();
}

void bigTextTest16(String s, int x, int y){
  
  bigTextTest16s.createSprite(140,20);
  bigTextTest16s.fillSprite(TFT_BLACK);
  bigTextTest16s.loadFont(gothicFont16);
  bigTextTest16s.setTextColor(TFT_WHITE, TFT_BLACK);
  bigTextTest16s.drawString(s,0,0);
  bigTextTest16s.unloadFont();
  bigTextTest16s.pushSprite(x,y);
  bigTextTest16s.deleteSprite();
}

void bigTextTest16short(String s, int x, int y){
  
  bigTextTest16shorts.createSprite(60,15);
  bigTextTest16shorts.fillSprite(TFT_BLACK);
  bigTextTest16shorts.loadFont(gothicFont16);
  bigTextTest16shorts.setTextColor(TFT_WHITE, TFT_BLACK);
  bigTextTest16shorts.drawString(s,0,0);
  bigTextTest16shorts.unloadFont();
  bigTextTest16shorts.pushSprite(x,y);
  bigTextTest16shorts.deleteSprite();
}

void printTemplate(){
  bigTextTest24(city, 10, 15);
  bigTextTest12("Max Temp:", 10, 40); 
  bigTextTest12("Min Temp:", 10, 55); 
  tempImage(10, 170); 
  humidImage(10, 200); 
}