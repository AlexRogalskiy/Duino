// список выводов Arduino для подключения к разрядам a-g
// семисегментного индикатора
int pins[7]={2,3,4,5,6,7,8};
// значения для вывода цифр 0-9
byte numbers[10] = { B11111100, B01100000, B11011010, B11110010, B01100110,
B10110110, B10111110, B11100000, B11111110, B11100110};
// переменная для хранения значения текущей цифры
int number=0;
void setup()
{
// Сконфигурировать контакты как выходы
for(int i=0;i<7;i++)
pinMode(pins[i],OUTPUT);
}
void loop()
{
showNumber(number);
delay(1000);
number=(number+1)%10;
}
// функция вывода цифры на семисегментный индикатор
void showNumber(int num)
{
for(int i=0;i<7;i++)
{
if(bitRead(numbers[num],7-i)==HIGH) // зажечь сегмент
digitalWrite(pins[i],HIGH);
else // потушить сегмент
digitalWrite(pins[i],LOW);
}
}
