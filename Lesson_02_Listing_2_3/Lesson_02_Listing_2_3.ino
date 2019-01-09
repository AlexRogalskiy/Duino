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
// Функция сглаживания дребезга. Принимает в качестве
// аргумента предыдущее состояние кнопки и выдает фактическое.
boolean debounce(boolean last)
{
boolean current = digitalRead(BUTTON); // Считать состояние кнопки,
if (last != current) // если изменилось...
{
d elay(5); // ж дем 5 м с
current = digitalRead(BUTTON); // считываем состояние кнопки
return current; // возвращаем состояние кнопки
}
}
void loop()
{
tekButton = debounce(prevButton);
if (prevButton == LOW && tekButton == HIGH) // если нажатие...
{
ledOn = !ledOn; // инвертировать значение состояния светодиода
}
prevButton = tekButton;
digitalWrite(LED, ledOn); // изменить статус состояния светодиода
}
