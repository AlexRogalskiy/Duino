// sixRelays.pde - Control relays with a computer 
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
 
int ledPin=13;
 
void relayPulse(int pin)
{
	pinMode(pin, OUTPUT);
	digitalWrite(ledPin, HIGH);
	digitalWrite(pin, HIGH);
	delay(1000);
	digitalWrite(pin, LOW);
	digitalWrite(ledPin, LOW);
}
 
int charToInt(char ch)
{
	return ch-'0';
}
 
void setup()
{
	pinMode(ledPin, OUTPUT);	
	Serial.begin(115200);
	Serial.println("Relay controller ready - www.sulautetut.fi");
}
 
void loop()
{
	if ( Serial.available()) {
		char ch = Serial.read();
		int pin = charToInt(ch);
		relayPulse(pin);
	}
	delay(20);
}
