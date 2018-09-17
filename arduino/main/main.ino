// библиотека для работы с GPRS устройством
#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include "DHT.h"

// длина сообщения
#define MESSAGE_LENGTH 1

#define DHTPIN 12

#define DHTTYPE DHT21

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

SoftwareSerial gprsSerial(10, 11);
 
// создаём объект класса GPRS и передаём в него объект Serial1 
//GPRS gprs(gprsSerial);
// можно указать дополнительные параметры — пины PK и ST
// по умолчанию: PK = 2, ST = 3
GPRS gprs(gprsSerial, 2, 3);
 
void setup()
{
  // открываем последовательный порт для мониторинга действий в программе
  Serial.begin(9600);
  // ждём пока не откроется монитор последовательного порта
  // для того, чтобы отследить все события в программе
  while (!Serial) {
  }
  Serial.print("Serial init OK\r\n");

  dht.begin();
  // открываем Serial-соединение с GPRS Shield
  gprsSerial.begin(9600);
  // включаем GPRS-шилд
  gprs.powerOn();
  // проверяем, есть ли связь с GPRS-устройством
  while (!gprs.init()) {
    // если связи нет, ждём 1 секунду
    // и выводим сообщение об ошибке;
    // процесс повторяется в цикле,
    // пока не появится ответ от GPRS-устройства
    delay(1000);
    Serial.print("GPRS Init error\r\n");
  }
  // выводим сообщение об удачной инициализации GPRS Shield
  Serial.println("GPRS init success");
  Serial.println("Please send SMS message to me!");
}
 
void loop()
{
  // если пришло новое сообщение
  if (gprs.ifSMSNow()) {
    // читаем его
    gprs.readSMS(smsmessage, phone, datetime);
    Serial.println(smsmessage);

    if(strcmp(smsmessage, "0") == 0) {

      //read temperature from dht21
      delay(1000);
      float t = dht.readTemperature();
    
      // Check if any reads failed and exit early (to try again).
      if (isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }


      //convert float temperature to char array for sending
      String messageObj = String (t);
      char message[5];
      messageObj.toCharArray(message, 5);

      //send sms with temperature
      gprs.sendSMS(PHONE_NUMBER, message);
    }
  }
}
