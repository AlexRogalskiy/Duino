#define INTERVAL_GET_DATA 2000  // интервала измерений, мс 
// пин, к которому подключен датчик огня flame sensor
#define FLAMEPIN         A12

// переменная для интервала измерений
unsigned long millis_int1=0;
 
void setup() {
  // открываем последовательный порт
  Serial.begin(9600);
}
 
void loop() {
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      // получение данных c датчика mq7
      float  flame= get_data_flame();
      // выводим значения flame sensor
      Serial.print("flame=");
      Serial.print(flame);
      // старт интервала отсчета
      millis_int1=millis();
   }
}

// получение данных с датчика flame sensor
float get_data_flame()  {

  // получение значения 
  float value=analogRead(FLAMEPIN);
  
  return (float)value;
}
