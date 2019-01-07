#include <Wire.h>
 
int clockAddress = 0x68;  // I2C адрес микросхемы
byte second, minute, hour, dayOfWeek, day, dayOfMonth, month, year;
  
#define DS3231_SCL 0
#define DS3231_SDA 2
  
void setup() {
  // запустить Wire
  Wire.begin(DS3231_SCL, DS3231_SDA);
  Serial.begin(9600);
}
 
void loop() { 
  if (Serial.available() > 0) {
    String str = Serial.readStringUntil('\n');
    if (str == "show time") {
      getDateDs3231(); // показать время
      String s = getTimeStr();
      Serial.println(s);
    }
    if (str == "set time") {
      time_set();
    } 
  }
}
 
// получить дату и время
void getDateDs3231() {
  Wire.beginTransmission(clockAddress);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(clockAddress, 7);
 
  second     = bcdToDec(Wire.read() & 0x7f);
  minute     = bcdToDec(Wire.read());
  hour       = bcdToDec(Wire.read() & 0x3f);
  dayOfWeek  = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  day = dayOfMonth;
  month      = bcdToDec(Wire.read());
  year       = bcdToDec(Wire.read());
}
void setDateDs1307()            
{
  Wire.beginTransmission(clockAddress);
  Wire.write(byte(0x00));
  Wire.write(decToBcd(second));  
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));    
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}
 
byte decToBcd(byte val) {
  return ( (val / 10 * 16) + (val % 10) );
} 
byte bcdToDec(byte val) {
  return ( (val / 16 * 10) + (val % 16) );
}
  
String getTimeStr() {
  String str = String(day) + "." + formatDigit(month, 2) + "." + 
      formatDigit(year, 2) + " " + formatDigit(hour, 2) + ":" +
      formatDigit(minute, 2) + ":" + formatDigit(second, 2);
  return str;
}
String formatDigit(int i, int len) {
  String s = String(i);
  while (s.length() < len) {
    s = "0" + s;
  }
  return (s);
} 
 
void time_set() {
  Serial.println("Enter Year 2 last digits");
  while (Serial.available() <= 0);
  year = Serial.parseInt();
  Serial.println(year);
 
  Serial.println("Month");
  while (Serial.available() <= 0);
  month = Serial.parseInt();
  Serial.println(month);
 
  Serial.println("Day");
  while (Serial.available() <= 0);
  day = Serial.parseInt();
  dayOfMonth = day;
  Serial.println(day);
 
  Serial.println("Day of Week");
  while (Serial.available() <= 0);
  dayOfWeek = Serial.parseInt();
  Serial.println(dayOfWeek);
 
  Serial.println("Hour");
  while (Serial.available() <= 0);
  hour = Serial.parseInt();
  Serial.println(hour);
 
  Serial.println("Minute");
  while (Serial.available() <= 0);
  minute = Serial.parseInt();
  Serial.println(minute);
 
  setDateDs1307();
 
  Serial.println("Time recvd OK");
} 

