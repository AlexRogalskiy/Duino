// Подключение библиотеки
#include <LiquidCrystal.h>
// инициализация с указанием контактов подключения
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
const int LM335=A0; // для подключения LM335
void setup() {
// установить размерность дисплея
lcd.begin(16, 2);
}
void loop()
{
double val = analogRead(LM335); // чтение
double voltage = val*5.0/1024; // перевод в вольты
// вывод значения в Кельвинах
lcd.setCursor(2,0);
lcd.print("Tk="); lcd.print(voltage*100); lcd.print("K");
double temp = voltage*100 - 273.15; // в градусы Цельсия
// вывод значения в градусах Цельсия
lcd.setCursor(2,1);
lcd.print("Tc="); lcd.print(temp); lcd.print("");
delay(1000); // пауза перед следующим измерением
}

