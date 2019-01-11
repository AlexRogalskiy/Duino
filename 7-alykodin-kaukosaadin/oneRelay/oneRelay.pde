// oneRelay.pde - Control relay with a computer 
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

int relayPin=2;
 
void setup()
{
	pinMode(relayPin, OUTPUT);	
	Serial.begin(115200);
	Serial.println("Control relay with serial - ready. www.sulautetut.fi");
}
 
void loop()
{
	char ch = Serial.read();
	if (ch=='2') {
		digitalWrite(relayPin, LOW);
	}
	if (ch=='8') {
		digitalWrite(relayPin, HIGH);
	}

}

