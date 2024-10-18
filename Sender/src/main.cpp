
#include <WiFi.h>
#include <esp_wifi.h>
#include <Arduino.h>
#include "FS.h"
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <esp_now.h>

// REPLACE WITH THE RECEIVER'S MAC Address
//MAC Servidor> c8:2e:18:f0:01:f0
uint8_t broadcastAddress[] = {0xc8, 0x2e, 0x18, 0xf0, 0x01, 0xf0};


//Definicion de pines del sensor ultrasonico
const int trigPin = 13;
const int echoPin = 12;
const double soundSpeed = 0.034;
const int dataTxTimeInterval = 500; //ms
long dist;


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id; // must be unique for each sender board
    int x;
    int y;
    float distance = 10.0;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Configuracion de pines del ultrasonico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  // Set values to send
  myData.id = 1;
  myData.x = random(0,50);
  myData.y = random(0,50);

  // Send message via ESP-NOW
  Serial.println(myData.id);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(10000);
}