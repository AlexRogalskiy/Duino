#define INTERVAL_GET_DATA 2000  // интервал измерений, мс
#define LEVELWATERPIN A9      // пин подключения контакта S
// пороговое значение протечки 
#define LEVELWATER 100

// переменная для интервала измерений
unsigned long millis_int1=0;

void setup(void) {
   // запуск последовательного порта
   Serial.begin(9600);
}

void loop(void) {
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      // получение данных c датчика уровня воды
      float  levelwater= get_data_levelwater();
      // вывод в монитор последовательного порта
      Serial.print("levelwater =");Serial.println(levelwater);
      if(levelwater>LEVELWATER)
         Serial.println(" flood !!!");
      // старт интервала отсчета
      millis_int1=millis();
   }
}

// получение данных с датчика уровня воды
float get_data_levelwater()  {
  // получение значения с аналогового вывода датчика
  int avalue=analogRead(LEVELWATERPIN);
  return (float) avalue;
}
