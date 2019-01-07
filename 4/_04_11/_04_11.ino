// библиотека для работы с датчиками MQ 
#include <TroykaMQ.h>

#define INTERVAL_GET_DATA 2000  // интервала измерений, мс 
// аналоговый пин
#define MQ2PIN         A0
// создаём объект для работы с датчиком
MQ2 mq2(MQ2PIN);

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
  // выбор входа мультиплексора CD4051 – y2 (010)
  digitalWrite(14,LOW);
  digitalWrite(13,HIGH);
  digitalWrite(15,LOW);    
  // калибровка
  mq2.calibrate();
  mq2.getRo();
}
 
void loop() {
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      // выбор входа мультиплексора CD4051 – y2 (010)
      digitalWrite(14,LOW);
      digitalWrite(13,HIGH);
      digitalWrite(15,LOW);    
      // получение данных c датчика mq2
      float  propan= get_data_ppmpropan();
      // выводим значения газа в ppm
      Serial.print("propan=");
      Serial.print(propan);
      Serial.println(" ppm ");
      float  methan= get_data_ppmmethan();
      // выводим значения газа в ppm
      Serial.print("methan=");
      Serial.print(methan);
      Serial.println(" ppm ");
      float  smoke= get_data_ppmsmoke();
      // выводим значения газа в ppm
      Serial.print("smoke=");
      Serial.print(smoke);
      Serial.println(" ppm ");
      // старт интервала отсчета
      millis_int1=millis();
   }
}

// получение данных содержания пропана с датчика MQ2
float get_data_ppmpropan()  {

  Serial.println(mq2.readRatio());
  // получение значения 
  float value=mq2.readLPG();
  
  return value;
}
// получение данных содержания метана с датчика MQ2
float get_data_ppmmethan()  {

  Serial.println(mq2.readRatio());
  // получение значения 
  float value=mq2.readMethane();
  
  return value;
}
// получение данных содержания дыма с датчика MQ2
float get_data_ppmsmoke()  {

  Serial.println(mq2.readRatio());
  // получение значения 
  float value=mq2.readSmoke();
  
  return value;
}

