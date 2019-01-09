// Подключение библиотеки
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// PIN 7 - RST Pin 1 on LCD
// PIN 6 - CE Pin 2 on LCD
// PIN 5 - DC Pin 3 on LCD
// PIN 4 - DIN Pin 4 on LCD
// PIN 3 - CLK Pin 5 on LCD
Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 6, 7);
const int LIGHT=A0; // Контакт A0 для входа фоторезистора
const int MIN_LIGHT=200; // Нижний порог освещенности
const int MAX_LIGHT=900; // Верхний порог освещенности
// Переменная для хранения данных фоторезистора
int val1,val2 = 0;
void setup()
{
display.begin();
// установить контраст фона экрана
// очень важный параметр!
display.setContrast(60);
display.clearDisplay(); // очистить экран
delay(2000);
}
void loop()
{
val1 = analogRead(LIGHT); // Чтение показаний фоторезистора
drawText(val1,1); // вывести текст
// масштабирование значения потенциометра к 0–75
val2= map(val1, MIN_LIGHT, MAX_LIGHT, 0, 75);
// вывод черного прямоугольника в %
display.fillRect(5, 25, val2, 10, 1);
// вывод белой части прямоугольника
display.fillRect(5+val2,25, 75-val2, 10, 0);
display.display();
delay(1000); // пауза перед новым измерением
drawText(val1,2); // стереть текст
}
// процедура вывода текста
void drawText(unsigned long num,int color)
{
display.setTextSize(2); // размер шрифта
display.setCursor(20,5); // позиция курсора
if(color==1)
display.setTextColor(BLACK); // вывести значение
else
display.setTextColor(WHITE); // стереть (белым по белому)
display.print(num);
}
