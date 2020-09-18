#include "TridentTD_ESP32NVS.h"

int Rec_Num = 100;
int Pro_Mode = 1;
int Pro_SetTime = 60;
int Pro_RealTime = 2;
float Pro_Kgp = 112.56;



void setup() {
      
  char temp[50];

  Serial.begin(115200); Serial.println();

  Serial.println("[ESP32] NVS start");
  NVS.begin();
  
//  Serial.println("[NVS] set object to NVS");
//  uint8_t mac[6] = {0xDF, 0xEE, 0x10, 0x49, 0xA1, 0x42};
//  NVS.setObject( "MAC", &mac, sizeof(mac) );  // จัดเก็บค่า mac[6] ลงที่ key ชื่อ "MAC" บน NVS

//  Serial.println("[NVS] get object from NVS");
//  uint8_t* mymac;
//  mymac = (uint8_t*) NVS.getObject( "MAC");   // อ่านค่า key ชื่อ "MAC" บน NVS มาเก็บไว้ที่ตัวแปร mymac
//  
//  Serial.printf("mac : %02X:%02X:%02X:%02X:%02X:%02X\n", 
//                 mymac[0],mymac[1],mymac[2],mymac[3],mymac[4],mymac[5]);

//  String  ssid        = "wuyiyi"; 
//  String  password    = "10238831";
//  NVS.setString("SSID", ssid);
//  NVS.setString("PASSWORD", password);

// String  ssid_t        = NVS.getString("SSID");
// String  password_t    = NVS.getString("PASSWORD");

// Serial.printf("ssid        : %s\n", ssid_t.c_str());
// Serial.printf("password    : %s\n", password_t.c_str());


  //@-Blood安全功能

  

  // for(int id=1; id<100; id++)
  // {
  //   sprintf(temp, "M%d_ST%d_RKgp%.2f_RT%d", Pro_Mode,Pro_SetTime,Pro_Kgp,Pro_RealTime);
  //   NVS.setString("Rec"+id, String(temp));
  //   Rec_Num = id;
  // }

  // NVS.setString("Rec_Num", String(Rec_Num));


  String  rec_num_str   = NVS.getString("Rec_Num");
  String  rec_num = String("Rec"+Rec_Num);
  String  rec_data_str = NVS.getString(rec_num);

  Serial.printf("RecNum        : %s\n", rec_num_str.c_str());
  Serial.printf("RecData    : %s\n", rec_data_str.c_str());

  NVS.close();

//  NVS.eraseAll();
  



}

void loop() {
  delay(1);
}
