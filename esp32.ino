#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h> 
#include "DHT.h"          

const char* ssid = "Enko_Students";    // Change this to YOUR Wifi Name
const char* password = "Enk0@234";     // Change this to YOUR Wifi Password

#define DHTPIN 27         
#define DHTTYPE DHT11     
#define SOIL_PIN 32       
#define LIGHT_PIN 33      

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

void handleData() {
  // 1. Read Raw Values
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
  int soilRaw = analogRead(SOIL_PIN);
  int lightRaw = analogRead(LIGHT_PIN);

  float s = map(soilRaw, 3200, 1500, 0, 100);
  s = constrain(s, 0, 100); // Keep it between 0 and 100%

  float l = map(lightRaw, 0, 4095, 0, 1000); 
  l = constrain(l, 0, 1000);

  if (isnan(h) || isnan(t)) {
    t = 0.0; h = 0.0;
  }

  StaticJsonDocument<200> doc;
  doc["t"] = t;
  doc["h"] = h;
  doc["s"] = s;
  doc["l"] = l;

  String jsonResponse;
  serializeJson(doc, jsonResponse);

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", jsonResponse);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  
  analogReadResolution(12);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n--- CONNECTED ---");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); 

  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}
