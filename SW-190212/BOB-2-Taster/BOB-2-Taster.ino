/*   BOB-2-Taster      Testet Taster
 
     Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019

     Testet LEDs und Tasten
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-2-Taster";

// LEDs
#define  LED1 23                   // LED an Pin x
#define  LED2  3                   // LED an Pin x
#define  LED3 19                   // LED an Pin x

#define  SW1   4                   // Taster an Pin x
#define  SW2   0                   // Taster an Pin x
#define  SW3   15                  // Taster an Pin x

// allgemeine Variable
int      Counter = 0;              // Zähler 

// 2. Setup wird beim Start einmal durchlaufen  ///////////////////
void setup() 
{
  Serial.begin(115200);            // Serieller Monitor
  Serial.println(Programmname);

  pinMode(LED1, OUTPUT);           // Port als Ausgabe
  pinMode(LED2, OUTPUT);           // Port als Ausgabe
  pinMode(LED3, OUTPUT);           // Port als Ausgabe
  pinMode(SW1,  INPUT);            // Port als Eingabe
  pinMode(SW2,  INPUT);            // Port als Eingabe
  pinMode(SW3,  INPUT);            // Port als Eingabe
}

// 3. Dauerschleife  //////////////////////////////////////////////
void loop() 
{
  int msec = 100;                  // wartet x msec
  Counter++;
  Serial.print(Counter);
  Serial.print(". Taster ");

  int statusSW1   = digitalRead(SW1);
  Serial.print(" SW1: ");
  Serial.print(statusSW1);
  digitalWrite(LED1, !statusSW1);

  int statusSW2   = digitalRead(SW2);
  Serial.print(" SW2: ");
  Serial.print(statusSW2);
  digitalWrite(LED2, !statusSW2);

  int statusSW3   = digitalRead(SW3);
  Serial.print(" SW3: ");
  Serial.print(statusSW3);
  digitalWrite(LED3, !statusSW3);
  
  Serial.println(" ");
    delay(msec);                    // Verzögerung
}
