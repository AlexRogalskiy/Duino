#include <SoftwareSerial.h>
SoftwareSerial Sim900Serial(2, 3);
String currStr = ""; //
String phone = ""; //
// True, если текущая строка является sms-сообщением
boolean isStringMessage = false;
void setup()
{
Serial.begin(19200);
Sim900Serial.begin(19200);
// Настраиваем приём сообщений с других устройств
Sim900Serial.print("AT+CMGF=1\r");
delay(300);
Sim900Serial.print("AT+IFC=1, 1\r");
delay(300);
Sim900Serial.print("AT+CPBS=\"SM\"\r");
delay(300);
Sim900Serial.print("AT+CNMI=1,2,2,1,0\r");
delay(500);
}
void loop()
{
if (!Sim900Serial.available())
return;
char currSymb = Sim900Serial.read();
if ('\r' == currSymb)
{
if (isStringMessage) // текущая строка - sms-сообщение,
{
if (!currStr.compareTo("temp")) // текст sms - temp
{
// отправить sms на приходящий номер
Sim900Serial.print("AT+CMGF=1\r");
delay(100);
Sim900Serial.print("AT + CMGS = \"");
Sim900Serial.print(phone);
Sim900Serial.println("\"");
delay(100);
double val = analogRead(A0); // чтение
double voltage = val*5.0/1024; // перевод в вольты
double temp = voltage*100 - 273.15; // в градусы Цельсия
Serial.println(temp);
Sim900Serial.println(temp);
delay(100);
Sim900Serial.println((char)26);
delay(100);
Sim900Serial.println();
}
Serial.println(currStr);
isStringMessage = false;
}
else
{
if (currStr.startsWith("+CMT")) {
Serial.println(currStr);
// выделить из сообщения номер телефона
phone=currStr.substring(7,19);
Serial.println(phone);
// если текущая строка начинается с "+CMT",
// то следующая строка является сообщением
isStringMessage = true;
}
}
currStr = "";
}
else if ('\n' != currSymb)
{
currStr += String(currSymb);
}
}
}

