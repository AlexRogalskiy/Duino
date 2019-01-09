// список выводов Arduino для подключения к разрядам a-g
// семисегментного индикатора
int pins[8]={9,13,4,6,7,10,3,5};
// значения для вывода цифр 0-9
byte numbers[10] = { B11111100, B01100000, B11011010,
B11110010, B01100110, B10110110,
B10111110, B11100000, B11111110,
B11110110};
// переменная для хранения и обработки текущего значения
int number=0;
int number1=0;
int number2=0;
// семисегментного индикатора
int pindigits[4]={2,8,11,12};
// переменная для хранения текущего разряда
int digit=0;
// для отмеривания 100 мс
unsigned long millis1=0;
// режим 1 - секундомер работает
mode=0;
const int BUTTON=14; // Контакт 14(A0) для подключения кнопки
int tekButton = LOW; // Переменная для сохранения текущего состояния кнопки
int prevButton = LOW; // Переменная для сохранения предыдущего состояния
// к нопки
boolean ledOn = false; // Текущее состояние светодиода (включен/выключен)
void setup()
{
// Сконфигурировать контакт кнопки как вход
pinMode (BUTTON, INPUT);
// Сконфигурировать контакты как выходы
for(int i=0;i<8;i++)
pinMode(pins[i],OUTPUT);
for(int i=0;i<4;i++)
{pinMode(pindigits[i],OUTPUT);
digitalWrite(pindigits[i],HIGH);
}
}
void loop()
{
tekButton = debounce(prevButton);
if (prevButton == LOW && tekButton == HIGH) // если нажатие...
{
mode=1-mode; // изменение режима
if(mode==1)
number=0;
}
if(millis()-millis1>=100 && mode==1)
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
for(int i=0;i<8;i++)
{
if(bitRead(numbers[num],7-i)==HIGH) // зажечь сегмент
digitalWrite(pins[i],HIGH);
else // потушить сегмент
digitalWrite(pins[i],LOW);
}
if(dig==1) // десятичная точка для второго разряда
digitalWrite(pins[7],HIGH);
}
// Функция сглаживания дребезга. Принимает в качестве
// аргумента предыдущее состояние кнопки и выдает фактическое.
boolean debounce(boolean last)
{
boolean current = digitalRead(BUTTON); // Считать состояние кнопки,
if (last != current) // если изменилось...
{
d elay(5); // ж дем 5 м с
current = digitalRead(BUTTON); // считываем состояние кнопки
return current; // возвращаем состояние кнопки
}
}
