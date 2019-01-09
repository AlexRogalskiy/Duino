#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // указываем пины rx и tx
void setup()
{
pinMode(2,INPUT);
pinMode(3,OUTPUT);
Serial.begin(9600);
mySerial.begin(9600);
}
void loop()
{
if (mySerial.available())
{
int c = mySerial.read(); // читаем из software-порта
Serial.write(c); // пишем в hardware-порт
if (Serial.available())
{
int c = Serial.read(); // читаем из hardware-порта
mySerial.write(c); // пишем в software-порт
}
}
