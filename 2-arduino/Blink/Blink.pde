// Blink.pde - Blink led, a "hello world"
// File: Sketchbook: Examples: Digital: Blink

int ledPin = 13;

void setup()
{
	pinMode(ledPin, OUTPUT);
}

void loop()
{
	digitalWrite(ledPin, HIGH);
	delay(1000);
	digitalWrite(ledPin, LOW);
	delay(1000);
}
