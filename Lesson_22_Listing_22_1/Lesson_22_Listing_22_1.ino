// контакт подключения аналогового вывода MQ4
const int analogInPin = A1;
const int ledPin = 8; // контакт подключения светодиода
int sensorValue = 0; // переменная для хранения значения датчика
void setup()
{
Serial.begin(9600);
pinMode(ledPin, OUTPUT);
}
void loop()
{
sensorValue = analogRead(analogInPin); // получить значение
if (sensorValue >= 750) // превышение уровня
digitalWrite(ledPin, HIGH); // зажечь светодиод превышения
else
digitalWrite(ledPin, LOW); // потушить светодиод превышения
// вывести значение в последовательный порт
Serial.print("sensor = " );
Serial.println(sensorValue); // пауза перед следующим измерением
delay(1000);
}
