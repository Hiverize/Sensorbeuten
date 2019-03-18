/*   BOB-6-Messen-Web    Webserver in WLAN WlanTest

     Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019

     Webserver, Messen BME280, DS18B20, HX711, Schalten von LEDs
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-6-Messen-Web";
const char* ssid     = "WlanTest";          // SSID     des WLAN
const char* password = "WlanTest";          // Passwort des WLAN

// LEDs
#define  LED1 23                            // Pin der LED
#define  LED2  3                            // Pin der LED
#define  LED3 19                            // Pin der LED

// allgemeine Variable
int      Counter = 0;                       // Zähler Webseiten-Aufrufe

// ESP32-Webserver
#include <WiFi.h>
WiFiServer server(80);                      // Port 80

// NTP-Zeit
#include "time.h"
const char* ntpServer          = "pool.ntp.org";
const long  gmtOffset_sec      = 3600;
const int   daylightOffset_sec = 3600;
char        chDatum[25];
char        chZeit[9];


// BME280 Sensor für Luft-Temperatur, -Feuchte und -Druck
#include <Wire.h>
#include <Adafruit_BME280.h>
char     chBME280tmp[6];
char     chBME280hum[6];
char     chBME280dru[7];
Adafruit_BME280 bme;

// DS18B20 1-wire Sensor für Temperatur
#define DS18B20Pin 18                       // GPIO-Pin für OneWire DS18B20
int     DS18B20anz = 6;                     // Anzahl Sensoren
float   DS18B20temp[10];                    // Temperaturen
float   DS18B20korr[] = {0.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0, 0.0, 0.0, 0.0 };
String  strDS18B20temp[10];
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(DS18B20Pin);
DallasTemperature DS18B20(&oneWire);
char     chTmp[6];

// Waage mit ADW HX711
#define  HX711PinDT    17                   // Pin für Data
#define  HX711PinSCK   16                   // Pin für System Clock
float    HX711skala    = 3480.0;            // Skalierung      100 g
char     chHX711kg[3]  = "g ";
// float HX711skala    = 11600.0;           // Skalierung H40A 200 kg
// char  chHX711kg[3]  = "kg";
char     chHX711gew[7] = "0.00";
float    unitsalt      =      0;
#include "HX711.h"
HX711 scale(HX711PinDT, HX711PinSCK);

// 2. Setup wird beim Start einmal durchlaufen /////////////////////////////////////
void setup()
{
  Serial.begin(115200);                     // serielle Schnittstelle
  Serial.println(Programmname);

  // LEDs
  pinMode(LED1, OUTPUT);                    // set the LED pin mode
  digitalWrite(LED1, LOW);                  // LED aus
  pinMode(LED2, OUTPUT);                    // set the LED pin mode
  digitalWrite(LED2, LOW);                  // LED aus
  pinMode(LED3, OUTPUT);                    // set the LED pin mode
  digitalWrite(LED3, LOW);                  // LED aus

  // Sensor BME280
  bool status;                              // Sensor BME280
  status = bme.begin(0x76);

  // DS18B20
  DS18B20.begin();                          // DS18B20 initialisieren

  // WLAN
  Serial.println();                         // Verbindung mit WLAN
  Serial.print("Verbinden mit ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);               // Verbindung aufbauen
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100); Serial.print(".");          // warten
  }
  Serial.println("");
  Serial.print("Verbunden mit ");           // Verbindung aufgebaut
  Serial.println(ssid);
  Serial.print("IP Adresse: ");
  Serial.print(WiFi.localIP());
  Serial.print(" MAC Adresse: ");
  Serial.println(WiFi.macAddress());
  server.begin();
  Serial.println("Server gestartet");

  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();


  // Waage mit ADW HX711
  scale.set_scale(HX711skala);              // Skalierung
  scale.tare();                             // bei leerer Waage
  scale.tare();                             // bei leerer Waage
  Serial.println("Waage gestartet");
}

// Ausgabe lokale Zeit    Funktion zum Auslesen  /////////////////////////////////////
void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println(&timeinfo, "%a, %d.%b.%Y %H:%M:%S");
  //Serial.println(&timeinfo, " %d.%b.%Y %H:%M:%S");
}

// BME280  Funktion zum Auslesen  /////////////////////////////////////
void getBME280()                            // Auslesen des BME280
{
  float   h, t, p;
  char    chBuffer[80];
  t = bme.readTemperature();
  h = bme.readHumidity();
  p = bme.readPressure() / 100.0;
  /*  Serial.print("BME280:  Temp: ");      // Ausgabe unformatiert
    Serial.print(t);
    Serial.print("°C  Feuchte: ");
    Serial.print(h);
    Serial.print("%rel.  Druck: ");
    Serial.print(p);
    Serial.println("hPa");
  */
  dtostrf(t, 4, 1, chBME280tmp);
  dtostrf(h, 4, 1, chBME280hum);
  dtostrf(p, 4, 0, chBME280dru);            // Ausgabe formatiert
  sprintf(chBuffer, "BME280:  Temp: %s°C  Feuchte: %s%%rel.  Druck: %shPa", chBME280tmp, chBME280hum, chBME280dru );
  Serial.println(chBuffer);
  delay(100);
}

// DS18B20  Funktion zum Auslesen  /////////////////////////////////////
void getDS18B20()                           // Auslesen
{
  int   i      = 0;                         // Laufvariable
  String  strBuffer = "DS18B20: ";
  // char    chTmp[6];

  DS18B20.requestTemperatures();
  // Serial.print("DS18B20: ");
  for (i = 0; i < DS18B20anz; i++)
  {
    DS18B20temp[i] = DS18B20.getTempCByIndex(i);
    float fTmp = DS18B20temp[i] + DS18B20korr[i];
    if ( fTmp <  -100.0 ) { fTmp = 0.0; }
    //    Serial.print(fTmp);               // unformatierte Ausgabe
    //    Serial.print("°C ");
    dtostrf(fTmp, 4, 1, chTmp);
    strDS18B20temp[i] = chTmp;              // String-Array für WebServer
    strBuffer = strBuffer + chTmp + "°C ";  // String für Ausgabe
  }
  // Serial.println(" ");
  Serial.println(strBuffer);                // formatierte Ausgabe
}

// HX711 AnalogDigitalWandler für Wägezelle
void getHX711()
{
  char    chBuffer[80];
  scale.power_up();                         // ADC wake up
  delay(200);
  float units = scale.get_units();          // Mittelwert - Tara / Scale
  //  if ( units < 0.0 ) { units = 0.0; }
  dtostrf(units, 6, 2, chHX711gew);         // formatierte Ausgabe
  sprintf(chBuffer, "HX711:   %s %s", chHX711gew, chHX711kg);
  Serial.print(chBuffer);
  if ( units > unitsalt + 1 ) {
    Serial.print(" Sprung nach oben");
  }
  if ( units < unitsalt - 1 ) {
    Serial.print(" Sprung nach unten");
  }
  unitsalt = units;
  Serial.println();
  scale.power_down();                       // ADC in sleep mode
}


// 3. Dauerschleife  //////////////////////////////////////////////
void loop()
{
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    Counter += 1;
    Serial.print(Counter);
    Serial.println(". Client connected");

    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // your actual web page that displays temperature
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head><META HTTP-EQUIV=\"refresh\" CONTENT=\"8\"></head>");
            client.println("<body><h1>BOB - BeeObserverBremen </h1>");

            printLocalTime();               // neue Zeit
            client.println("<h2>");
            struct tm timeinfo;
            if (!getLocalTime(&timeinfo)) {
              Serial.println("Failed to obtain time");
              return;
            }
            client.println(&timeinfo, "Datum: %d.%b.%Y %H:%M:%S");
            client.println("</h2>");

            client.println("<h2>");         // Messung
            client.println(Counter);
            client.println(". Messung </h2>");

            getBME280();                    // neue Wetterdaten
            client.println("<h3>BME280  Temp.= ");
            client.println(chBME280tmp);
            client.println("&deg;C  Feuchte= ");
            client.println(chBME280hum);
            client.println("%  Druck= ");
            client.println(chBME280dru);
            client.println("hPa</h3>");

            getDS18B20();                   // neue Temperaturen
            client.println("<h3>DS18B20 Temp.= ");
            client.println(strDS18B20temp[0]);
            client.println("&deg;C  ");
            client.println(strDS18B20temp[1]);
            client.println("&deg;C  ");
            client.println(strDS18B20temp[2]);
            client.println("&deg;C  ");
            client.println(strDS18B20temp[3]);
            client.println("&deg;C  ");
            client.println(strDS18B20temp[4]);
            client.println("&deg;C  ");
            client.println("</h3>");

            getHX711();                     // neues Gewicht
            client.println("<h3>HX711   Gewicht= ");
            client.println(chHX711gew);
            client.println(" g</h3>");

            // LEDs Ein- und Aus-Schalten
            client.print("<h3> LEDs Ein- und Aus-Schalten <br> </h3> ");

            client.print("LED1 ein  <a href=\"/H1\">Hier Klicken</a> ");
            client.print("LED1 aus  <a href=\"/L1\">Hier Klicken</a> <br><br>");
            client.print("LED2 ein  <a href=\"/H2\">Hier Klicken</a> ");
            client.print("LED2 aus  <a href=\"/L2\">Hier Klicken</a> <br><br>");
            client.print("LED3 ein  <a href=\"/H3\">Hier Klicken</a> ");
            client.print("LED3 aus  <a href=\"/L3\">Hier Klicken</a> <br>");

            client.println("</body></html>");
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {             // if you got anything else but a carriage return character,
          currentLine += c;                 // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H1")) {
          digitalWrite(LED1, HIGH);
        }
        if (currentLine.endsWith("GET /L1")) {
          digitalWrite(LED1, LOW);
        }
        if (currentLine.endsWith("GET /H2")) {
          digitalWrite(LED2, HIGH);
        }
        if (currentLine.endsWith("GET /L2")) {
          digitalWrite(LED2, LOW);
        }
        if (currentLine.endsWith("GET /H3")) {
          digitalWrite(LED3, HIGH);
        }
        if (currentLine.endsWith("GET /L3")) {
          digitalWrite(LED3, LOW);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
