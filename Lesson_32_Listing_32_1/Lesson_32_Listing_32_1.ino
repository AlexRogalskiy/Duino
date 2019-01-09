// подключение библиотеки SoftwareSerial
#include <SoftwareSerial.h>
// номер телефона для отправки sms (поменяйте на свой)
#define PHONE "+79031111111"
// Выводы для SoftwareSerial (у вас могут быть 7,8)
SoftwareSerial Sim900Serial(2, 3);
const int lm335=A0; // для подключения LM335
unsigned long millis1;
void setup()
{
Sim900Serial(19200); // the Hardware serial rate
}
void loop()
{
if (millis()-millis1>30*60*1000) // прошло 30 минут?
{
SendTextMessage(); // отправить sms
millis1=millis();
}
}
// подпрограмма отправки sms
void SendTextMessage()
{
// AT-команда установки text mode
Sim900Serial.print("AT+CMGF=1\r");
delay(100);
// номер телефона получателя
Sim900Serial.println("AT + CMGS = \"");
Sim900Serial.println(PHONE);
Sim900Serial.println("\"");
delay(100);
// сообщение – данные температуры
double val = analogRead(lm335); // чтение
double voltage = val*5.0/1024; // перевод в вольты
double temp = voltage*100 - 273.15; // в градусы Цельсия
Sim900Serial.println(temp);
delay(100);
// ASCII код ctrl+z – окончание передачи
Sim900Serial.println((char)26);
delay(100);
Sim900Serial.println();
}
}

