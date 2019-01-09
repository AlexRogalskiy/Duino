const int MOTOR=9; // Выход для подключения MOSFET
const int POT=0; // Аналоговый вход A0 для подключения потенциометра
int valpot = 0; // переменная для хранения значения потенциометра
int speedMotor = 0; // переменная для хранения скорости двигателя
void setup()
{
//
pinMode(MOTOR,OUTPUT);
}
void loop()
{
valpot = analogRead(POT); // чтение данных потенциометра
// масштабируем значение к интервалу 0-255
speedMotor=map(valpot,0,1023,0,255);
// устанавливаем новое значение ШИМ
analogWrite(MOTOR,speedMotor);
delay(1000); // пауза
}
