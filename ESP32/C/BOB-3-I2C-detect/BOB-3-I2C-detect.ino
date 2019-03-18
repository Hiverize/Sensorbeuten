/* BOB-3-I2C-detect    testet I2C-Bus     
 *    
 *  Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019
 *   
 *  I2C-Adressen 03 ... 77
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-3-I2C-detect";

// I2C-Bus
#include <Wire.h> 
#include <i2cdetect.h>

int msec = 2000;              // Verzögerung in msec
int anf  = 0x03;              // I2C-Adresse Anfang 
int end  = 0x77;              // I2C-Adresse Ende

// 2. Setup wird beim Start einmal durchlaufen  ///////////////////
void setup() 
{
  Serial.begin(115200);       // Serieller Monitor setze Geschwindigkeit
  Serial.println(Programmname);
  
  Serial.print("i2cdetect    Adresse 0x");
  Serial.print(anf, HEX);
  Serial.print(" bis 0x");
  Serial.println(end, HEX);
  
  Wire.begin();               // I2C-Bus starten
}

// 3. Dauerschleife  //////////////////////////////////////////////
void loop() 
{
  //Serial.println("loop:  Anfang");
  i2cdetect(anf, end);        // Bereich von 0x03 bis 0x77
  delay(msec);   
}
