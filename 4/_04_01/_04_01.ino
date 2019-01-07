// подключение библиотеки DHT
#include "DHT.h"
// константы
#define DHTPIN 22           // пин подключения контакта DATA
#define DHTTYPE DHT22      // датчик DHT 11
#define INTERVAL_GET_DATA 2000  // интервала измерений, мс
// создание экземпляра объекта DHT
DHT dht(DHTPIN,DHTTYPE);
// переменная для интервала измерений
unsigned long millis_int1=0;

void setup() {
   Serial.begin(9600);  // запуск последовательного порта
   dht.begin();         // запуск DHT      
}

void loop() {
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      // получение данных c DHT11
      int humidity = get_data_humidity();
      // вывод в монитор последовательного порта
      Serial.print("humidity=");
      Serial.println(humidity);
      // старт интервала отсчета
      millis_int1=millis();
   }
}

int get_data_humidity() {
   int h = dht.readHumidity();
   return h;
}
