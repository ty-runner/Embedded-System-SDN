#include <WiFi.h>
#include <WiFiUdp.h>
#include "DHT.h"

#define DHTPIN 23
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
const char* ssid = "MountVernon";
const char* password = "DG860A9EC502";

WiFiUDP udp;
const int localPort = 5005;
const IPAddress broadcastIP(10, 0, 0, 255);

void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(10);

  // Connect to Wi-Fi network
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
  Serial.println(broadcastIP);

  udp.begin(localPort);
  Serial.printf("UDP server started on port %d\n", localPort);
}

void loop() {
  // Broadcast a UDP packet
  const char* message = "Hello, peers!";
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  t = t * 1.8 + 32;
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *F");
  udp.beginPacket(broadcastIP, localPort);
  udp.write((const uint8_t*)message, strlen(message));
  udp.endPacket();

  // Listen for incoming UDP packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    Serial.printf("Received packet from %s: %s\n", udp.remoteIP().toString().c_str(), incomingPacket);

    // Echo the message back
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write((const uint8_t*)incomingPacket, strlen(incomingPacket));
    udp.endPacket();
  }

  delay(10000); // Delay to avoid flooding
}
