#include <Servo.h> // подключение библиотеки Servo
Servo servo1, servo2;
const int pinServo1=8; // Пин для подключения 1 сервопривода
const int pinServo2=9; // Пин для подключения 2 сервопривода
// переменные для хранения углов поворота сервоприводов
int angleServo1,angleServo2 = 0;
const int axisX=A0; // ось Х подключена к A0
const int axisY=A1; // ось Y подключена к A1
int valX, valY = 0; // переменные для хранения значений осей
void setup()
{
// подключить переменную servo1 к выводу pinServo1
servo1.attach(pinServo1);
// подключить переменную servo2 к выводу pinServo2
Servo2.attach(pinServo2);
}
void loop()
{
valX = analogRead(axisX); // значение оси Х
valY = analogRead(axisY); // значение оси Y
// масштабируем значение к интервалу 0-180
angleServo1=map(valX,0,1023,0,180);
angleServo2=map(valY,0,1023,0,180);
// поворот сервоприводов на полученный угол
servo1.write(angleServo1);
servo2.write(angleServo2);
delay(15); // пауза для ожидания поворота сервоприводов
}
