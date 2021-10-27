

#include <Wire.h>
#include <U8g2lib.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

//#include <WiFi.h>


//@-按键
#define BLDC_OUT_U            5
#define BLDC_OUT_V            18
#define BLDC_OUT_W            19


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
uint8_t txValue = 0;
char  BLE_Msg[256];                      //BLE接收缓存区
char  BLE_SendMsg[256];                  //BLE发送缓存区


int dir = 0;
int dir_count = 0;


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

//  Serial.println(WiFi.macAddress());

  //@-BLDC控制
  pinMode(BLDC_OUT_U, OUTPUT);
  pinMode(BLDC_OUT_V, OUTPUT);
  pinMode(BLDC_OUT_W, OUTPUT);

//  digitalWrite(BLDC_OUT_U, HIGH); 
//  digitalWrite(BLDC_OUT_V, LOW); 
//  digitalWrite(BLDC_OUT_W, LOW); 


  // //@-鸣音器控制-PWM_Channel PWM_Ferquency PWM_Resoution
  // ledcSetup(0, 700, 8);
  // ledcAttachPin(Tone_Pin, 0);
  // ledcWrite(0, 0);


  // Create the BLE Device
  BLEDevice::init("ESP32_BLDC");

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

}

 
void loop() {

    char temp[20];

    Serial.println(dir_count);

    dir_count = dir_count + 1;
    if(dir_count > 500)
    {
      dir_count = 0;
      if(dir == 0)
      dir = 1;
      else if(dir == 1)
      dir = 0; 
    }

    if(dir == 0)
    {
      digitalWrite(BLDC_OUT_U, LOW);
      digitalWrite(BLDC_OUT_V, HIGH);
      delay(10);
      digitalWrite(BLDC_OUT_U, LOW);
      digitalWrite(BLDC_OUT_W, HIGH);
      delay(10);
      digitalWrite(BLDC_OUT_V, LOW);
      digitalWrite(BLDC_OUT_W, HIGH);
      delay(10);
      digitalWrite(BLDC_OUT_V, LOW);
      digitalWrite(BLDC_OUT_U, HIGH);
      delay(10);
      digitalWrite(BLDC_OUT_W, LOW);
      digitalWrite(BLDC_OUT_U, HIGH);
      delay(10);
      digitalWrite(BLDC_OUT_W, LOW);
      digitalWrite(BLDC_OUT_V, HIGH);
      delay(10);
    }
    else if(dir == 1)
    {
      digitalWrite(BLDC_OUT_W, LOW);
      digitalWrite(BLDC_OUT_V, HIGH);
      delay(10);
      digitalWrite(BLDC_OUT_W, LOW);
      digitalWrite(BLDC_OUT_U, HIGH);
      delay(10);
      digitalWrite(BLDC_OUT_V, LOW);
      digitalWrite(BLDC_OUT_U, HIGH);
      delay(10);
      digitalWrite(BLDC_OUT_V, LOW);
      digitalWrite(BLDC_OUT_W, HIGH);
      delay(10);
      digitalWrite(BLDC_OUT_U, LOW);
      digitalWrite(BLDC_OUT_W, HIGH);
      delay(10);
      digitalWrite(BLDC_OUT_U, LOW);
      digitalWrite(BLDC_OUT_V, HIGH);
      delay(10);
    }
    
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
    
}
