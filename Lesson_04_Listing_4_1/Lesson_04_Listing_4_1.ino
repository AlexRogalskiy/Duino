const int POT=0; // Аналоговый вход A0 для подключения потенциометра
int valpot = 0; // переменная для хранения значения потенциометра
void setup()
{
Serial.begin(9600);
}
void loop()
{
valpot = analogRead(POT); // чтение данных потенциометра
Serial.println(valpot); // вывод значений в последовательный порт
delay(500); // задержка 0.5 сек
}
