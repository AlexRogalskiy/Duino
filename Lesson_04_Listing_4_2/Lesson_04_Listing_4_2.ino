const int POT=0; // Аналоговый вход A0 для подключения потенциометра
int valpot = 0; // переменная для хранения значения потенциометра
// список контактов подключения светодиодной шкалы
const int pinsled[10]={3,4,5,6,7,8,9,10,11,12};
int countleds = 0; // переменная для хранения значения шкалы
void setup()
{
for(int i=0;i<10;i++)
{
// Сконфигурировать контакты подсоединения шкалы как выходы
pinMode(pinsled[i],OUTPUT);
digitalWrite(pinsled[i],LOW);
{
}
void loop()
{
valpot = analogRead(POT); // чтение данных потенциометра
// масштабируем значение к интервалу 0-10
countled=map(valpot,0,1023,0,10);
// зажигаем количество полосок на шкале, равное countled
for(int i=0;i<10;i++)
{
if(i<countleds) // зажигаем светодиод шкалы
digitalWrite(pinsled[i],HIGH);
else // гасим светодиод шкалы
digitalWrite(pinsled[i],LOW);
}
}
