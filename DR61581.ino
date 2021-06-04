#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "SPI_595_ESP8266.h"

const char* ssid     = "Livebox-0B8F";
const char* password = "1122334455";
const char* host     = "192.168.0.34";
const int   port     = 80;

const char* apiKey   = "rrJYD7EXmepFPpyvO3yXcO2O4M1Cc7x7"; // API id jeedom
const int   watchdog = 20000; //60000; //interval envoi requette HTTP vers jeedom
unsigned long previousMillis = millis(); 

#define ONE_WIRE_BUS 5

//création d'une instance d'objet de la classe SPI_595_ESP8266  
SPI_595_ESP8266 tft;

//Crée une instance de serveur web
ESP8266WebServer server ( 80 );
//Cré un client web
HTTPClient http;

boolean sendToJeedom(String url);
bool flag=true;

void setup() {
  tft.begin();
  delay(300);
  //tft.invertDisplay(true);
  tft.fillScreen(BLACK);
  
  WiFi.begin(ssid, password);// Connexion au réseau local en wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  
  tft.setTextSize(1);
  tft.setRotation(3); //met l'affichage dans le bon sens
  tft.println("WiFi connected");
  tft.printUTF8((char *)"WiFi connected\n");
  //tft.print("Local IP address: ");
  tft.printUTF8((char *)"Local IP address: ");
  //ip.toString().toCharArray(IP, 16);
  char buf[16];
  sprintf(buf,"IP:%d.%d.%d.%d",WiFi.localIP()[0],WiFi.localIP()[1],WiFi.localIP()[2],WiFi.localIP()[3] );
  //tft.println(WiFi.localIP()); 
  tft.printUTF8((char *) buf);
  delay(100);
  server.on("/datas", update1);  
  server.begin();
  server.handleClient();
}

/*****************************************************************
**                       LOOP
******************************************************************/
void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();

  if ( currentMillis - previousMillis > watchdog ) 
  {
    previousMillis = currentMillis;
    flag=true;
    if(WiFi.status() != WL_CONNECTED) {
      tft.println("WiFi not connected !");
    } else 
    {  
      /**********************************************************************************************
      Ici on fabrique l'url qui va lancer script_003 de jeedom dont l'id=258
      http://192.168.0.34/core/api/jeeApi.php?apikey=rrJYD7EXmepFPpyvO3yXcO2O4M1Cc7x7&type=cmd&id=258
      ***********************************************************************************************/
      String baseurl = "/core/api/jeeApi.php?apikey=";
      baseurl += apiKey;
      //baseurl += "&type=cmd&id=258";  // Id de la cmd "bang" de script_003  
      baseurl += "&type=cmd&id=276";  // Id de la cmd "bang" de script_001 (afficheur d'essai)
      String url = baseurl;  
      Serial.println (baseurl);   
      sendToJeedom(url);       
    }
    delay(1000);
  }
  
}

/*****************************************************************
**                      UPDATE
******************************************************************/
void update1()
{  
  char unit[10];
  uint8_t sizechar=1,offset=0;
  
  if(flag==true) //empèche les redondances de trames
  {
    tft.clearScreen();
    flag=false;
    for ( int i = 0 ; i < server.args()-1-0; i++ )
    {
    //Serial.print("argName=");Serial.println((char *)server.argName(i).c_str());
    if(server.argName(i)=="IN") {
      tft.setCursor(24,0);tft.setTextColor(YELLOW); 
      //unit=(char *)"°C"; sizechar=3; 
      strcpy(unit,"°C");sizechar=3; 
    }
    if(server.argName(i)=="OUT") {
      tft.setCursor(0,40);tft.setTextColor(ORANGE);  
      //unit=(char *)"°C";sizechar=3;  
      strcpy(unit,"°C");sizechar=3;   
    }
   
    if(server.argName(i)=="Consigne") {
      tft.setCursor(0,83);tft.setTextColor(CYAN); 
      //unit=(char *)"°C"; sizechar=2;
      strcpy(unit,"°C");sizechar=2;
    }
   
    if(server.argName(i)=="Brûleur") {
      tft.setCursor(0,120);tft.setTextColor(YELLOW);sizechar=2;
      //if(server.arg(i)=="0") unit=" OFF"; else unit=(char *)" ON";
      if(server.arg(i)=="0") strcpy(unit," OFF"); else strcpy(unit," ON");
    }
   
    if(server.argName(i)=="Humidité") {
      //tft.setCursor(0,120+30);tft.setTextColor(GREEN);  unit=(char *)"%"; sizechar=2;
      tft.setCursor(0,120+30);tft.setTextColor(GREEN); strcpy(unit,"%");sizechar=2;
    }
   
    if(server.argName(i)=="Pression") {
      //tft.setCursor(0,150+30);tft.setTextColor(GREEN);  unit=(char *)" mb";sizechar=2;
      tft.setCursor(0,150+30);tft.setTextColor(GREEN); strcpy(unit," mb");sizechar=2;
    }
   
    if(server.argName(i)=="Piles thermostat") {
      //tft.setCursor(0,180+30);tft.setTextColor(WHITE); unit=(char *)"%"; sizechar=2;
      tft.setCursor(0,180+30);tft.setTextColor(WHITE); strcpy(unit,"%");sizechar=2;
    }
    if(server.argName(i)=="Piles sonde ext") {
      //tft.setCursor(0,210+30);tft.setTextColor(PINK);  unit=(char *)" "; sizechar=2;
      tft.setCursor(0,210+30);tft.setTextColor(PINK); strcpy(unit," ");sizechar=2;
    }
    if(server.argName(i)=="Fête") {
      //tft.setCursor(0,240+30);tft.setTextColor(WHITE);  unit=(char *)" "; sizechar=2;
     tft.setCursor(0,240+30);tft.setTextColor(WHITE); strcpy(unit," ");sizechar=2;
    }
    tft.setTextSize(sizechar);
    if (sizechar>0)
    tft.printUTF8((char *)server.argName(i).c_str());tft.printUTF8((char *)":");tft.printUTF8((char *)server.arg(i).c_str());tft.printUTF8((char *)unit);
    //tft.print(server.argName(i));tft.print(":");tft.print(server.arg(i));tft.print((char *)unit);
    //Serial.print((char *)server.argName(i).c_str());Serial.print(":");Serial.print((char *)server.arg(i).c_str());Serial.print((char *)unit);
   
    sizechar=0;offset=0;
    }
    String datas = server.arg("id"); 
    String token = server.arg("token");

    /**********************************
    Vérifie si le serveur est autorisé 
    ***********************************/
    if ( token != "123abCde" ) 
    {
      Serial.println("\nNot authentified ");
      return;
    }
    else
    {
      Serial.println("\nauthentified ");
      server.send(200, "application/json","success:1");
    }

    delay(1000);
    }
}

/*****************************************************************
**                       SEND TO JEEDOM
******************************************************************/
boolean sendToJeedom(String url){
  Serial.print("connecting to ");
  Serial.println(host);
  Serial.print("Requesting URL: ");
  Serial.println(url);
  http.begin(host,port,url);
  //begin(WiFiClient &client, const String& url)
  //http.begin(host,url);
  int httpCode = http.GET();
  http.end();
}
