// Library für WiFi-Verbindung
#include <ESP8266WiFi.h>
#include <Wire.h> //Including wire library

#include <SFE_BMP180.h> //Including BMP180 library

#define ALTITUDE 522 //Altitude where I live (change this to your altitude)

SFE_BMP180 pressure; //Creating an object
// Daten des WiFi-Netzwerks
const char* ssid     = "WLAN-949910";
const char* password = "14157163383930886305";
const long interval = 7000;
unsigned long previousMillis = 0;
// Port des Web Servers auf 80 setzen
WiFiServer server(80);
 double T, P, p0; //Creating variables for temp, pressure and relative pressure
// Variable für den HTTP Request
String header;

void setup() {
  Serial.begin(115200);

  // Mit dem WiFi-Netzwerk verbinden
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFiServer server(80);
  // Lokale IP-Adresse im Seriellen Monitor ausgeben und Server starten
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  if (pressure.begin()) //If initialization was successful, continue
    Serial.println("BMP180 init success");
  else //Else, stop code forever
  {
    Serial.println("BMP180 init fail");
    while (1);
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
   getData();
    previousMillis = currentMillis;
  }
   
  WiFiClient client = server.available();   // Auf Clients (Server-Aufrufe) warten

  if (client) {
   // Bei einem Aufruf des Servers
    Serial.println("Client available");
    String currentLine = "";                // String definieren für die Anfrage des Clients

    while (client.connected()) { // Loop, solange Client verbunden ist

      if (client.available()) {
        char c = client.read();             // Ein (1) Zeichen der Anfrage des Clients lesen
      //  Serial.write(c);                    // und es im Seriellen Monitor ausgeben
        header += c;
        if (c == '\n') {                    // bis eine Neue Zeile ausgegeben wird

          // Wenn der Client eine Leerzeile sendet, ist das Ende des HTTP Request erreicht
          if (currentLine.length() == 0) {

            // Der Server sendet nun eine Antwort an den Client
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Die Webseite anzeigen
           // client.println("<!DOCTYPE html><html>");
            //client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
           // client.println("<link rel=\"icon\" href=\"data:,\"></head>");
           // client.println("<body><h1 align=\"center\">Hier spricht dein Server! :)</h1></body></html>");

            // Die Antwort mit einer Leerzeile beenden
            client.println(String(T,2)+"_"+String(p0));
            client.println();
            // Den Loop beenden
            break;
          } else { // Bei einer Neuen Zeile, die Variable leeren
            currentLine = "";
          }
        } else if (c != '\r') {  // alles andere als eine Leerzeile wird
          currentLine += c;      // der Variable hinzugefüht
        }
      }
    }
    // Variable für den Header leeren
    header = "";
    // Die Verbindung beenden
    client.stop();
  //  Serial.println("Client disconnected");
   // Serial.println("");
  }
}

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
