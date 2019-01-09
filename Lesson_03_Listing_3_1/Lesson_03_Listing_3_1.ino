const int LED=10; // вывод для подключения светодиода 10 (D10)
void setup()
{
// Конфигурируем вывод подключения светодиода как выход (OUTPUT)
pinMode(10, OUTPUT);
// включаем светодиод, подавая на вывод 1 (HIGH)
digitalWrite(LED,HIGH);
}
void loop()
{;}
