#define INTERVAL_GET_DATA 2000  // интервала измерений, мс 
// аналоговый пин
#define FLAMEPIN         A0

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
}
 
void loop() {
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      // выбор входа мультиплексора CD4051 – y4 (100)
      digitalWrite(14,LOW);
      digitalWrite(13,LOW);
      digitalWrite(15,HIGH);     
      // получение данных c датчика mq7
      float  flame= get_data_flame();
      // выводим значения flame sensor
      Serial.print("flame=");
      Serial.println(flame);
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

