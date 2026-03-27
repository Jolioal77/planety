#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h> 
#include "DHT.h"

const char* ssid = "Enko_Students";
const char* password = "Enk0@234";

#define DHTPIN 27         
#define DHTTYPE DHT11     
#define SOIL_PIN 32       
#define LIGHT_PIN 33      

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

void handleData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
  int soilRaw = analogRead(SOIL_PIN);
  int lightRaw = analogRead(LIGHT_PIN);

  
  Serial.print("Raw Soil: "); Serial.print(soilRaw);
  Serial.print(" | Raw Light: "); Serial.println(lightRaw);

  
  float s = map(soilRaw, 3200, 1000, 0, 100); 
  s = constrain(s, 0, 100); 

  
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
  Serial.print("Connecting to Planety Network");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n--- SYSTEM ONLINE ---");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); 

  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}
