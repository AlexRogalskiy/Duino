#define INTERVAL_GET_DATA 2000  // интервала измерений, мс
#define DS18B20PIN 23           // пин подключения контакта DATA

#include <OneWire.h>

OneWire ds(DS18B20PIN);


// переменная для интервала измерений
unsigned long millis_int1=0;

void setup(void) {
   Serial.begin(9600);
}

void loop(void) {
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      // получение данных c DS18b20
      float temp = get_data_ds18b20();
      // вывод в монитор последовательного порта
      if(temp<100) {
        Serial.print("temp=");Serial.println(temp);
      }
      // старт интервала отсчета
      millis_int1=millis();
   }
}
// получение данных с датчика DS18B20
float get_data_ds18b20()  {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  int Temp;
  float fTemp=0.0;
  
  if ( !ds.search(addr)) {
    Serial.print("No more addresses.\n");
    ds.reset_search();
    return 999;
  }
  // вывод в монитор уникального адреса 1-Wire устройства
  for( i = 0; i < 8; i++)  {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }
  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.print("CRC is not valid!\n");
    return 999;
  }
  if ( addr[0] != 0x28) {
    Serial.print("Device is not a DS18S20 family device.\n");
    return 999;
  }
  ds.reset();
  ds.select(addr);
  // запустить конвертацию температуры датчиком
  ds.write(0x44,1);
  delay(750); // ждем 750 мс
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); 
  // считываем ОЗУ датчика
  for ( i = 0; i < 9; i++) { 
    data[i] = ds.read();
  }
  Temp=((data[1]<<8)+data[0]);
  fTemp=1.0*Temp/16+(float(Temp%16))*1.0/16;

  return fTemp;
}
