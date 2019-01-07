// библиотека для работы с датчиками MQ 
#include <TroykaMQ.h>

#define INTERVAL_GET_DATA 2000  // интервала измерений, мс 
// аналоговый пин
#define MQ7PIN         A0
// создаём объект для работы с датчиком
MQ7 mq7(MQ7PIN);

// переменная для интервала измерений
unsigned long millis_int1=0;
 
void setup() {
  // входы подключения к мультиплексору D5, D7, D8 (GPIO 14, 13, 15) 
  // как OUTPUT
  pinMode(14,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(15,OUTPUT);  
  // открываем последовательный порт
  Serial.begin(9600);
  // выбор входа мультиплексора CD4051 – y3 (011)
  digitalWrite(14,HIGH);
  digitalWrite(13,HIGH);
  digitalWrite(15,LOW);    
  // калибровка
  mq7.calibrate();
  mq7.getRo();
}
 
void loop() {
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      // выбор входа мультиплексора CD4051 – y3 (011)
      digitalWrite(14,HIGH);
      digitalWrite(13,HIGH);
      digitalWrite(15,LOW);     
      // получение данных c датчика mq7
      float  carbonmonoxide= get_data_ppmcarbonmonoxide();
      // выводим значения газа в ppm
      Serial.print("carbonmonoxide=");
      Serial.print(carbonmonoxide);
      Serial.println(" ppm ");
      // старт интервала отсчета
      millis_int1=millis();
   }
}

// получение данных с датчика MQ7
float get_data_ppmcarbonmonoxide()  {
  Serial.println(mq7.readRatio());
  // получение значения 
  float value=mq7.readCarbonMonoxide();
  return value;
}

