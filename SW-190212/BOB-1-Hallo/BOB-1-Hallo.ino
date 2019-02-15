/*   BOB-1-Hallo        der erste Test

     Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019

     der erste Test
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-1-Hallo";

// allgemeine Variable
int      Counter = 0;                     // Zähler 

// 2. Setup wird beim Start einmal durchlaufen  ///////////////////
void setup()
{
  Serial.begin(115200);                   // Serieller Monitor Baud-Rate
  Serial.println(Programmname);
}
 
// 3. Dauerschleife  //////////////////////////////////////////////
void  loop()
{
  Counter++;
  Serial.print(Counter);
  Serial.println(". Test startet ... ");

  delay(2000);                            // Verzögerung
}
