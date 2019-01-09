// коды клавиш ИК-пульта
#define K2 1936
#define K3 3984
#define K4 144
#define K5 2192
#define K6 3472
#define K7 1424
#define K8 3216
#define K9 1168
#include <IRremote.h> // подключение библиотеки
int RECV_PIN = 1; // контакт подключения ИК-приемника
IRrecv irrecv(RECV_PIN);
decode_results results;
// значения на D2 – D9 Arduino
int val_pins[]={0,0,0,0,0,0,0,0};
int res=0;
void setup()
{
Serial.begin(9600);
irrecv.enableIRIn(); // включить приемник
for(int i=2;i<10;i++)
{
pinMode(i,OUTPUT);
digitalWrite(i,LOW);
}
}
void loop()
{
if (irrecv.decode(&results))
{
switch(results.value)
{
case K2: res=2; break;
case K3: res=3; break;
case K4: res=4; break;
case K5: res=5; break;
case K6: res=6; break;
case K7: res=7; break;
case K8: res=8; break;
case K9: res=9; break;
default: res=0; break;
}
if(res>0)
{
pins[res-2]=1- pins[res-2];
// переключить светодиод
digitalWrite(res, pins[res-2]);
}
irrecv.resume(); // получить следующее значение
}
}
