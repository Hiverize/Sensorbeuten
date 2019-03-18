/*   BOB-4-HX711-Waage  Wägezelle mit ADW HX711

     Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019
 
     Wägezelle ( max. 100g oder H40A mit max. 200 kg ) mit ADW HX711
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-4-HX711-Waage";

// allgemeine Variable
int      Counter = 0;                  // Zähler 

// Waage mit ADW HX711
#define  HX711PinDT      17            // Pin für Data
#define  HX711PinSCK     16            // Pin für System Clock
float    HX711skala    = 3480.0;       // Skalierung      100 g
char     chHX711kg[3]  = "g ";
//float  HX711skala    = 11600.0;      // Skalierung H40A 200 kg
//char   chHX711kg[3]  = "kg";
char     chHX711gew[7] = "0.00";
float    unitsalt      =      0;
#include "HX711.h"
HX711 scale(HX711PinDT, HX711PinSCK);

// 2. Setup wird beim Start einmal durchlaufen  ///////////////////
void setup()
{
  Serial.begin(115200);
  Serial.println(Programmname);

  // Waage mit ADW HX711
  scale.set_scale(HX711skala);         // Skalierung
  scale.tare();                        // bei leerer Waage
  scale.tare();                        // bei leerer Waage
  Serial.println("Waage gestartet");
}

// HX711 AnalogDigitalWandler für Wägezelle
void getHX711()
{
  char    chBuffer[80];
  scale.power_up();                    // ADC wake up
  delay(200);
  float units = scale.get_units();     // Mittelwert - Tara / Scale
  //  if ( units < 0.0 ) { units = 0.0; }
  dtostrf(units, 6, 2, chHX711gew);    // formatierte Ausgabe
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
  scale.power_down();                  // ADC in sleep mode
}

// 3. Dauerschleife  //////////////////////////////////////////////
void  loop()
{
  Counter++;
  Serial.print(Counter);
  Serial.print(". HX711 Test startet ... ");
 
  getHX711();                          // neues Gewicht
  
  delay(2000);                         // Delay.
}
