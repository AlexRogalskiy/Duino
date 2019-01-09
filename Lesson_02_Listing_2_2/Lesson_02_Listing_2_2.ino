const int LED=10; // Контакт 10 для подключения светодиода
const int BUTTON=2; // Контакт 2 для подключения кнопки
int tekButton = LOW; // Переменная для сохранения текущего состояния кнопки
int prevButton = LOW; // Переменная для сохранения предыдущего состояния
// к нопки
boolean ledOn = false; // Текущее состояние светодиода (включен/выключен)
void setup()
{
// Сконфигурировать контакт светодиода как выход
pinMode (LED, OUTPUT);
// Сконфигурировать контакт кнопки как вход
pinMode (BUTTON, INPUT);
}
void loop()
{
tekButton=digitalRead(BUTTON);
if (tekButton == HIGH && prevButton == LOW)
{
// нажатие кнопки – изменить состояние светодиода
ledOn=!ledOn;
digitalWrite(LED, ledOn);
}
prevButton=tekButton;
}
