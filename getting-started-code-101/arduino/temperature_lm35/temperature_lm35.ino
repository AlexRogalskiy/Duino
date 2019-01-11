// temperature_lm35.ino - LM35 temperature in Celcius to serial
// (c) BotBook.com - Karvinen, Karvinen, Valtokari

int lmPin = A0; // <1>

void setup()	// <2>
{
  Serial.begin(9600);	// <3>
}

float tempC()	// <4>
{
  float raw = analogRead(lmPin);  // <5>
  float percent = raw/1023.0; // <6>
  float volts = percent*5.0; // <7>
  return 100.0*volts;  // <8>
}

void loop()	// <9>
{
  Serial.println(tempC());  // <10>
  delay(200); // ms	// <11>
}

