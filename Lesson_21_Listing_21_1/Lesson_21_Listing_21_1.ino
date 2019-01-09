#include "DHT.h"
#define DHTPIN 2 // пин подключения контакта DATA
#define DHTTYPE DHT11 // DHT 11
#include <LiquidCrystal.h>
// инициализация с указанием контактов подключения
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
DHT dht(DHTPIN, DHTTYPE);
void setup()
{
lcd.begin(16,2); // режим работы
dht.begin();
}
void loop()
{
// получение с датчика данных влажности и температуры
float h = dht.readHumidity();
float t = dht.readTemperature();
if (isnan(t) || isnan(h)) // ошибка получения данных
{
lcd.clear();lcd.setCursor(0,0);
lcd.print("Failed to read");
}
else // вывести данные на ЖКИ
{
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Humidity: ");lcd.print(h); lcd.println(" %");
lcd.setCursor(0,1);
lcd.print("Temp: "); lcd.print(t);lcd.println(" *C");
}
delay(2000); // пауза перед следующим измерением
}
