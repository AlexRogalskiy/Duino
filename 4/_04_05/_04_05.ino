#define INTERVAL_GET_DATA 2000  // интервала измерений, мс
#define SOILMOISTUREPIN A8      // пин подключения контакта A0
// значение полного полива
#define MINVALUESOILMOISTURE 220
// значение критической сухости
#define MAXVALUESOILMOISTURE 900

// переменная для интервала измерений
unsigned long millis_int1=0;

void setup(void) {
   // запуск последовательного порта
   Serial.begin(9600);
}
 
void loop(void) {
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      // получение данных c датчика SoilMoisture
      float  moisture= get_data_soilmoisture();
      // вывод в монитор последовательного порта
      Serial.print("soilmoisture =");Serial.println(moisture);
      Serial.println(" %");
      // старт интервала отсчета
      millis_int1=millis();
   }
}

// получение данных с датчика Soil Moisture
float get_data_soilmoisture()  {
  // получение значения с аналогового вывода датчика
  int avalue=analogRead(SOILMOISTUREPIN);
  // масштабируем значение в проценты
  Serial.print("avalue=");Serial.println(avalue);
  avalue=constrain(avalue, MINVALUESOILMOISTURE,MAXVALUESOILMOISTURE);
  int moisture=map(avalue, MINVALUESOILMOISTURE,
                   MAXVALUESOILMOISTURE,100,0);
  return (float)moisture;
}


