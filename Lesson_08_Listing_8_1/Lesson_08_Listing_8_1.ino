// Подключение библиотеки SPI
#include <SPI.h>
// пин SS
int pin_spi_ss=8;
// значения для вывода цифр 0-9
byte numbers[10] = { B11111100, B01100000, B11011010,
B11110010, B01100110, B10110110,B10111110, B11100000, B11111110,B11110110};
// переменная для хранения значения текущей цифры
int number=0;
int number1=0;
int number2=0;
// семисегментного индикатора
int pindigits[4]={4,5,6,7};
// переменная для хранения текущего разряда
int digit=0;
//
unsigned long millis1=0;
void setup()
{
SPI.begin();
// Сконфигурировать контакты как выходы
pinMode(pin_spi_ss,OUTPUT);
for(int i=0;i<4;i++)
{pinMode(pindigits[i],OUTPUT);
digitalWrite(pindigits[i],HIGH);
}
}
void loop()
{
if(millis()-millis1>=100)
{millis1=millis1+100;
number=number+1;
if(number==10000)
number=0;
}
number1=number;
for(int i=0;i<4;i++)
{
  number2=number1%10;
number1=number1/10;
showNumber(number2,i);
for(int j=0;j<4;j++)
digitalWrite(pindigits[j],HIGH);
digitalWrite(pindigits[i],LOW);
delay(1);
}
}
// функция вывода цифры на семисегментный индикатор
void showNumber(int num,int dig)
{
byte maska;
digitalWrite(pin_spi_ss,LOW);
if(dig==1) maska=1;
else maska=0;
SPI.transfer(numbers[num]+maska);
digitalWrite(pin_spi_ss,HIGH);
}
}

