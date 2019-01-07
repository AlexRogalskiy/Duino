#include <Wire.h> // Wire.h
byte input=0;
void setup()
{
  Serial.begin(9600);
  Wire.begin(0,2); // wake up I2C bus
  delay(10);
  Wire.beginTransmission(0x20);
  Wire.write(0x01); // IODIRB register
  Wire.write(0x00); // set entire PORT B as output
  Wire.endTransmission();
}
 
void loop()
{
  Serial.println(0x00);
  // write that input to PORT B
  Wire.beginTransmission(0x20);
  delay(10);
  Wire.write(0x13); // address PORT B
  Wire.write(0x00);    // PORT B
  Wire.endTransmission();
  delay(1000); // for debounce
 
  Serial.println(0xff);
  // write that input to PORT B
  Wire.beginTransmission(0x20);
  delay(10);
  Wire.write(0x13); // address PORT B
  Wire.write(0xff);    // PORT B
  Wire.endTransmission();
  analogWriteFreq(200);
  analogWrite(14,512);
  delay(2000);
  analogWrite(14,0);  
  delay(1000); // for debounce
}

