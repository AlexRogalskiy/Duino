#include <Servo.h> // подключение библиотеки Servo
Servo servo1;
const int pinServo=8; // пин для подключения сервопривода
int pos = 0; // переменная для хранения позиции сервопривода
int dir =1; // направление перемещения сервопривода
// Выводы для подключения HC-SR04 Trig - 12, Echo - 13
Ultrasonic ultrasonic(12, 13);
float dist_cm; // переменная для дистанции, см
// подключить динамик к pin 9
int speakerPin = 9;
void setup()
{
// подключить переменную servo1 к выводу pinServo1
servo1.attach(pinServo1);
pinMode(speakerPin, OUTPUT);
}
void loop()
{
servo1.write(pos); // поворот сервоприводов на полученный угол
delay(15); // пауза для ожидания поворота сервоприводов
float dist_cm = ultrasonic.Ranging(CM);
if(dist_cm<100 && dist_cm>20)
tone(speakerPin,); // включить пьезозуммер
else
{
tone(speakerPin,0); // отключить пьезозуммер
pos=pos+dir; // изменение переменной положения сервопривода
if(pos==0 || pos==180)
dir=dir*(-1); // изменение направления движения
}
}
