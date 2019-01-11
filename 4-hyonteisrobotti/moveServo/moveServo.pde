// moveServo.pde - Move servo to center, maximum angle
// and to minumum angle
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
	for (int i=0; i<=25; i++) {
		pulseServo(servoFrontPin, 1500);
	}
	for (int i=0; i<=25; i++) {
		pulseServo(servoFrontPin, 2400);
	}
	for (int i=0; i<=25; i++) {
		pulseServo(servoFrontPin, 1500);
	}
	for (int i=0; i<=25; i++) {
		pulseServo(servoFrontPin, 600);
	}
}
