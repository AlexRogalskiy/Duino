#define INTERVAL_GET_DATA 2000  // интервала измерений, мс
#define SOILMOISTUREPIN A0      // аналоговый вход
// значение полного полива
#define MINVALUESOILMOISTURE 220
// значение критической сухости
#define MAXVALUESOILMOISTURE 900

// переменная для интервала измерений
unsigned long millis_int1=0;

void setup(void) {
   // входы подключения к мультиплексору D5, D7, D8 (GPIO 14, 13, 15) 
   // как OUTPUT
   pinMode(14,OUTPUT);
   pinMode(13,OUTPUT);
   pinMode(15,OUTPUT);  
   // запуск последовательного порта
   Serial.begin(9600);
}

void loop(void) {
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      // получение данных c датчика SoilMoisture
      float  moisture= get_data_soilmoisture();
      // вывод в монитор последовательного порта
      Serial.print("soilmoisture =");Serial.print(moisture);
      Serial.println(" %");
      // старт интервала отсчета
      millis_int1=millis();
   }
}
// получение данных с датчика SoilMoisture
float get_data_soilmoisture()  {
  // выбор входа мультиплексора CD4051 – y0 (000)
  digitalWrite(14,LOW);
  digitalWrite(13,LOW);
  digitalWrite(15,LOW);
  // получение значения с аналогового вывода датчика
  int avalue=analogRead(SOILMOISTUREPIN);
  Serial.print("avalue =");Serial.println(avalue);
  // масштабируем значение в проценты
  avalue=constrain(avalue, MINVALUESOILMOISTURE,MAXVALUESOILMOISTURE);
  int moisture=map(avalue, MINVALUESOILMOISTURE, MAXVALUESOILMOISTURE,100,0);
  return (float)moisture;
}

