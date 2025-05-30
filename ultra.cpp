#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
// WiFi credentials
const char* ssid = "Galaxy";
const char* password = "ashish@123";
WiFiClient client;
const int trigPin1 = D1;
const int trigPin2 = D5;
const int echoPin1 = D2;
const int echoPin2 = D6;
const char* serverUrl1 = "http://192.168.130.191:3000/rfid_data";
const char* serverUrl2 = "http://192.168.130.191:3000/emptyslotsdata";
int slot1OccupationStatus = 0;
int slot2OccupationStatus = 0;
float checkslot1()
{
 digitalWrite(trigPin1,LOW);
 delayMicroseconds(2);
 digitalWrite(trigPin1,HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin1,LOW);
 float duration = pulseIn(echoPin1,HIGH);
 float distance = duration * 0.034/2;
 Serial.println(distance);
 return distance;
}
float checkslot2()
{
 digitalWrite(trigPin2,LOW);
 delayMicroseconds(2);
 digitalWrite(trigPin2,HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin2,LOW);
 float duration = pulseIn(echoPin2,HIGH);
 float distance = duration * 0.034/2;
 Serial.println(distance);
 return distance;
}
void setup() { pinMode(trigPin1,OUTPUT);
 pinMode(trigPin2,OUTPUT);
 pinMode(echoPin1,INPUT);
 pinMode(echoPin2,INPUT);

 Serial.begin(115200);
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
 if(checkslot1()>30 && slot1OccupationStatus==1)
 {
 slot1OccupationStatus = 0;
 if (WiFi.status() == WL_CONNECTED) {
 HTTPClient http;
 http.begin(client,serverUrl2);
 http.addHeader("Content-Type", "application/json");
 String id = "1";
 String payload = "{\"_id\":\"" + id + "\", \"occupation_status\":\"" + slot1OccupationStatus +
"\"}";
 // Send HTTP POST request
 int httpResponseCode = http.POST(payload);
 if (httpResponseCode == 200) {
 String response = http.getString();
 Serial.println("Server Response: " + response);

 }else if(httpResponseCode == 201){
 String response = http.getString();

 }
 else {
 Serial.println("Error in sending POST request");
 }
 http.end();
 } else {
 Serial.println("WiFi not connected");
 }
 }
delay(2000);
if(checkslot1()<30 && slot1OccupationStatus == 0)
 {
 slot1OccupationStatus=1;
 if (WiFi.status() == WL_CONNECTED) {
 HTTPClient http;
 http.begin(client,serverUrl2);
 http.addHeader("Content-Type", "application/json");
 String id2 = "1";
 String payload = "{\"_id\":\"" + id2 + "\", \"occupation_status\":\"" + slot1OccupationStatus +
"\"}";
 int httpResponseCode = http.POST(payload);
 if (httpResponseCode == 200) {
 String response = http.getString();
 Serial.println("Server Response: " + response);
 }else if(httpResponseCode == 201){
 String response = http.getString();
 //Serial.println("Server Response: " + response);
 //Serial.println("authorization failed");
 }
 else {
 Serial.println("Error in sending POST request");
 }
 http.end();
 } else {
 Serial.println("WiFi not connected");
 }

 }
 delay(2000);
 if(checkslot2()>30 && slot2OccupationStatus == 1)
 {
 slot2OccupationStatus=0;

 if (WiFi.status() == WL_CONNECTED) {
 HTTPClient http;
 http.begin(client,serverUrl2);
 http.addHeader("Content-Type", "application/json");
 // Prepare JSON payload
 String id2 = "2";
 String payload = "{\"_id\":\"" + id2 + "\", \"occupation_status\":\"" + slot2OccupationStatus +
"\"}";
 // Send HTTP POST request
 int httpResponseCode = http.POST(payload);
 if (httpResponseCode == 200) {
 String response = http.getString();
 Serial.println("Server Response: " + response);
 }else if(httpResponseCode == 201){
 String response = http.getString();

 }
 else {
 Serial.println("Error in sending POST request");
 }
 http.end();
 } else {
 Serial.println("WiFi not connected");
 }
 }
 delay(2000);
 if(checkslot2()<30 && slot2OccupationStatus == 0)
 {
 slot2OccupationStatus=1;
 if (WiFi.status() == WL_CONNECTED) {
 HTTPClient http;
 http.begin(client,serverUrl2);
 http.addHeader("Content-Type", "application/json");
 // Prepare JSON payload
 String id2 = "2";
 String payload = "{\"_id\":\"" + id2 + "\", \"occupation_status\":\"" + slot2OccupationStatus +
"\"}";
 // Send HTTP POST request
 int httpResponseCode = http.POST(payload);
 if (httpResponseCode == 200) {
 String response = http.getString();
 Serial.println("Server Response: " + response);
 }else if(httpResponseCode == 201){
 String response = http.getString();
 }
 else {
 Serial.println("Error in sending POST request");
 }
 http.end();
 } else {
 Serial.println("WiFi not connected");
 }
 }
 delay(2000);
}