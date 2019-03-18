/*   BOB-3-OLED-Anzeige   OLED-Display

     Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019

     OLED-Display I2C 1,3"
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-3-OLED-Anzeige";

// I2C-OLED-Display 1,3"
#define OLEDpinSCL   22                   // SCL = SysClocK
#define OLEDpinSDA   21                   // SDA = SysDAta
#define OLEDpinRST   16                   // RST = ReSet
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLEDpinRST, OLEDpinSCL, OLEDpinSDA );

// allgemeine Variable
int      Counter = 0;                     // Zähler Webseiten-Aufrufe

// 2. Setup wird beim Start einmal durchlaufen  ///////////////////
void setup()
{
  Serial.begin(115200);
  Serial.println(Programmname);

  // I2C-OLED-Display 1,3"
  u8g2.begin();                           // Initialize the graphics library
  u8g2.setFont(u8g2_font_6x10_tf);
  delay(100);
  Serial.println("I2C-OLED-Display gestartet");
}

// 3. Dauerschleife  //////////////////////////////////////////////
void  loop()
{
  int  zeile     = 0;
  char chBuffer[200];
 
  Counter++;
  Serial.print(Counter);
  Serial.println(". OLED Test startet ... ");

  // Ausgabe auf I2C-OLED-Display 1,3"
  u8g2.clearBuffer();                     // Clear the display buffer

  zeile = 1;
  sprintf(chBuffer, "BOB-BeeObserverBremen" );
  u8g2.drawStr(3, (zeile) * 8, chBuffer); // x, y, Text

  zeile = 2;
  sprintf(chBuffer, "%4d. Messung", Counter );
  u8g2.drawStr(3, (zeile) * 8, chBuffer); // x, y, Text

  u8g2.sendBuffer();                      // Send the display buffer

  delay(2000);                            // Delay.
}
