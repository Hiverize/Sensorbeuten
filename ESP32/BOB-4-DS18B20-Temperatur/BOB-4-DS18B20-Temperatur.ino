/*   BOB-4-DS18B20-Temperatur  Sensor DS18B20

     Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019

     Sensor DS18B20 für Temperatur
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-4-DS18B20-Temperatur";

// allgemeine Variable
int      Counter = 0;              // Zähler Webseiten-Aufrufe

// DS18B20 1-wire Sensor für Temperatur
#define DS18B20Pin  18              // GPIO-Pin für OneWire DS18B20
int     DS18B20anz = 6;             // Anzahl Sensoren
float   DS18B20temp[10];            // Temperaturen
//float DS18B20korr[] = {0.0, 0.1, -0.2, -0.3, -0.1, -0.2, 0.0, 0.0, 0.0, 0.0 };
float   DS18B20korr[] = {0.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0, 0.0, 0.0, 0.0 };
String  strDS18B20temp[10];
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(DS18B20Pin);
DallasTemperature DS18B20(&oneWire);
char     chTmp[6];
char     chTmp1[6], chTmp2[6], chTmp3[6];
char     chTmp4[6], chTmp5[6], chTmp6[6];

// 2. Setup wird beim Start einmal durchlaufen  ///////////////////
void setup()
{
  Serial.begin(115200);
  Serial.println(Programmname);

  // DS18B20
  DS18B20.begin();                         // DS18B20 initialisieren
  Serial.println("DS18B20 gestartet");
}

// DS18B20  Funktion zum Auslesen  /////////////////////////////////////
void getDS18B20()                           // Auslesen
{
  int   i      = 0;                 // Laufvariable
  String  strBuffer = "DS18B20: ";

  DS18B20.requestTemperatures();
  //  Serial.print("DS18B20: ");
  for (i = 0; i < DS18B20anz; i++)
  {
    DS18B20temp[i] = DS18B20.getTempCByIndex(i);
    float fTmp = DS18B20temp[i] + DS18B20korr[i];
    if ( fTmp == -127.0 ) { fTmp = 0.0; }
    //    Serial.print(fTmp);                       // unformatierte Ausgabe
    //    Serial.print("°C ");
    dtostrf(fTmp, 4, 1, chTmp);
    strDS18B20temp[i] = chTmp;                 // String-Array für WebServer
    strBuffer = strBuffer + chTmp + "°C ";   // String für Ausgabe
  }
  strDS18B20temp[0].toCharArray(chTmp1, 6);
  strDS18B20temp[1].toCharArray(chTmp2, 6);
  strDS18B20temp[2].toCharArray(chTmp3, 6);
  strDS18B20temp[3].toCharArray(chTmp4, 6);
  strDS18B20temp[4].toCharArray(chTmp5, 6);
  strDS18B20temp[5].toCharArray(chTmp6, 6);
  //  Serial.println(" ");
  Serial.println(strBuffer);                // formatierte Ausgabe
}

// 3. Dauerschleife  //////////////////////////////////////////////
void  loop()
{
  Counter++;
  Serial.print(Counter);
  Serial.println(". DS18B20 Test starten ... ");

  getDS18B20();                          // neue Temperatur

  delay(2000);                            // Delay.
}
