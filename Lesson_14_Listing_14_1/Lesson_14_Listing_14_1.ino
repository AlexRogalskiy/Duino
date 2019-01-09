const int BLUE=9; // Вывод BLUE RGB-светодиода
const int GREEN=10; // Вывод GREEN RGB-светодиода
const int RED=11; // Вывод RED RGB-светодиода
const int lm335=A0; // для подключения LM335
int MIN_T=20; // Нижний порог
int MAX_T=30; // Верхний порог
int val = 0;
void setup()
{
// конфигурируем выводы светодиоды как OUTPUT
pinMode(RED,OUTPUT);
pinMode(GREEN,OUTPUT);
pinMode(BLUE,OUTPUT);
}
void loop()
{
double val = analogRead(lm335); // чтение
double voltage = val*5.0/1024; // перевод в вольты
double temp = voltage*100 - 273.15; // в градусы Цельсия
Serial.print(" temp = ");
Serial.println(temp);
if(temp < MIN_T) // синий цвет RGB-светодиода
setRGB(0,0,1);
else if(temp > MIN_T) // красный цвет RGB-светодиода
setRGB(1,0,0);
else // желтый цвет RGB-светодиода
setRGB(1,0,0);
delay(1000); // пауза перед следующим измерением
}
// установка цвета RGB-светодиода
void setRGB(int r, int g, int b)
{
digitalWrite(RED,r);
digitalWrite(GREEN,g);
digitalWrite(BLUE,b);
}
