// servoRange.pde - Send increasing pulse to Arduino 
// and show values in serial console
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

int servoFrontPin=2;

void pulseServo(int servoPin, int pulseLenUs)
{
	digitalWrite(servoPin, HIGH);
	delayMicroseconds(pulseLenUs);
	digitalWrite(servoPin, LOW);
	delay(20);
}

void setup()
{
	pinMode(servoFrontPin, OUTPUT);
	Serial.begin(115200);
}

void loop()
{
	for (int i=1; i<=3000; i=i+2) {
		pulseServo(servoFrontPin, i);
		Serial.println(i);  
	}
}
