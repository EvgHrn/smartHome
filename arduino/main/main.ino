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
  WiFiMulti.addAP("thl", "12349876");

}

void loop() {
//  // wait for WiFi connection
//  if ((WiFiMulti.run() == WL_CONNECTED)) {
//
//    WiFiClient client;
//
//    HTTPClient http;
//
//    Serial.print("[HTTP] begin...\n");
//    // configure traged server and url
//
//
//    http.begin(client, "http://user:pass@server/smarthome/first");
//
//    /*
//      // or
//      http.begin(client, "http://");
//      http.setAuthorization("guest", "guest");
//
//      // or
//      http.begin(client, "http://");
//      http.setAuthorization("Z3Vlc3Q6Z3Vlc3Q=");
//    */
//
//    http.addHeader("Accept", "application/json");
//    http.addHeader("Content-Type", "application/json");
//
//    Serial.print("[HTTP] PUT...\n");
//    // start connection and send HTTP header
//    int httpCode = http.PUT("{\"description\":\"An\"}");
//
//    // httpCode will be negative on error
//    if (httpCode > 0) {
//      // HTTP header has been send and Server response header has been handled
//      Serial.printf("[HTTP] PUT... code: %d\n", httpCode);
//
//      // file found at server
//      if (httpCode == HTTP_CODE_OK) {
//        String payload = http.getString();
//        Serial.println(payload);
//      }
//    } else {
//      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//    }
//
//    http.end();
//  }

  String timestamp = getTimestamp();

  sendData(timestamp, getTemp(), getHum());
  
  delay(10000);
}

float getTemp() {
  delay(500);
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return 999;
  }
  else {
    return t;
  }
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
  
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http;

    http.setTimeout(10000);

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url


    http.begin(client, "http://qrlogadmin:qrlogadminpass@77.220.212.42:5984");

    /*
      // or
      http.begin(client, "http://");
      http.setAuthorization("guest", "guest");

      // or
      http.begin(client, "http://");
      http.setAuthorization("Z3Vlc3Q6Z3Vlc3Q=");
    */

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
        
        String headerDate = http.header("Date");
        Serial.print("HeaderDate: ");
        Serial.println(headerDate);
        Serial.println();
        return headerDate;
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      Serial.println();
    }
    
    http.end();
  }
}

void sendData(String timestamp, float temp, float hum) {
    // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http;

    http.setTimeout(10000);

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url

    String url = String("http://name:pass@serv/smarthome/") + String(sha1(timestamp.c_str()));

    http.begin(client, url.c_str());

    /*
      // or
      http.begin(client, "http://");
      http.setAuthorization("guest", "guest");

      // or
      http.begin(client, "http://");
      http.setAuthorization("Z3Vlc3Q6Z3Vlc3Q=");
    */

    http.addHeader("Accept", "application/json");
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] PUT...\n");

    String data = String("{\"timestamp\":\"") + timestamp + String("\",\"temperature\":\"") + temp + String("\",\"humidity\":\"") + hum + String("\"}");

//    sprintf(buffer, "Aconcagua is %d metres height.", height);

    Serial.print("Data to PUT: ");
    Serial.println(data.c_str());
    
    // start connection and send data
    int httpCode = http.PUT(data.c_str());

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
