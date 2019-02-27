/*   BOB-5-WebServer    in WLAN WlanTest

     Testprogramm für ESP32 Dev Module      Didi Lamken 30.01.2019

     WebServer
*/
// 1. allgemeine Definitionen /////////////////////////////////////
const char* Programmname = "BOB-5-WebServer";

const char* ssid     = "WlanTest";         // SSID     des WLAN
const char* password = "WlanTest";         // Passwort des WLAN
//const char* ssid     = "WlanBOB1";       // SSID     des WLAN
//const char* password = "WlanBOB1";       // Passwort des WLAN

// allgemeine Variable
int      Counter = 0;                      // Zähler Webseiten-Aufrufe

// ESP32-Webserver
#include <WiFi.h>
WiFiServer server(80);                     // Port 80

// 2. Setup wird beim Start einmal durchlaufen /////////////////////////////////////
void setup()
{
  Serial.begin(115200);                    // serielle Schnittstelle
  Serial.println(Programmname);

  // WLAN
  Serial.println();                        // Verbindung mit WLAN
  Serial.print("Verbinden mit ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);              // Verbindung aufbauen
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100); Serial.print(".");         // warten
  }
  Serial.println("");
  Serial.print("Verbunden mit ");          // Verbindung aufgebaut
  Serial.println(ssid);
  Serial.print("IP Adresse: ");
  Serial.print(WiFi.localIP());
  Serial.print(" MAC Adresse: ");
  Serial.println(WiFi.macAddress());
  server.begin();
  Serial.println("Server gestartet");
}

// 3. Dauerschleife  //////////////////////////////////////////////
void loop()
{
  WiFiClient client = server.available();  // listen for incoming clients
  if (client) {                            // if you get a client,
    Counter += 1;
    Serial.print(Counter);
    Serial.println(". Client connected");

    String currentLine = "";               // make a String to hold incoming data from the client
    while (client.connected()) {           // loop while the client's connected
      if (client.available()) {            // if there's bytes to read from the client,
        char c = client.read();            // read a byte, then
        Serial.write(c);                   // print it out the serial monitor
        if (c == '\n') {                   // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // your actual web page that displays temperature
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head><META HTTP-EQUIV=\"refresh\" CONTENT=\"10\"></head>");
            client.println("<body><h1>BOB - BeeObserverBremen </h1>");

            client.println("<h2>");               // Messung
            client.println(Counter);
            client.println(". Messung </h2>");

            client.println("</body></html>");
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
