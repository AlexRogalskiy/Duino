const int axisX=A0 // ось Х подключена к A0
const int axisY=A1 // ось Y подключена к A1
const int axisZ=2 // ось Z (кнопка джойстика) подключена к D2
int valX, valY, valZ = 0; // переменные для хранения значений осей
void setup()
{
pinMode(axis_Z, INPUT_PULLUP); // конфигурируем D2 как INPUT с включением
// подтягивающего резистора внутри процессора
Serial.begin(9600);
}
void loop() {
valX = analogRead(axisX); // значение оси Х
valY = analogRead(axisY); // значение оси Y
valZ = 1-digitalRead(axisZ); // значение оси Z (кнопка)
// выводим значения в монитор
Serial.print("X:");Serial.print(valX, DEC);
Serial.print(" | Y:");Serial.print(valY, DEC)
Serial.print(" | Z: ");Serial.println(valZ, DEC);
delay(500); // пауза перед следующим считыванием данных
}
