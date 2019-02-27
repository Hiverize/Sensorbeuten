/*   BOB-5-WifiScan     Scannt WLAN-Netzwerke

     Testprogramm für ESP32 Dev Module      Didi Lamken 07.02.2019

     Scannt Wlan-Netzwerke und sortiert nach Signalstärke
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-5-WiFiScan";
#include "WiFi.h"

// 2. Setup wird beim Start einmal durchlaufen  ///////////////////
void setup()
{
  Serial.begin(115200);
  Serial.println(Programmname);

  WiFi.mode(WIFI_STA);                // station mode
  WiFi.disconnect();                  // disconnect

  Serial.println("WLAN-Setup fertig");
  delay(100);
}

// 3. Dauerschleife  //////////////////////////////////////////////
void loop()
{
  Serial.print("Scan start ... ");

  int n = WiFi.scanNetworks();        // Anzahl der gefundenen WLANs
  Serial.println("Scan stop");
  if (n == 0)
  {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" Netze gefunden");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");

      String ssid = WiFi.SSID(i);
      ssid += "              ";
      ssid  = ssid.substring(0, 18);
      Serial.print(ssid);
      Serial.print(" \t");

      Serial.print(" ");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" db ");

      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "offen" : " ");
      delay(10);
    }
  }
  Serial.println("");
  delay(5000);                        // warten
}
