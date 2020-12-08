

//@-ttgo配置
#include "config.h"
#include <WiFi.h>
#include <WiFiUdp.h>


WiFiUDP Udp;  // Creation of wifi Udp instance

char packetBuffer[255];

unsigned int localPort = 8883;

const char *ssid = "8879";  
const char *password = "blackbug381";

IPAddress ipServidor(10, 0, 0, 14);   // Declaration of default IP for server
/* 
 *  The ip address of the client has to be different to the server
 *  other wise it will conflict because the client tries to connect
 *  to it self.
 */
IPAddress ipCliente(10, 0, 0, 7);   // Different IP than server
IPAddress Subnet(255, 255, 0, 0);

void setup() {

  unsigned long startingTime = millis();

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < 10000)
  {
    delay(100);
  }

  Serial.println(WiFi.localIP().toString());
  delay(1000);

  WiFi.mode(WIFI_STA); // ESP-32 as client
  WiFi.config(ipCliente, ipServidor, Subnet);
  Udp.begin(localPort);
}

void loop() {
//unsigned long Tiempo_Envio = millis();

//SENDING
//     Udp.beginPacket(ipServidor,8881);   //Initiate transmission of data
    
//     Udp.printf("Millis: ");
    
//     char buf[20];   // buffer to hold the string to append
//     unsigned long testID = millis();   // time since ESP-32 is running millis() 
//     sprintf(buf, "%lu", testID);  // appending the millis to create a char
//     Udp.printf(buf);  // print the char
    
//     Udp.printf("\r\n");   // End segment
    
//     Udp.endPacket();  // Close communication
    
//     Serial.print("enviando: ");   // Serial monitor for user 
//     Serial.println(buf);
    
// delay(5); // 
 
//RECEPTION
  int packetSize = Udp.parsePacket();   // Size of packet to receive
  if (packetSize) {       // If we received a package
    
    int len = Udp.read(packetBuffer, 255);
    
    if (len > 0) packetBuffer[len-1] = 0;
    Serial.print("RECIBIDO(IP/Port/Size/Datos): ");
    Serial.print(Udp.remoteIP());Serial.print(" / ");
    Serial.print(Udp.remotePort()); Serial.print(" / ");
    Serial.print(packetSize);Serial.print(" / ");
    Serial.println(packetBuffer);
  }
// Serial.println("");
delay(5);
}

