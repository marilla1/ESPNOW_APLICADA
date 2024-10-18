#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_wifi.h>
#include <Arduino.h>
#include "FS.h"
#include <WiFiClientSecure.h>
#include <LiquidCrystal_I2C.h> //lcd


// REPLACE WITH THE MAC Address of your receiver
//revisar
uint8_t broadcastAddress[] = {0xc8, 0x2e, 0x18 , 0x8f, 0x00, 0xf4};

//definiendo funciones
void validacion(float distancia);
void pantalla(float distancia, String msj);


//definicion de pin de led

const int led = 2;

//config de pantalla lcd
LiquidCrystal_I2C lcd(0x27,16,2);


// Structure to send and receive data
typedef struct struct_message {
  int id; // must be unique for each sender board
    // int x;
    // int y;
  float distance;
  int porcentaje;
} struct_message;

// Create instances of struct_message
struct_message myDataRec;
struct_message myDataSen;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
/*
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
*/

// Callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info_t *esp_now_info, const uint8_t *incomingData, int len) {
  memcpy(&myDataRec, incomingData, sizeof(myDataRec));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("ID: ");
  Serial.println(myDataRec.id);

  // Serial.print("X: ");
  // Serial.println(myDataRec.x);
  // Serial.print("Y: ");
  // Serial.println(myDataRec.y);

  Serial.print("Distance: ");
  Serial.println(myDataRec.distance);
  Serial.println();
  Serial.print("Porcentaje del Tanque: ");
  Serial.print(myDataRec.porcentaje);

}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //config de pin del led
  pinMode(led, OUTPUT);

  //config pantalla lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  // Register callback for sending data
  /*
  esp_now_register_send_cb(OnDataSent);
  */
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // Register callback for receiving data
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Set values to send
  // myDataSen.id = 1;        // Cambiado de myDataSen.a a myDataSen.id
  // myDataSen.x = 10;        // Ejemplo de valor para x
  // myDataSen.y = 20;        // Ejemplo de valor para y
  // myDataSen.dist = 5.5;    // Ejemplo de valor para dist

  // Send message via ESP-NOW
  /*
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myDataSen, sizeof(myDataSen));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  */

  validacion(myDataRec.distance);
  
  delay(500);
}



/////////////////////VALIDACION, PARA EL CASO EN QUE EL TANQUE ESTE LLENO, QUE SE DEBERA HACER//////////

void validacion(float distancia){

  if(distancia > 5){
    pantalla(distancia, "Llenando");
    digitalWrite(led, HIGH);
    
  }
  else{
    pantalla(distancia, "Llenado Detenido");
    digitalWrite(led, LOW);



  }


}


void pantalla(float distancia, String msj){
  if (distancia > 5) {

    // digitalWrite(ledPin, LOW);  // Enciende el LED 1 cuando la distancia es mayor a 150 cm
    lcd.print(distancia);
    lcd.print(" cm");
    lcd.setCursor(0,1); // column#4 and Row #1 
    lcd.print(msj);
    delay(500);
    lcd.clear();
  } 
  else {

    //digitalWrite(ledPin, HIGH);   // Apaga el LED 1
    
    lcd.print(distancia);
    lcd.print(" cm");
    lcd.setCursor(1,1); // column#4 and Row #1 
    lcd.print("Detener Llenado");
    
    delay(500);
    lcd.clear();



  }
}

