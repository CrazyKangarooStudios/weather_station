/*
 * HTTP Client GET Request
 * Copyright (c) 2018, circuits4you.com
 * All rights reserved.
 * https://circuits4you.com 
 * Connects to WiFi HotSpot. */
 #include <NTPClient.h>
 #include <WiFiUdp.h>
 #define NTP_OFFSET   80 * 90      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "europe.pool.ntp.org"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
#include <SFE_BMP180.h> //Including BMP180 library
#define ALTITUDE 522 //Altitude where I live (change this to your altitude)
SFE_BMP180 pressure; //Creating an object
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
WiFiClient wifiClient;
WiFiClientSecure client;
/* Set these to your desired credentials. */
const char *ssid = "WLAN-949910";  //ENTER YOUR WIFI SETTINGS
const char *password = "14157163383930886305";
const long interval = 1000;
unsigned long previousMillis = 0;
// Port des Web Servers auf 80 setzen
WiFiServer server(80);
 double T, P, p0; //Creating variables for temp, pressure and relative pressure
//Web/Server address to read/write from 
const char *host = "https://weather-station.crazykaenguru.repl.co/";   //https://circuits4you.com website or IP address of server

//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  timeClient.begin();
  client.setInsecure();
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  if (pressure.begin()) //If initialization was successful, continue
    Serial.println("BMP180 init success");
  else //Else, stop code forever
  {
    Serial.println("BMP180 init fail");
    while (1);
  }
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
timeClient.update();
  unsigned long currentMillis = millis();
  String formattedTime = timeClient.getFormattedTime();
  Serial.print(formattedTime);

  if (currentMillis - previousMillis >= interval) {
   getData();
    previousMillis = currentMillis;
  }
  HTTPClient http;    //Declare object of class HTTPClient

  String ADCData, station, getData, Link;
  int adcvalue=analogRead(A0);  //Read Analog value of LDR
  ADCData = String(adcvalue);   //String to interger conversion
  station = "B";

  //GET Data
  getData =String(T)+"_"+String(p0)+"_"+String(formattedTime); //"?status=" + ADCData + "&station=" + station ;  //Note "?" added at front
  Link = "https://weather-station.crazykaenguru.repl.co/" + getData;
  
  http.begin(client,Link);     //Specify request destination
//  getData();
 
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  http.end();  //Close connection
  delay(5000);  //GET Data at every 5 seconds
}
//=======================================================================
void getData()
{
  char status;

  status = pressure.startTemperature();
  if (status != 0) {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0) {


      status = pressure.startPressure(3);

      if (status != 0) {
        delay(status);

        status = pressure.getPressure(P, T);
        if (status != 0) {
        
          p0 = pressure.sealevel(P, ALTITUDE);
         
        }
      }
    }
  }  
 // delay(100);
}
