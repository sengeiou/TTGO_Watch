//@-ttgo配置
#include "config.h"
#include <WiFi.h>
#include <AsyncUDP.h>

const char * ssid = "DX_JS";
const char * password = "dingxiao";

AsyncUDP udp;

char buf[20];   // buffer to hold the string to append

// AsyncUDPPacket send_packet;

void setup()
{
    Serial.begin(115200);

    for(int i=0; i<20; i++)
    buf[i] = i;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }
    if(udp.listenMulticast(IPAddress(224,100,23,200), 6000)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
            Serial.print("UDP Packet Type: ");
            Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", To: ");
            Serial.print(packet.localIP());
            Serial.print(":");
            Serial.print(packet.localPort());
            Serial.print(", Length: ");
            Serial.print(packet.length());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            //reply to the client
            // packet.printf("Got %u bytes of data", packet.length());
        });
        //Send multicast
        // udp.print("Hello!");
    }
}

void loop()
{
    delay(100);
    //Send multicast
    // udp.print("Anyone here?");
    // udp.printf("Anyone here?");
    // udp.broadcast("Anyone here?");
    // udp.write((const uint8_t*)buf, 20);
    udp.broadcastTo("Anyone here?", 6000);

}