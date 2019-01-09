// подключение библиотек для RTC
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
// подключение библиотеки для SD
#include <SD.h>
File myFile;
String sfilename;
char filename[20];
const int LM335=A0; // для подключения LM335
tmElements_t tm;
unsigned long millis1=0;
void setup()
{;}
void loop()
{
// проверка прошло 5 минут?
if(millis()-millis1>5*60*000)
{
millis1=millis();
// получить имя файла для текущего дня
sfilename=get_file_name();
sfilename.toCharArray(filename,20);
// открыть файл или создать
myFile = SD.open(filename, FILE_WRITE);
// получить температуру
double val = analogRead(lm335); // чтение
double voltage = val*5.0/1024; // перевод в вольты
double temp = voltage*100 - 273.15; // в градусы Цельсия
// получить время H:m
// создать запись для файла
record=get_time();
record+=" ";
record+=String(temp);
myFile.println(record);
myFile.close();
}
// получение времени дня
String get_time()
{
String time1;
RTC.read(tm);
if(tm.Hour()<10)
time1="0"+String(tm.Hour(),DEC);
else
time1=String(tm.Hour(),DEC);
if(tm.Minute()<10)
time1+=":0"+String(tm.Minute(),DEC);
else
time1+=":"+String(tm.Minute(),DEC);
return time1;
}
