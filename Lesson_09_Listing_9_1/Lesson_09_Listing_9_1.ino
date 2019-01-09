// подключение библиотеки SPI
#include <SPI.h>
int ss_pin=8; // пин SS
int pos=0; //
int offfigure=0; // текущая фигура для отображения
unsigned long millis1=0;
// массив с данными фигур для отображения
byte figure[2][8]={
{B10011001,B10011001,B10011001,B10000001,B10000001,B10011001,B10011001,B10011001},
{B10101010,B10101010,B10101010,B10101010,B10101010,B10101010,B10101010,B10101010}
};
void setup()
{
SPI.begin();
// Сконфигурировать контакт SS как выход
pinMode(ss_pin, OUTPUT);
}
void loop()
{
digitalWrite(ss_pin, LOW);
// столбцы
SPI.transfer(B00000001<<pos);
// строки
SPI.transfer(figure[offfigure][pos]);
digitalWrite(ss_pin,HIGH); // вывести данные на выводы 74HC595
delay(1);
pos=(pos+1)%8;
if(millis()-millis1>3000) // через 3 секунды – новая фигура
{
offfigure=(offfigure+1)%2;
millis1=millis();
}
}
