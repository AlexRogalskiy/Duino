// библиотека для работы с датчиками MQ 
#include <TroykaMQ.h>

#define INTERVAL_GET_DATA 2000  // интервала измерений, мс 
// пин, к которому подключен датчик
#define MQ7PIN         A11
// создаём объект для работы с датчиком
MQ7 mq7(MQ7PIN);

// переменная для интервала измерений
unsigned long millis_int1=0;
 
void setup() {
  // открываем последовательный порт
  Serial.begin(9600);
  // запускаем термоцикл
  mq2.calibrate();
  mq2.getRo();
}
 
void loop() {
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
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
