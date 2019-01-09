// подключение библиотек для RTC
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
// подключение библиотеки для lcd
#include <LiquidCrystal.h>
// инициализация с указанием контактов подключения
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);
// строка, собираемая из данных, приходящих в последовательный порт
String inputString = "";
boolean stringComplete = false; // флаг комплектности строки
void setup()
{
Serial.begin(9600); // запустить последовательный порт
lcd.begin(16, 2); // установить размерность дисплея
}
void loop()
{
tmElements_t tm;
// ожидание конца строки для анализа поступившего запроса:
if (stringComplete)
{
tm.Day=(int(inputString[0])-48)*10+(int(inputString[1])-48);
tm.Month=(int(inputString[3])-48)*10+(int(inputString[4])-48);
tm.Year=CalendarYrToTm((int(inputString[6])-
48)*1000+(int(inputString[7])-48)*100+
(int(inputString[8])-48)*10+(int(inputString[9])-48));
tm.Hour=(int(inputString[11])-48)*10+(int(inputString[12])-48);
tm.Minute=(int(inputString[14])-48)*10+(int(inputString[15])-48);
tm.Second=(int(inputString[17])-48)*10+(int(inputString[18])-48);
RTC.write(tm); // записать время в RTC
// очистить строку
inputString = "";
stringComplete = false;
}
if (RTC.read(tm))
{
print2digits(tm.Hour,0,0);
lcd.print(":");
print2digits(tm.Minute,3,0);
lcd.print(":");
print2digits(tm.Second,6,0);
print2digits(tm.Day,0,1);
lcd.print("/");
print2digits(tm.Month,3,1);
lcd.print("/");
lcd.print(tmYearToCalendar(tm.Year));
}
else
{
if (RTC.chipPresent())
{
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("DS1307 is stopped");
}
else
{
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("DS1307 read error");
}
delay(9000);
}
delay(1000);
}
// процедура вывода на дисплей с добавлением до двух цифр
void print2digits(int number,int col, int str)
{
lcd.setCursor(col, str);
if (number >= 0 && number < 10)
{lcd.print("0");}
lcd.print(number);
}
// получение данных по последовательному порту
void serialEvent()
{
while (Serial.available())
{ // получить очередной байт:
char inChar = (char)Serial.read();
// добавить в строку
inputString += inChar;
// /n - конец передачи
if (inChar == '\n')
{stringComplete = true;}
}
}
