#include <OneWire.h>
OneWire ds(10); // линия 1-Wire будет на pin 10
#include <LiquidCrystal.h>
// инициализация с указанием контактов подключения
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
void setup(void)
{
Serial.begin(9600);
// установить размерность дисплея
lcd.begin(16, 2);
lcd.clear();
}
void loop(void)
{
int t=get_temp();
lcd.setCursor(0,1);lcd.print(" ");
lcd.setCursor(0,1);
lcd.print(t/16);lcd.print(".");lcd.print((t%16)*100/16);
}
// получение данных с датчика DS18B20
int get_temp()
{
byte i;
byte present = 0;
byte data[12];
byte addr[8];
int Temp;
if ( !ds.search(addr))
{Serial.print("No more addresses.\n");
ds.reset_search();
return -1;
}
// вывод в монитор уникального адреса 1-Wire устройства
lcd.setCursor(0,0);
lcd.print("R=");
for( i = 0; i < 8; i++)
{lcd.print(addr[i], HEX);lcd.print(" ");}
if ( OneWire:crc8( addr, 7) != addr[7])
{
Serial.print("CRC is not valid!\n");
return -1;
}
if ( addr[0] != 0x28)
{
Serial.print("Device is not a DS18S20 family device.\n");
return -1;
}
ds.reset();
// запустить конвертацию температуры датчиком
ds.write(0x44,1);
delay(750); // ждем 750 мс
present = ds.reset();
ds.select(addr);
ds.write(0xBE); /
// считываем ОЗУ датчика
for ( i = 0; i < 9; i++)
{ data[i] = ds.read();}
Temp=(data[1]<<8)+data[0];
return Temp;
}
