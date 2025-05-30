#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
#define RST_PIN D1 // Reset pin
#define SS_PIN D2 // SDA pin
MFRC522 rfid(SS_PIN, RST_PIN);
// WiFi credentials
const char* ssid = "Galaxy";
const char* password = "ashish@123";
WiFiClient client;
Servo servo;
// Server details
const char* serverUrl = "http://192.168.130.191:3000/rfid_data"; // Replace with your server IP
and endpoint
void uthao()
{
 for(int i=0;i<90;i++)
 {
 servo.write(i);
 delayMicroseconds(10);
 Serial.println("inside servo2");
 }
}
void girao()
{
 for(int i=90;i>0;i--)
 {
 servo.write(i);
 delayMicroseconds(10);
 Serial.println("inside servo");
 }
}
void setup() {
 Serial.begin(115200);
 SPI.begin();
 rfid.PCD_Init();
 servo.attach(D3);
 servo.write(0);

 WiFi.begin(ssid, password);
 Serial.print("Connecting to WiFi");
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("\nConnected to WiFi");
 Serial.print("NodeMCU IP Address: ");
 Serial.println(WiFi.localIP());
}
void loop() {
 if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
 delay(500);
 return;
 }
 String rfidTag = "";
 for (byte i = 0; i < rfid.uid.size; i++) {
 rfidTag += String(rfid.uid.uidByte[i], HEX);
 }
 rfidTag.toUpperCase();
 Serial.print("RFID Tag: ");
 Serial.println(rfidTag);
 if (WiFi.status() == WL_CONNECTED) {
 HTTPClient http;
 http.begin(client,serverUrl);
 http.addHeader("Content-Type", "application/json");
 String payload = "{\"rfid\":\"" + rfidTag + "\"}";
 int httpResponseCode = http.POST(payload);
 if (httpResponseCode == 200) {
 String response = http.getString();
 Serial.println("Server Response: " + response);
 Serial.println("authorization succcessful");
 uthao();
 delay(3000);
 girao();
 }else if(httpResponseCode == 201){
 String response = http.getString();
 Serial.println("Server Response: " + response);
 Serial.println("authorization failed");
 }else if(httpResponseCode == 202){
 String response = http.getString();
 Serial.println("Server Response: " + response);
 uthao();
 delay(3000);
 girao();
 }
 else {
 Serial.println("Error in sending POST request");
 }
 http.end();
 } else {
 Serial.println("WiFi not connected");
 }
 delay(3000);
}