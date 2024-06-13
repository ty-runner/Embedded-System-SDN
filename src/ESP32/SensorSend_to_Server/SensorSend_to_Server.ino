#include "DHT.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <arpa/inet.h>

#define DHTPIN 23
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

//tuck into a enc config file at some point or something like that
const char* ssid = "MountVernon";
const char* password = "DG860A9EC502";
char broadcast[INET_ADDRSTRLEN];

WiFiUDP udp;
const int localPort = 5005;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();
  randomSeed(analogRead(0));
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Broadcast address: ");
  calculate_broadcast_address(WiFi.localIP().toString().c_str(), WiFi.subnetMask().toString().c_str(), broadcast);
  Serial.println(broadcast);

  udp.begin(localPort);
  Serial.printf("UDP server started on port %d\n", localPort);

}
void calculate_broadcast_address(const char *ip, const char *mask, char *broadcast) {
    struct in_addr ip_addr, mask_addr, broadcast_addr;

    // Convert IP address and subnet mask from string to in_addr
    inet_aton(ip, &ip_addr);
    inet_aton(mask, &mask_addr);

    // Calculate broadcast address
    broadcast_addr.s_addr = ip_addr.s_addr | ~mask_addr.s_addr;

    // Convert broadcast address back to string
    strcpy(broadcast, inet_ntoa(broadcast_addr));
}
long rand_num(){
    return (random(1, 99999));
}
void parse_packet(char* packet, long int* isn, char** message){
  const char* isnKey = "";
  const char* msgKey = "";

  const char* isnPos = strstr(packet, isnKey);
  const char* msgPos = strstr(packet, msgKey);

  if (isnPos && msgPos){
    isnPos += strlen(isnKey);
    *isn = atoi(isnPos);

    msgPos += strlen(msgKey);
    *message = strdup(msgPos);
  }
  else{
    *isn = -1;
    *message = strdup("Invalid packet");
  }

}
bool three_way_handshake(char* broadcast, const int localPort){
    /*
    1. Send SYN packet broadcast
    2. Listen for SYN_ACK from server
    3. Respond to SYN_ACK with ACK
    */
   //1.
    char msg[23] = "SYN"; // + ISN
    char isn_str[20] = "";
    char* message;
    long isn;
    sprintf(isn_str, "%ld", rand_num());
    strcat(msg, isn_str);
    udp.beginPacket(broadcast, localPort);
    udp.write((const uint8_t*)msg, strlen(msg));
    udp.endPacket();
    Serial.println(isn_str);
    delay(2000);
  //2.
  //Listen for incoming UDP packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[1024];
    int len = udp.read(incomingPacket, 1024);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    Serial.printf("Received packet from %s: %s\n", udp.remoteIP().toString().c_str(), incomingPacket);
    //split incoming packet into SYN-ACK, ISN
    //incomingPacket, long ISN = split_packet(incomingPacket);
    parse_packet(incomingPacket, &isn, &message);
    Serial.println(message);
    Serial.println(isn);
    if(strcmp(message, "SYN-ACK") == 0){
      //3.
      Serial.println("Received SYN-ACK");
      char* msg = "ACK";
      char isn_str[20] = "";
      sprintf(isn_str, "%ld", isn + 1);
      strcat(msg, isn_str);
      udp.beginPacket(broadcast, localPort);
      udp.write((const uint8_t*)msg, strlen(msg));
      udp.endPacket();
      return 1;
    }
    return 0;
  }
  return 0;
}
void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  t = t * 1.8 + 32;
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *F");
  if(three_way_handshake(broadcast, localPort)){
    Serial.println("Link established.");
  }
}
