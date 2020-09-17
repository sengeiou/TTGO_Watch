/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

Servo myservo;
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 13;

// REPLACE WITH YOUR RECEIVER MAC Address
// uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  
uint8_t broadcastAddress[] = {0x8C, 0xAA, 0xB5, 0x82, 0xEA, 0x58};    //TTGO_2020 白色

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


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// IPAddress ip(192, 168, 0, 20);
IPAddress ip(172, 25, 9, 30);  //XH
// IPAddress ip1(230, 1, 2, 3); 
IPAddress ip1(172,25,9,61);

unsigned int localPort = 2300;      // local port to listen on
int count = 0;
int count_temp = 0;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
byte  ReplyBuffer[128];        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

long Recv_Packet_Count = 0;

int data = 0;

bool Ethernet_Connect_Flag = false;

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

  for(int i=0; i<128; i++)
  {
    ReplyBuffer[i] = 0x00;
  }
  ReplyBuffer[96] = 0x54;

  pinMode(2, OUTPUT);

  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2500); // attaches the servo on pin 18 to the servo object

  #if 1
  Ethernet.init(5);

  // start the Ethernet
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    // while (true) {
    //   delay(1); // do nothing, no point running without Ethernet hardware
    // }
  }
  if (Ethernet.linkStatus() == LinkOFF) {  
    Serial.println("Ethernet cable is not connected.");
    Ethernet_Connect_Flag = false;
  }
  else if (Ethernet.linkStatus() == LinkON)
  {
    Ethernet_Connect_Flag = true;
  }
  
  if(Ethernet_Connect_Flag ==  true)
  {
    // start UDP
    Udp.begin(23302);
    // Udp.beginMulticast(ip1, 23302);
  }
  #endif
 
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

  myservo.write(180);

}
 


void loop() {

  count =  count + 1;
  count_temp =  count_temp + 1;

  myservo.write(recv_Data.b);

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

  #if 1
  if(Ethernet_Connect_Flag == true)
  {
    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize) {

      Recv_Packet_Count = Recv_Packet_Count + 1;

      //@-网络数据包信息
      // Serial.print("Received packet of size ");
      // Serial.println(packetSize);
      // Serial.print("From ");
      // IPAddress remote = Udp.remoteIP();
      // for (int i=0; i < 4; i++) {
      //   Serial.print(remote[i], DEC);
      //   if (i < 3) {
      //     Serial.print(".");
      //   }
      // }
      // Serial.print(", port ");
      // Serial.println(Udp.remotePort());

      // read the packet into packetBufffer
      // Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
      // Serial.println("Contents:");
      // Serial.println(packetBuffer);

      // myservo.write(packetBuffer[0]);

      // send a reply to the IP address and port that sent us the packet we received
  //    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  //    Udp.write(ReplyBuffer);
  //    Udp.endPacket();
    }
  }
  #endif


  if(count > 10)
  {
    count = 0;

    #if 1
    if(Ethernet_Connect_Flag == true)
    {
      // send a reply to the IP address and port that sent us the packet we received
      Udp.beginPacket(ip1, 23602);
      // Udp.write(ReplyBuffer);  //write str
      Udp.write(ReplyBuffer,128);  //write byte
      Udp.endPacket();  
    }
    #endif


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
  }

  if(count_temp > 200)
  {
    count_temp = 0;

    Serial.print("Recv:");
    Serial.println(Recv_Packet_Count);
  }
  
  delay(5);
}
