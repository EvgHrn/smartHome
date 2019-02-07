# 1 "c:\\users\\pc123~1\\appdata\\local\\temp\\tmpjwnlyw"
#include <Arduino.h>
# 1 "C:/smartHome/arduino/src/main.ino"
#include "../include/tft.h"

#include "../include/secret.h"



#include <Arduino.h>

#include <Hash.h>

#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>



#include <DHT.h>



#define DHTPIN 5

#define DHTTYPE DHT21



DHT dht(DHTPIN, DHTTYPE);



int temp = 99;



const char * headerKeys[] = {"Date", "Server"} ;

const size_t numberOfHeaders = 2;



char buffer[100];
void setup();
void loop();
float getTemp();
float getHum();
String getTimestamp();
void sendData(String timestamp, float temp, float hum);
#line 47 "C:/smartHome/arduino/src/main.ino"
void setup() {



  Serial.begin(115200);



  dht.begin();



  Serial.println();

  Serial.println();

  Serial.println();



  for (uint8_t t = 4; t > 0; t--) {

    Serial.printf("[SETUP] WAIT %d...\n", t);

    Serial.flush();

    delay(1000);

  }



  WiFi.setAutoConnect(true);

  WiFi.mode(WIFI_STA);



  tftSetup();

}



void loop() {

  String timestamp = getTimestamp();

  float temp = getTemp();

  float hum = getHum();

  tftShowData(timestamp, temp, hum);

  sendData(timestamp, temp, hum);

  delay(60000);

}



float getTemp() {

  float t = dht.readTemperature();

  if (isnan(t)) {

    Serial.println(F("Failed to read from DHT sensor!"));

    tftShowInfo("Failed to read from DHT sensor!", 1);

    return 999;

  }

  Serial.print(F("Temperature: "));

  Serial.println(t);

  return t;

}



float getHum() {

  float h = dht.readHumidity();

  if (isnan(h)) {

    Serial.println(F("Failed to read from DHT sensor!"));

    tftShowInfo("Failed to read from DHT sensor!", 1);

    return 999;

  }

  Serial.print(F("Humidity: "));

  Serial.println(h);

  return h;

}



String getTimestamp() {

  String headerDate = "";



  WiFi.begin(AP, AP_PASS);

  while (WiFi.status() != WL_CONNECTED) {

    Serial.print(".");

    Serial.print(WiFi.status());

    tftShowInfo("WiFi disconnect", 1);

    delay(1000);

  }



  if (WiFi.status() == WL_CONNECTED) {

    tftShowInfo("WiFi connected", 1);



    HTTPClient http;



    http.setTimeout(20000);



    Serial.print("[HTTP] begin...\n");





    String url = "http://" + String(DB_IP) + ":" + DB_PORT;



    http.begin(url);

    http.setAuthorization(DB_LOG, DB_PASS);



    http.addHeader("Accept", "application/json");

    http.addHeader("Content-Type", "application/json");



    Serial.print("[HTTP] GET...\n");



    http.collectHeaders(headerKeys, numberOfHeaders);





    int httpCode = http.GET();





    if (httpCode > 0) {



      Serial.printf("[HTTP] GET code: %d\n", httpCode);

      sprintf(buffer,"%s%i", "HTTP GET code: ", httpCode);

      tftShowInfo(buffer, 1);





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

      sprintf(buffer,"%s%i", "HTTP GET fauled with error: ", httpCode);

      tftShowInfo(buffer, 1);

    }



    http.end();



    return headerDate;

  }

}



void sendData(String timestamp, float temp, float hum) {



  if ((timestamp == "") || (temp > 100) || (hum > 100)) {

    tftShowInfo("Wrong data", 1);

    return;

  }

  WiFi.begin(AP, AP_PASS);



  while (WiFi.status() != WL_CONNECTED) {

    Serial.print(".");

    tftShowInfo("WiFi disconnect", 1);

    delay(1000);

  }

  if (WiFi.status() == WL_CONNECTED) {

    tftShowInfo("WiFi connected", 1);

    HTTPClient http;



    http.setTimeout(20000);



    Serial.print("[HTTP] begin...\n");





    String url = "http://" + String(DB_IP) + ":" + DB_PORT + "/smarthome/" + String(sha1(timestamp.c_str()));



    http.begin(url);

    http.setAuthorization(DB_LOG, DB_PASS);



    http.addHeader("Accept", "application/json");

    http.addHeader("Content-Type", "application/json");



    Serial.print("[HTTP] PUT...\n");



    String data = String("{\"timestamp\":\"") + timestamp + String("\",\"temperature\":\"") + temp + String("\",\"humidity\":\"") + hum + String("\"}");



    Serial.print("Data to PUT: ");

    Serial.println(data.c_str());





    int httpCode = http.PUT(data);





    if (httpCode > 0) {



      Serial.printf("[HTTP] PUT... code: %d\n", httpCode);

      sprintf(buffer,"%s%i", "HTTP PUT... code: ", httpCode);

      tftShowInfo(buffer, 1);





      if (httpCode == HTTP_CODE_OK) {

        String payload = http.getString();

        Serial.println(payload);

      }

    } else {

      Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());

      sprintf(buffer,"%s%i", "HTTP PUT... failed, error: ", httpCode);

      tftShowInfo(buffer, 1);

    }

    Serial.println();

    http.end();

  }

}