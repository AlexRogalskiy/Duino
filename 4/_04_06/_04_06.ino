// список пинов для подключения к s0, s1, s2 мультиплексора
// D5, D7, D8 (GPIO 14, 13, 15)
int pins[]={14, 13, 15};
// Массив двоичных числ определяющих номер выбранного входа/выхода 
// микросхемы 4051, с 1 по 8. 
int bin [] = { B000, B001, B010, B011, B100, B101, B110, B111 } ;
// служебные переменные
int row;
int r0 = 0;     
int r1 = 0;     
int r2 = 0;
int  avalue =0;

void setup(void) {
   // входы подключения к мультиплексору как OUTPUT
   for(int i=0;i<3;i++) {
      pinMode(pins[i],OUTPUT);
   }
   // запуск последовательного порта
   Serial.begin(9600);
}

void loop(void) {
   for(int i=0;i<8;i++) {
      // выбор входа мультиплексора      
      row = bin [i] ;      
      r0 = row & 0x01 ;      
      r1 = (row >> 1) & 0x01 ; // 
      r2 = (row >> 2) & 0x01 ; // 
      digitalWrite (pins[i], r0) ;      
      digitalWrite (pins[i], r1) ;      
      digitalWrite (pins[i], r2) ;      
      // получение данных c A0
      avalue= analogRead(A0);
      // вывод в монитор последовательного порта
      Serial.print("analog input =");Serial.print(i);
      Serial.println(" = "); Serial.println(avalue);
   }
   // пауза
   delay(2000);
}

