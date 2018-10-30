#include <GPRS_Shield_Arduino.h>
#include <TroykaDHT.h>

// длина сообщения
#define MESSAGE_LENGTH 1

#define DHTPIN2 9

#define DHTTYPE1 DHT22

#define DHTPIN1 8

#define DHTTYPE2 DHT21

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



DHT dht1(DHTPIN1, DHTTYPE1);
DHT dht2(DHTPIN2, DHTTYPE2);

//SoftwareSerial gprsSerial(10, 11);
 
// создаём объект класса GPRS и передаём в него объект Serial1 
//GPRS gprs(gprsSerial);
// можно указать дополнительные параметры — пины PK и ST
// по умолчанию: PK = 2, ST = 3
GPRS gprs(Serial);

unsigned long previousMillis = 0;
const long interval = 60000;

float t1;
float t2;
 
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

  dht1_init();
  dht2_init();
  
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

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    t1 = getTemp1();
    t2 = getTemp2();
  }
  
  // если пришло новое сообщение
  if (gprs.ifSMSNow()) {
    // читаем его
    gprs.readSMS(smsmessage, phone, datetime);
    //Serial.println(smsmessage);

    if(strcmp(smsmessage, "0") == 0) {

      //convert float temperature to char array for sending
      String messageObj = String ("Dom: ") + String (t1) + String ("\r\nPodval: ") + String (t2);
      char message[30];
      messageObj.toCharArray(message, 30);

//      Serial.print(message);

//      send sms with temperature
      gprs.sendSMS(PHONE_NUMBER, message);
    }
  }
}

float getTemp1()
{
  dht1.read();
  switch(dht1.getState()) {
    // всё OK
    case DHT_OK:
      // выводим показания влажности и температуры
//      Serial.print("Temperature = ");
//      Serial.print(dht1.getTemperatureC());
//      Serial.println(" C \t");
//      Serial.print("Humidity = ");
//      Serial.print(dht1.getHumidity());
//      Serial.println(" %");
        return dht1.getTemperatureC();
      break;
    // ошибка контрольной суммы
    case DHT_ERROR_CHECKSUM:
//      Serial.println("Checksum 1 error");
      dht1_init();
      return 99.99;
      break;
    // превышение времени ожидания
    case DHT_ERROR_TIMEOUT:
//      Serial.println("Time out 1 error");
      dht1_init();
      return 99.99;
      break;
    // данных нет, датчик не реагирует или отсутствует
    case DHT_ERROR_NO_REPLY:
//      Serial.println("Sensor 1 not connected");
      dht1_init();
      return 99.99;
      break;
  }
//  delay(2000);
}

float getTemp2()
{
  dht2.read();
  switch(dht2.getState()) {
    // всё OK
    case DHT_OK:
      // выводим показания влажности и температуры
//      Serial.print("Temperature = ");
//      Serial.print(dht2.getTemperatureC());
//      Serial.println(" C \t");
//      Serial.print("Humidity = ");
//      Serial.print(dht2.getHumidity());
//      Serial.println(" %");
      return dht2.getTemperatureC();
      break;
    // ошибка контрольной суммы
    case DHT_ERROR_CHECKSUM:
//      Serial.println("Checksum 2 error");
      dht2_init();
      return 99.99;
      break;
    // превышение времени ожидания
    case DHT_ERROR_TIMEOUT:
//      Serial.println("Time out 2 error");
      dht2_init();
      return 99.99;
      break;
    // данных нет, датчик не реагирует или отсутствует
    case DHT_ERROR_NO_REPLY:
//      Serial.println("Sensor 2 not connected");
      dht2_init();
      return 99.99;
      break;
  }
//  delay(2000);
}

void dht1_init(void)
{
  dht1.begin();
}

void dht2_init(void)
{
  dht2.begin();
}
