#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX, TX
const int LM335=A0; // контакт подключения датчика LM335
#define SSID "MacBook Pro - Petin" // введите ваш SSID
#define PASS "19101966" // введите ваш пароль
#define DST_IP "92.39.235.156" // naronmon.ru
void setup()
{
Serial.begin(9600); // для отладки
mySerial.begin(9600);
delay(2000);
Serial.println("Init");
mySerial.println("AT+RST"); // сброс и проверка, если модуль готов
delay(1000);
if(mySerial.find("ready"))
{Serial.println("WiFi - Module is ready");}
else
{Serial.println("Module dosn't respond.");
while(1);
}
delay(1000);
// соединение по wifi
boolean connected=false;
for(int i=0;i<5;i++)
{
if(connectWiFi())
{connected = true;
mySerial.println("Connected to Wi-Fi...");
break;
}
}
if (!connected)
{
mySerial.println("Coudn't connect to Wi-Fi.");
while(1);
}
delay(5000);
mySerial.println("AT+CIPMUX=0"); // режим одиночного соединения
}
void loop()
{
String cmd = "AT+CIPSTART=\"TCP\",\"";
cmd += DST_IP;
cmd += "\",8283";
Serial.println(cmd);
mySerial.println(cmd);
if(mySerial.find("Error"))
return;
double val = analogRead(LM335); // чтение показаний LM335
double voltage = val*5.0/1024; // перевод в вольты
double temp = voltage*100 - 273.15; // в градусы Цельсия
cmd = "#A0:F3:C1:70:AA:94\n#2881C4BA0200003B1#"+String(temp)+"\n##";
delay(3000);
mySerial.print("AT+CIPSEND=");
mySerial.println(cmd.length());
delay(1000);
Serial.println(">");
mySerial.print(cmd);
Serial.println(cmd);
delay(3000);
mySerial.println("AT+CIPCLOSE");
delay(600000);
}
// процедура установки Wi-Fi-соединения
boolean connectWiFi()
{
String cmd="AT+CWJAP=\"";
cmd+=SSID;
cmd+="\",\"";
cmd+=PASS;
cmd+="\"";
mySerial.println(cmd);
Serial.println(cmd);
delay(2000);
if(mySerial.find("OK"))
{
Serial.println("OK, Connected to Wi-Fi.");
return true;
}
else
{
Serial.println("Can not connect to the Wi-Fi.");
return false;
}
}
