/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
// uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  
uint8_t broadcastAddress[] = {0x8C, 0xAA, 0xB5, 0x82, 0xEA, 0x58};    //TTGO_2020

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

// Create a struct_message called myData
struct_message send_Data;
struct_message recv_Data;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&recv_Data, incomingData, sizeof(recv_Data));
//   Serial.print("Bytes received: ");
//   Serial.println(len);
//   Serial.print("Char: ");
//   Serial.println(send_Data.a);
//   Serial.print("Int: ");
//   Serial.println(send_Data.b);
//   Serial.print("Float: ");
//   Serial.println(send_Data.c);
//   Serial.print("String: ");
//   Serial.println(send_Data.d);
//   Serial.print("Bool: ");
//   Serial.println(send_Data.e);
//   Serial.println();
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  pinMode(2, OUTPUT);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Serial.println(WiFi.macAddress());  //84:0D:8E:0B:B2:54


  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  // esp_now_register_send_cb(OnDataSent);

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 

int data = 0;

void loop() {
  // Set values to send
  strcpy(send_Data.a, "THIS IS A CHAR");
  // send_Data.b = random(1,20);
  data = data + 1;
  if(data > 500)
  data = 0;
  send_Data.b = data;

  send_Data.c = 1.2;
  send_Data.d = "Hello";
  send_Data.e = false;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));

   
  // if (result == ESP_OK) {
  //   Serial.println("Sent with success");
  // }
  // else {
  //   Serial.println("Error sending the data");
  // }

  if(recv_Data.e == false)
  {
    // led_flag = true;
    digitalWrite(2, HIGH);
  }
  else if(recv_Data.e == true)
  {
    // led_flag = false;
    digitalWrite(2, LOW);
  }

  delay(100);
}
