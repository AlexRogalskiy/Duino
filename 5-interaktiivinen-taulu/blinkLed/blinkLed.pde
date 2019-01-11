// blinkLed.pde - Turn led on and off
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

int ledPin = 2;

void setup()
{
	pinMode(ledPin, OUTPUT);
}
 
void loop()
{
	digitalWrite(ledPin, HIGH);
	delay(500);
	digitalWrite(ledPin, LOW);
	delay(500);
}
