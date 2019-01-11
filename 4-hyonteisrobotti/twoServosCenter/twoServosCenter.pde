// twoServosCenter.pde - Center two servos
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

int servoFrontPin=2;
int servoBackPin=3;

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
	pinMode(servoBackPin, OUTPUT);
	Serial.begin(115200);
}

void loop()
{
	for (int i=0; i<=25; i++) {
		pulseServo(servoFrontPin, 1500);
		pulseServo(servoBackPin, 1500);
	}
}
