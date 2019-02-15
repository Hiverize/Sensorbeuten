/*   BOB-2-LEDs      Testet LEDs

     Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019
 
     Testet LEDs und Tasten
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-2-LEDs";

// LEDs
#define  LED1 23                   // Pin der LED
#define  LED2  3                   // Pin der LED
#define  LED3 19                   // Pin der LED

// allgemeine Variable
int      Counter = 0;              // Zähler 

// 2. Setup wird beim Start einmal durchlaufen  ///////////////////
void setup()
{
  Serial.begin(115200);
  Serial.println(Programmname);

  // LEDs
  pinMode(LED1, OUTPUT);           // set the LED pin mode
  digitalWrite(LED1, LOW);         // LED aus
  pinMode(LED2, OUTPUT);           // set the LED pin mode
  digitalWrite(LED2, LOW);         // LED aus
  pinMode(LED3, OUTPUT);           // set the LED pin mode
  digitalWrite(LED3, LOW);         // LED aus
}


// 3. Dauerschleife  //////////////////////////////////////////////
void  loop()
{
  int msec = 100;                  // wartet x msec
  Counter++;
  Serial.print(Counter);
  Serial.println(". LED Test startet ... ");

  digitalWrite(LED1, HIGH);
  delay(msec);
  digitalWrite(LED1, LOW);

  digitalWrite(LED2, HIGH);
  delay(msec);
  digitalWrite(LED2, LOW);

  digitalWrite(LED3, HIGH);
  delay(msec);
  digitalWrite(LED3, LOW);

  delay(1000);                     // Verzögerung
}
