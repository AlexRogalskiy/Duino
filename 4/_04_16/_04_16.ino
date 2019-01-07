#define INTERVAL_GET_DATA 2000  // интервала измерений, мс 
// пин, к которому подключен датчик
#define HCSR501PIN         18
// счетчик срабатыаний
int counterHCSR501=0;


// переменная для интервала измерений
unsigned long millis_int1=0;
 
void setup() {
  Serial.begin(9600);
  attachInterrupt(5,incCounterHCSR501,RISING);
}

void loop() {
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      // получение данных счетчика срабатываний HC-SR501
      int counter= get_data_counterHCSR501();
      // выводим значения газа в ppm
      Serial.print("counter=");
      Serial.println(counter);
      // старт интервала отсчета
      millis_int1=millis();
   }
}

// получение данных счетчика срабатываний HC-SR501
int get_data_counterHCSR501()  {
  // вернуть значение счетчика срабатываний HC-SR501
  return counterHCSR501;
}

void incCounterHCSR501() {
   // инкремент счетчика
   counterHCSR501=counterHCSR501+1;
}
