


// C:\Users\DX\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\libraries\WiFi\src
#include <WiFi.h>
#include <WiFiUdp.h>


#include <Adafruit_NeoPixel.h>

#define PIN            21
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      12

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


WiFiUDP Udp;  // Creation of wifi Udp instance


char packetBuffer[50];

unsigned int localPort = 6000;

const char *ssid = "8879";  
const char *password = "blackbug381";

// const char *ssid = "DX_JS";  
// const char *password = "dingxiao";

// IPAddress ipServidor(192, 168, 31, 188);   // Declaration of default IP for server 10, 0, 0, 14
IPAddress ipServidor(224, 100, 23, 200);   // Declaration of default IP for server 10, 0, 0, 14
// IPAddress ipServidor(10, 0, 0, 14);   // Declaration of default IP for server 10, 0, 0, 14
/* 
 *  The ip address of the client has to be different to the server
 *  other wise it will conflict because the client tries to connect
 *  to it self.
 */
IPAddress ipCliente(10, 0, 0, 24);   // Different IP than server  TTGO ip 10.0.0.7
// IPAddress ipCliente(192, 168, 31, 79);   // Different IP than server  TTGO ip 10.0.0.7
IPAddress Subnet(255, 255, 255, 0);
// IPAddress Subnet(255, 0, 0, 0);

char buf[20];   // buffer to hold the string to append

int Multicast_Flag = 1;  //0:点播  1:组播
int send_count = 0;


int  Color_R;
int  Color_G;
int  Color_B;

// ---------------------------------------------------------------
void setup() {

  unsigned long startingTime = millis();

  for(int i=0; i<20; i++)
  buf[i] = i;
  // Serial.println(buf[15]);

  Serial.begin(115200);

  pixels.begin(); // This initializes the NeoPixel library. 

  WiFi.begin(ssid, password);
  // if (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //   Serial.println("WiFi Failed");
  //   while (1) {
  //     delay(1000);
  //   }
  // }
  while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < 10000)
  {
    delay(100);
  }

  //This is used to ensure low latency. I've tried it both commented out and not and nothing changes.
  WiFi.setSleep(false); //Disable modem sleep to ensure low latency wifi.

  Serial.println(WiFi.localIP().toString());
  delay(1000);

  // 点播
  WiFi.mode(WIFI_STA); // ESP-32 as client
  WiFi.config(ipCliente, ipServidor, Subnet);
  if(Multicast_Flag == 0)
  Udp.begin(localPort);  //点播

  //组播
  // https://github.com/espressif/arduino-esp32/issues/4410
  //Init the multicast UDP with One Object : Universe 5
  else if(Multicast_Flag == 1)
  Udp.beginMulticast(ipServidor, 6000);
  //Add another Multicast Listener for Universe 6? No, probably just reinitializes it. Only Universe 6 visible in output.
  // udp.beginMulticast(IPAddress(239, 255, 0, 6), (uint16_t )5568);


}


void loop() {
//unsigned long Tiempo_Envio = millis();

    send_count = send_count + 1;
    if(send_count >= 10)
    {
      send_count = 0;
      //SENDING
      //点播  also 组播
      if(Multicast_Flag == 0)
      Udp.beginPacket(IPAddress(192, 168, 31, 188),6000);   //Initiate transmission of data
      else if(Multicast_Flag == 1)
      Udp.beginPacket(IPAddress(224, 100, 23, 200),6000);   //Initiate transmission of data
      //  Udp.beginMulticastPacket();   //Initiate transmission of data
      // 组播
      // Serial.println(Udp.remoteIP());
      // Serial.println(Udp.remotePort());
      // Udp.beginMulticastPacket();
      
      // Udp.printf("Millis: ");
      

      // unsigned long testID = millis();   // time since ESP-32 is running millis() 
      // sprintf(buf, "%lu", testID);  // appending the millis to create a char
      // Udp.printf(buf);  // print the char
      Udp.write((const uint8_t*)buf, 20); //复制数据到发送缓存
      // Udp.printf("Seconds since boot: %lu", millis()/1000);
      
      // Udp.printf("\r\n");   // End segment
      
      Udp.endPacket();  // Close communication
      
      // Serial.print("enviando: ");   // Serial monitor for user 
      // Serial.println(buf);
      
    // delay(5); // 
    }
 
//RECEPTION
  int packetSize = Udp.parsePacket();   // Size of packet to receive
  if (packetSize) {       // If we received a package
    
    int len = Udp.read(packetBuffer, 50);

    Color_R = packetBuffer[40];
    Color_G = packetBuffer[41];
    Color_B = packetBuffer[42];
    
    // if (len > 0) packetBuffer[len-1] = 0;
    // Serial.print("RECIBIDO(IP/Port/Size/Datos): ");
    // Serial.print(Udp.remoteIP());Serial.print(" / ");
    // Serial.print(Udp.remotePort()); Serial.print(" / ");
    // Serial.print(packetSize);Serial.print(" / ");
    // Serial.println(packetBuffer);
    // Serial.println(packetBuffer[46]);
  }

  //@-显示颜色
  colorWipe(pixels.Color(Color_R, Color_G, Color_B), 1);    // Green

  // Serial.println("");
  delay(5);

}


void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<pixels.numPixels(); i++) { // For each pixel in pixels...
    pixels.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    pixels.show();                          //  Update pixels to match
    delay(wait);                           //  Pause for a moment
  }
}