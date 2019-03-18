/*   BOB-5-NTP-Zeit    in WLAN WlanTest

     Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019
 
     NTP-Zeit aus Internet 
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-5-NTP-Zeit";
const char* ssid     = "WlanTest";         // SSID     des WLAN
const char* password = "WlanTest";         // Passwort des WLAN

// allgemeine Variable
int      Counter = 0;                      // Zähler 

// NTP-Zeit
#include <WiFi.h>
#include "time.h"
const char* ntpServer          = "pool.ntp.org";
const long  gmtOffset_sec      = 3600;
const int   daylightOffset_sec = 3600;
char        chDatum[25];
char        chZeit[9];

// 2. Setup wird beim Start einmal durchlaufen /////////////////////////////////////
void setup()
{
  Serial.begin(115200);                    // serielle Schnittstelle
  Serial.println(Programmname);

  // WLAN
  Serial.print("Verbinden mit ");          // Verbindung mit WLAN
  Serial.print(ssid);
  WiFi.begin(ssid, password);              // Verbindung aufbauen
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100); Serial.print(".");         // warten
  }
  Serial.println("");
  Serial.print("Verbunden mit ");          // Verbindung aufgebaut
  Serial.println(ssid);
  Serial.print("IP Adresse: ");
  Serial.print(WiFi.localIP());
  Serial.print(" MAC Adresse: ");
  Serial.println(WiFi.macAddress());

  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

// disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
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

// 3. Dauerschleife  //////////////////////////////////////////////
void loop()
{
  Counter++;
  Serial.print(Counter);
  Serial.print(". NTP-Zeit Test startet ... ");

  printLocalTime();

  delay(1000);                             // Verzögerung
}
