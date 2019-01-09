#include <IRremote.h> // подключение библиотеки
int RECV_PIN = 1; // контакт подключения ИК-приемника
IRrecv irrecv(RECV_PIN);
decode_results results;
void setup()
{
Serial.begin(9600);
irrecv.enableIRIn(); // включить приемник
}
void loop()
{
if (irrecv.decode(&results))
{
Serial.println(results.value, HEX);
irrecv.resume(); // получить следующее значение
}
}
