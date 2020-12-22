

//@-ttgo配置
#include "config.h"
#include <WiFi.h>
#include <WiFiUdp.h>


//@-TTGO
TTGOClass *ttgo;

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
IPAddress ipCliente(10, 0, 0, 18);   // Different IP than server  TTGO ip 10.0.0.7
// IPAddress Subnet(255, 255, 255, 0);
IPAddress Subnet(255, 0, 0, 0);

char buf[20];   // buffer to hold the string to append

int Multicast_Flag = 1;  //0:点播  1:组播


// ---------------------------------------------------------------
void setup() {

  unsigned long startingTime = millis();

  for(int i=0; i<20; i++)
  buf[i] = i;
  // Serial.println(buf[15]);

  Serial.begin(115200);
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

  //@-ttgo初始化
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();
  //@-ttgo初始化lvgl库
  ttgo->lvgl_begin();
  //@-进入lvgl主页面
  lv_ex_tileview_1();
}

void lv_ex_tileview_1(void)
{
    static lv_style_t model_style;
    lv_style_init(&model_style);
    lv_style_set_text_color(&model_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    lv_obj_t *btn_xh_dsp_fwd = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_width(btn_xh_dsp_fwd, 100);
    lv_obj_set_height(btn_xh_dsp_fwd, 50);
    lv_obj_align(btn_xh_dsp_fwd, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);

    lv_obj_t *label_xh_dsp_fwd = lv_label_create(btn_xh_dsp_fwd, NULL);
    lv_obj_add_style(label_xh_dsp_fwd, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(label_xh_dsp_fwd, "OK");
}

void loop() {
//unsigned long Tiempo_Envio = millis();

    //SENDING
    //点播  also 组播
    if(Multicast_Flag == 0)
    Udp.beginPacket(IPAddress(10, 0, 0, 14),6000);   //Initiate transmission of data
    else if(Multicast_Flag == 1)
    Udp.beginPacket(IPAddress(224, 100, 23, 200),6000);   //Initiate transmission of data
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
    
  delay(5); // 
 
//RECEPTION
  int packetSize = Udp.parsePacket();   // Size of packet to receive
  if (packetSize) {       // If we received a package
    
    int len = Udp.read(packetBuffer, 50);
    
    if (len > 0) packetBuffer[len-1] = 0;
    Serial.print("RECIBIDO(IP/Port/Size/Datos): ");
    Serial.print(Udp.remoteIP());Serial.print(" / ");
    Serial.print(Udp.remotePort()); Serial.print(" / ");
    Serial.print(packetSize);Serial.print(" / ");
    Serial.println(packetBuffer);
  }


  lv_task_handler();
  
  // Serial.println("");
  delay(5);

}

