/*   BOB-6-Messen-OLED     Messen mit OLED-Display

     Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019
 
     OLED-Display I2C 1,3"   Feuchte, Temperatur und Gewicht messen
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-6-Messen-OLED";

// LEDs
#define  LED1 23                             // LED an Pin x
#define  LED2  3                             // LED an Pin x
#define  LED3 19                             // LED an Pin x


// I2C-OLED-Display 1,3"
#define OLEDpinSCL   22                      // SCL = SysClocK
#define OLEDpinSDA   21                      // SDA = SysDAta
#define OLEDpinRST   16                      // RST = ReSet
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLEDpinRST, OLEDpinSCL, OLEDpinSDA );

// allgemeine Variable
int      Counter = 0;                        // Zähler 

// BME280 Sensor für Luft-Temperatur, -Feuchte und -Druck
#include <Wire.h>
#include <Adafruit_BME280.h>
char     chBME280tmp[6];
char     chBME280hum[6];
char     chBME280dru[7];
Adafruit_BME280 bme;

// DS18B20 1-wire Sensor für Temperatur
#define DS18B20Pin 18                        // GPIO-Pin für OneWire DS18B20
int     DS18B20anz = 6;                      // Anzahl Sensoren
float   DS18B20temp[10];                     // Temperaturen
float   DS18B20korr[] = {0.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0, 0.0, 0.0, 0.0 };
String  strDS18B20temp[10];
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(DS18B20Pin);
DallasTemperature DS18B20(&oneWire);
char     chTmp[6];
char     chTmp1[6], chTmp2[6], chTmp3[6];
char     chTmp4[6], chTmp5[6], chTmp6[6];

// Waage mit ADW HX711
#define  HX711PinDT    17                    // Pin für Data
#define  HX711PinSCK   16                    // Pin für System Clock
float  HX711skala    = 3480.0;             // Skalierung      100 g
char   chHX711kg[3]  = "g ";
//float    HX711skala    = 11600.0;            // Skalierung H40A 200 kg
//char     chHX711kg[3]  = "kg";
char     chHX711gew[7] = "0.00";
float    unitsalt      =      0;
#include "HX711.h"
HX711 scale(HX711PinDT, HX711PinSCK);

// 2. Setup wird beim Start einmal durchlaufen  ///////////////////
void setup()
{
  Serial.begin(115200);
  Serial.println(Programmname);

  pinMode(LED1, OUTPUT);                     // Port als Ausgabe
  pinMode(LED2, OUTPUT);                     // Port als Ausgabe
  pinMode(LED3, OUTPUT);                     // Port als Ausgabe

  // Waage mit ADW HX711
  scale.set_scale(HX711skala);               // Skalierung
  scale.tare();                              // bei leerer Waage
  scale.tare();                              // bei leerer Waage
  Serial.println("Waage gestartet");

  // I2C-OLED-Display 1,3"
  u8g2.begin();                              // Initialize the graphics library
  u8g2.setFont(u8g2_font_6x10_tf);
  delay(100);
  Serial.println("I2C-OLED-Display gestartet");

  // Sensor BME280
  bool status;                               // Sensor BME280
  status = bme.begin(0x76);
  Serial.println("BME280 gestartet");

  // DS18B20
  DS18B20.begin();                           // DS18B20 initialisieren
  Serial.println("DS18B20 gestartet");

}

// BME280  Funktion zum Auslesen  /////////////////////////////////////
void getBME280()                             // Auslesen des BME280
{
  float   h, t, p;
  char    chBuffer[80];
  t = bme.readTemperature();
  h = bme.readHumidity();
  p = bme.readPressure() / 100.0;
  /*
    Serial.print("BME280:  Temp: ");         // Ausgabe unformatiert
    Serial.print(t);
    Serial.print("°C  Feuchte: ");
    Serial.print(h);
    Serial.print("%rel.  Druck: ");
    Serial.print(p);
    Serial.println("hPa");
  */
  dtostrf(t, 4, 1, chBME280tmp);
  dtostrf(h, 4, 1, chBME280hum);
  dtostrf(p, 4, 0, chBME280dru);             // Ausgabe formatiert
  sprintf(chBuffer, "BME280:  Temp: %s°C  Feuchte: %s%%rel.  Druck: %shPa", chBME280tmp, chBME280hum, chBME280dru );
  Serial.println(chBuffer);
  delay(100);
}

// DS18B20  Funktion zum Auslesen  /////////////////////////////////////
void getDS18B20()                            // Auslesen
{
  int   i      = 0;                          // Laufvariable
  String  strBuffer = "DS18B20: ";
  // char    chTmp[6];

  DS18B20.requestTemperatures();
  //  Serial.print("DS18B20: ");
  for (i = 0; i < DS18B20anz; i++)
  {
    DS18B20temp[i] = DS18B20.getTempCByIndex(i);
    float fTmp = DS18B20temp[i] + DS18B20korr[i];
    if ( fTmp <  -100.0 ) { fTmp = 0.0; }
    //    Serial.print(fTmp);                // unformatierte Ausgabe
    //    Serial.print("°C ");
    dtostrf(fTmp, 4, 1, chTmp);
    strDS18B20temp[i] = chTmp;               // String-Array für WebServer
    strBuffer = strBuffer + chTmp + "°C ";   // String für Ausgabe
  }
  strDS18B20temp[0].toCharArray(chTmp1, 6);
  strDS18B20temp[1].toCharArray(chTmp2, 6);
  strDS18B20temp[2].toCharArray(chTmp3, 6);
  strDS18B20temp[3].toCharArray(chTmp4, 6);
  strDS18B20temp[4].toCharArray(chTmp5, 6);
  strDS18B20temp[5].toCharArray(chTmp6, 6);
  //  Serial.println(" ");
  Serial.println(strBuffer);                 // formatierte Ausgabe
}

// HX711 AnalogDigitalWandler für Wägezelle
void getHX711()
{
  char    chBuffer[80];
  scale.power_up();                          // ADC wake up
  delay(200);
  float units = scale.get_units();           // Mittelwert - Tara / Scale
  if ( units < 0.0 ) { units = 0.0; }

  dtostrf(units, 6, 2, chHX711gew);          // formatierte Ausgabe
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
  scale.power_down();                        // ADC in sleep mode
}

// 3. Dauerschleife  //////////////////////////////////////////////
void  loop()
{
  int  zeile     = 0;
  char chBuffer[200];

  Counter++;
  Serial.print(Counter);
  Serial.println(". OLED Messen start ... ");

  // Ausgabe auf I2C-OLED-Display 1,3"
  u8g2.clearBuffer();                        // Clear the display buffer

  zeile = 1;
  sprintf(chBuffer, "BOB  %4d. Messung", Counter );
  u8g2.drawStr(3, (zeile) * 8, chBuffer);    // x, y, Text

  getHX711();                                // neues Gewicht
  zeile = 2;
  sprintf(chBuffer, "Waage %s %s", chHX711gew, chHX711kg );
  u8g2.drawStr(3, zeile * 9, chBuffer);      // x, y, Text

  getBME280();                               // neue Wetterdaten
  zeile = 3;
  sprintf(chBuffer, "BME280" );
  u8g2.drawStr(3, zeile * 9, chBuffer);      // x, y, Text
  zeile = 4;
  sprintf(chBuffer, "%s C %s%% %shPa", chBME280tmp, chBME280hum, chBME280dru);
  u8g2.drawStr(3, zeile * 9, chBuffer);      // x, y, Text

  getDS18B20();                              // neue Temperatur
  zeile = 5;
  sprintf(chBuffer, "DS18B20" );
  u8g2.drawStr(3, zeile * 9, chBuffer);      // x, y, Text
  zeile = 6;
  sprintf(chBuffer, "%s C %s C %s C", chTmp1, chTmp2, chTmp3 );
  u8g2.drawStr(3, zeile * 9, chBuffer);      // x, y, Text
  zeile = 7;
  sprintf(chBuffer, "%s C %s C %s C", chTmp4, chTmp5, chTmp6 );
  u8g2.drawStr(3, zeile * 9, chBuffer);      // x, y, Text

  u8g2.sendBuffer();                         // Send the display buffer

  // Serial.println(" ");
  delay(2000);                               // Delay.
}
