/*   BOB-4-BME280-Feuchte  Sensor BME280

     Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019

     Sensor für Luft-Temperatur, -Feuchte und -Druck
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-4-BME280-Feuchte";

// allgemeine Variable
int      Counter = 0;              // Zähler 

// BME280 Sensor für Luft-Temperatur, -Feuchte und -Druck
#include <Wire.h>
#include <Adafruit_BME280.h>
char     chBME280tmp[6];
char     chBME280hum[6];
char     chBME280dru[7];
Adafruit_BME280 bme;

// 2. Setup wird beim Start einmal durchlaufen  ///////////////////
void setup()
{
  Serial.begin(115200);
  Serial.println(Programmname);

  // Sensor BME280
  bool status = 0;                 // Sensor BME280
  status = bme.begin(0x76);
  Serial.println("BME280 gestartet");
}

// BME280  Funktion zum Auslesen   /////////////////////////////////////
void getBME280()                   // Auslesen des BME280
{
  float   h, t, p;
  char    chBuffer[80];
  t = bme.readTemperature();
  h = bme.readHumidity();
  p = bme.readPressure() / 100.0;
  /*
    Serial.print("BME280:  Temp: "); // Ausgabe unformatiert
    Serial.print(t);
    Serial.print("°C  Feuchte: ");
    Serial.print(h);
    Serial.print("%rel.  Druck: ");
    Serial.print(p);
    Serial.println("hPa");
  */
  dtostrf(t, 4, 1, chBME280tmp);
  dtostrf(h, 4, 1, chBME280hum);
  dtostrf(p, 4, 0, chBME280dru);   // Ausgabe formatiert
  sprintf(chBuffer, "BME280:  Temp: %s°C  Feuchte: %s%%rel.  Druck: %shPa", chBME280tmp, chBME280hum, chBME280dru );
  Serial.println(chBuffer);
  delay(100);
}

// 3. Dauerschleife  //////////////////////////////////////////////
void  loop()
{
  Counter++;
  Serial.print(Counter);
  Serial.println(". BME280 Test starten ... ");

  getBME280();                     // neue Wetterdaten

  delay(2000);                     // Verzögerung
}
