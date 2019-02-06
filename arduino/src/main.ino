#include "../include/secret.h"

#include <Arduino.h>

#include <Hash.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

#include <DHT.h>

#define DHTPIN 5

#define DHTTYPE    DHT21

DHT dht(DHTPIN, DHTTYPE);

ESP8266WiFiMulti WiFiMulti;

int temp = 99;

const char * headerKeys[] = {"Date", "Server"} ;
const size_t numberOfHeaders = 2;

char buffer[100];

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  dht.begin();

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(AP, AP_PASS);

}

void loop() {
  String timestamp = getTimestamp();
  sendData(timestamp, getTemp(), getHum());
  delay(60000);
}

float getTemp() {
  delay(500);
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return 999;
  }
  return t;
}

float getHum() {
  delay(500);
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return 999;
  }
  else {
    return h;
  }
}

String getTimestamp() {

  String headerDate = "";

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    // WiFiClient client;

    HTTPClient http;

    http.setTimeout(20000);

    Serial.print("[HTTP] begin...\n");

    // configure traged server and url
    String url = "http://" + String(DB_IP) + ":" + DB_PORT;

    http.begin(url);
    http.setAuthorization(DB_LOG, DB_PASS);

    http.addHeader("Accept", "application/json");
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] GET...\n");
    
    http.collectHeaders(headerKeys, numberOfHeaders);
    
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.print("Payload: ");
        Serial.print(payload);
        
        headerDate = http.header("Date");
        Serial.print("HeaderDate: ");
        Serial.println(headerDate);
        Serial.println();
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      Serial.println();
    }
    
    http.end();
    
    return headerDate;
  }
}

void sendData(String timestamp, float temp, float hum) {

  if ((timestamp == "") || (temp > 100) || (hum > 100)) {
    return;
  }

  // wait for WiFi connection
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    // WiFiClient client;

    HTTPClient http;

    http.setTimeout(20000);

    Serial.print("[HTTP] begin...\n");

    // configure traged server and url
    String url = "http://" + String(DB_IP) + ":" + DB_PORT + "/smarthome/" + String(sha1(timestamp.c_str()));

    http.begin(url);
    http.setAuthorization(DB_LOG, DB_PASS);

    http.addHeader("Accept", "application/json");
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] PUT...\n");

    String data = String("{\"timestamp\":\"") + timestamp + String("\",\"temperature\":\"") + temp + String("\",\"humidity\":\"") + hum + String("\"}");

//    sprintf(buffer, "Aconcagua is %d metres height.", height);

    Serial.print("Data to PUT: ");
    Serial.println(data.c_str());
    
    // start connection and send data
    int httpCode = http.PUT(data);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] PUT... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    Serial.println();
    http.end();
  }
}
