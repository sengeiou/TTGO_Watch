// #include <BTstack.h>
// #include <stdio.h>
// #include <SPI.h>

// /* EXAMPLE_START(iBeacon): iBeacon Simulator
//  *
//  * @section Setup
//  *
//  * @text After BTstack.setup(), iBeaconConfigure() configures BTstack
//  * to send out iBeacons Advertisements with the provided Major ID,
//  * Minor ID and UUID.
//  */
// /* LISTING_START(iBeaconSetup): iBeacon Setup */
// UUID uuid("E2C56DB5-DFFB-48D2-B060-D0F5A71096E0");
// void setup(void){
//     Serial.begin(115200);
//     BTstack.setup();
//     BTstack.iBeaconConfigure(&uuid, 4711, 2);
//     BTstack.startAdvertising();
// }
// /* LISTING_END(iBeaconSetup) */

// void loop(void){
//     BTstack.loop();
// }



// #include "BluetoothSerial.h"

// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

// BluetoothSerial SerialBT;

// void setup() {
//   Serial.begin(115200);
//   SerialBT.begin("ESP32test"); //Bluetooth device name
//   Serial.println("The device started, now you can pair it with bluetooth!");
// }

// void loop() {
//   if (Serial.available()) {
//     SerialBT.write(Serial.read());
//   }
//   if (SerialBT.available()) {
//     Serial.write(SerialBT.read());
//   }
//   delay(20);
// }


/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara
   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8
   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.
   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.
*/

#include <Wire.h>
#include <U8g2lib.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "dx_lcd_map.h"


//@-按键
#define Button1_Pin            32
#define Button2_Pin            33
#define Button3_Pin            27
#define Button4_Pin            26
//@-光感和测距供用同一I2C总线
#define VL53L0X_BH1750_SCL_Pin 22
#define VL53L0X_BH1750_SDA_Pin 21
//@-OLED的I2C总线
#define OLED_SCL_Pin           5
#define OLED_SDA_Pin           18
//@-系统功能Pin
#define Power_Ctl_Pin          2
#define LED1_Pin               17   //设备告警指示LED
#define LED2_Pin               32   //充电控制指示LED
#define BAT_Pin                36
#define Tone_Pin               23
#define Check_DX1_Pin          14
#define Check_DX2_Pin          12

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


//@-创建显示设备
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ OLED_SCL_Pin, /* data=*/ OLED_SDA_Pin, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED
// U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ OLED_SCL_Pin, /* data=*/ OLED_SDA_Pin, /* reset=*/ U8X8_PIN_NONE);

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
uint8_t txValue = 0;
char  BLE_Msg[256];                      //BLE接收缓存区
char  BLE_SendMsg[256];                  //BLE发送缓存区

//@-键盘相关
int Power_Button_State = 0;     //@-电源键状态
int Power_PressOFF_Tick = 0;       //@-电源键长按状态
bool Power_Press_Close_Flag = false; //@-电源键启动关机功能标志
int USB_Button_State = 1;       //@-充电状态
int USB_Button_State_Copy = 1;  //@-充电状态Copy
bool USB_Button_State_Change = false;  //@-充电状态改变
int  USB_Button_State_Change_ShowTick = 500; //@-充电状态改变展示时间tick
int None_Button_Press_Tick = 0; //@-没有按键按下     
int Button1_State = 0;          //@-按键1状态
int Button2_State = 0;          //@-按键2状态
int Button3_State = 0;          //@-按键3状态
int Button4_State = 0;          //@-按键4状态
int Button4_Press_Tick = 0;     //@-按键4长按状态监测
bool Button4_LongPress_Lock = false;  //@-按键4长按锁标志
bool Key_Flag = false;


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
  
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
                            
    //  Serial.println("DXDXDX");

      if( rxValue.length() < 256)
      {
          memset(BLE_Msg, 0, sizeof(BLE_Msg));
          for (int i = 0; i < rxValue.length(); i++)
          {
            BLE_Msg[i] = rxValue[i];
            // BLE_Msg_Input.setCharAt(i,rxValue[i]);
          }
      }

      // if(rxValue[0] == 0x01)
      // Serial.println("ok");

     }
};



void setup() {

  Serial.begin(115200);

  //@-接管电源控制
  pinMode(Power_Ctl_Pin, OUTPUT);
  digitalWrite(Power_Ctl_Pin, HIGH); 

  //@-用户LED
  pinMode(LED1_Pin, OUTPUT);
  pinMode(LED2_Pin, OUTPUT);

  //@-鸣音器控制-PWM_Channel PWM_Ferquency PWM_Resoution
  ledcSetup(0, 700, 8);
  ledcAttachPin(Tone_Pin, 0);
  ledcWrite(0, 0);

  //@-配置键盘上拉输入Pin
  pinMode(Check_DX1_Pin, INPUT|PULLUP);
  pinMode(Check_DX2_Pin, INPUT|PULLUP);
  pinMode(Button1_Pin, INPUT|PULLUP);
  pinMode(Button2_Pin, INPUT|PULLUP);
  pinMode(Button3_Pin, INPUT|PULLUP);
  pinMode(Button4_Pin, INPUT|PULLUP);


  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pCharacteristic->setCallbacks(new MyCallbacks()); 

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");

  //@5-初始化显示
  u8g2.begin();
  u8g2.enableUTF8Print();    // enable UTF8 support for the Arduino print() function
//  u8g2.setFont(u8g2_font_10x20_tn ); 
  u8g2.setFontDirection(0);

  u8g2.clearBuffer();

}

          // //@-告警方式选择
          // if(Alarm_Pro_Index == 0)
          // {
          //     digitalWrite(LED_Pin, HIGH); 
          //     ledcWrite(0, 30); 
          //     delay(80);
          //     digitalWrite(LED_Pin, LOW); 
          //     ledcWrite(0, 0); 
          // }
          // else if(Alarm_Pro_Index == 1)
          // {
          //     digitalWrite(LED_Pin, HIGH); 
          //     delay(80);
          //     digitalWrite(LED_Pin, LOW); 
          // }

//@-键盘扫描
void Button_Check()
{
    String Temp3 = "None";

//    touchRead(T0)

    Power_Button_State = digitalRead(Check_DX1_Pin);
    USB_Button_State = digitalRead(Check_DX2_Pin);
    Button1_State = digitalRead(Button1_Pin);
    Button2_State = digitalRead(Button2_Pin);
    Button3_State = digitalRead(Button3_Pin);
    Button4_State = digitalRead(Button4_Pin);


    if((Button1_State == LOW) && (Key_Flag == false))
    {
        Key_Flag = true;
        BLE_SendMsg[1] = 1;
        Serial.println("Button1\n");
    }
    else if((Button2_State == LOW) && (Key_Flag == false))
    {
        Key_Flag = true;
        BLE_SendMsg[1] = 2;
        Serial.println("Button2\n");
    }
    else if((Button3_State == LOW) && (Key_Flag == false))
    {
        Key_Flag = true;
        BLE_SendMsg[1] = 3;
        Serial.println("Button3\n");
    }
    else if((Button4_State == LOW) && (Key_Flag == false))
    {
        Key_Flag = true;
        BLE_SendMsg[1] = 4;
        Serial.println("Button4\n");
    }

    //@-所有功能按键均没有按下
    if((Button1_State == HIGH)&&(Button2_State == HIGH)&&(Button3_State == HIGH)&&(Button4_State == HIGH))
    {
        Key_Flag = false;
        BLE_SendMsg[1] = 0;
    }

}


void loop() {

    char temp[20];

    Button_Check();

    // notify changed value
    if (deviceConnected) {
        // pCharacteristic->setValue((uint8_t*)&value, 4);
        pCharacteristic->setValue((uint8_t*)&BLE_SendMsg, 256);
        pCharacteristic->notify();
        // Serial.println(value);
        // txValue++;
        BLE_SendMsg[0] = BLE_SendMsg[0] + 1;
        delay(100); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
    // // disconnecting
    // if (!deviceConnected && oldDeviceConnected) {
    //     delay(500); // give the bluetooth stack the chance to get things ready
    //     pServer->startAdvertising(); // restart advertising
    //     Serial.println("start advertising");
    //     oldDeviceConnected = deviceConnected;
    // }
    // // connecting
    // if (deviceConnected && !oldDeviceConnected) {
    //     // do stuff here on connecting
    //     oldDeviceConnected = deviceConnected;
    // }

    // // u8g2.clearBuffer();
    // u8g2.setFont( u8g2_font_6x13_t_hebrew ); //9PX
    // u8g2.setCursor(10, 30);
    // u8g2.print("dingxiao");
    // u8g2.sendBuffer();


    u8g2.clearBuffer();
    //@-设置字体
    u8g2.setFont( u8g2_font_t0_16b_tr ); //12px
    // u8g2.drawXBMP(0 , 3 , 41 , 23 , col_qidian);
    u8g2.drawXBMP(0 , 3 , 20 , 20 , col_logo23);
    u8g2.setCursor(60, 30);
    // sprintf(temp, "%.0f", BLE_Msg[0]); 
    sprintf(temp, "%d", BLE_Msg[0]); 
    u8g2.print(String(temp));
    // u8g2.print("dingxiao");
    u8g2.sendBuffer();

}