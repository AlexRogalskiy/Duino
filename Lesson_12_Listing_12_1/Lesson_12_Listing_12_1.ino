int relayPin = 10; // подключение к выводу D10 Arduino
void setup()
{
pinMode(relayPin, OUTPUT); // настроить вывод как выход (OUTPUT)
}
// функция выполняется циклически бесконечное число раз
void loop()
{
digitalWrite(relayPin, HIGH); // включить реле
delay(5000);
digitalWrite(relayPin, LOW); // выключить реле
delay(5000);
}
