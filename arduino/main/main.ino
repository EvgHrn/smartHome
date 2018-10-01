// библиотека для работы с GPRS устройством
#include <GPRS_Shield_Arduino.h>
//#include <SoftwareSerial.h>
#include "DHT.h"

// длина сообщения
#define MESSAGE_LENGTH 1

#define DHTPIN 12

#define DHTTYPE DHT21

#define DHTPIN2 13

#define DHTTYPE2 DHT22

// номер на который будем отправлять сообщение
#define PHONE_NUMBER  "+79090520560"
 
// номер сообщения в памяти сим-карты
int messageIndex = 0;
 
// текст сообщения
char smsmessage[MESSAGE_LENGTH];
// номер, с которого пришло сообщение
char phone[16];
// дата отправки сообщения
char datetime[24];



DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE2);

//SoftwareSerial gprsSerial(10, 11);
 
// создаём объект класса GPRS и передаём в него объект Serial1 
//GPRS gprs(gprsSerial);
// можно указать дополнительные параметры — пины PK и ST
// по умолчанию: PK = 2, ST = 3
GPRS gprs(Serial);
GPRS gprs2(Serial);
 
void setup()
{
//  pinMode(A5, INPUT_PULLUP);
  // открываем последовательный порт для мониторинга действий в программе
  Serial.begin(9600);
  
  // ждём пока не откроется монитор последовательного порта
  // для того, чтобы отследить все события в программе
  while (!Serial) {
  }
  //Serial.print("Serial init OK\r\n");

  dht.begin();
  dht2.begin();
  // открываем Serial-соединение с GPRS Shield
//  gprsSerial.begin(9600);
  // включаем GPRS-шилд
  gprs.powerOn();
  // проверяем, есть ли связь с GPRS-устройством
  while (!gprs.init()) {
    // если связи нет, ждём 1 секунду
    // и выводим сообщение об ошибке;
    // процесс повторяется в цикле,
    // пока не появится ответ от GPRS-устройства
    delay(1000);
    //Serial.print("GPRS Init error\r\n");
  }
  // выводим сообщение об удачной инициализации GPRS Shield
  //Serial.println("GPRS init success");
  //Serial.println("Please send SMS message to me!");
}
 
void loop()
{
  // если пришло новое сообщение
  if (gprs.ifSMSNow()) {
    // читаем его
    gprs.readSMS(smsmessage, phone, datetime);
    //Serial.println(smsmessage);

    if(strcmp(smsmessage, "0") == 0) {

      //read temperature from dht21
      delay(1000);
      float t1 = dht.readTemperature();
      delay(1000);
      float t2 = dht2.readTemperature();
      // Check if any reads failed and exit early (to try again).
      if (isnan(t1)) {
        //Serial.println("Failed to read from DHT sensor!");
        t1 = 99.99;
      }

      if (isnan(t2)) {
        //Serial.println("Failed to read from DHT sensor!");
        t2 = 99.99;
      }

      //convert float temperature to char array for sending
      String messageObj = String ("t1: ") + String (t1) + String (" t2: ") + String (t2);
      char message[20];
      messageObj.toCharArray(message, 20);

//      Serial.print(message);

//      send sms with temperature
      gprs.sendSMS(PHONE_NUMBER, message);
    }
  }
}
