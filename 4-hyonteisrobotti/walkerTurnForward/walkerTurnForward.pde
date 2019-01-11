// walkerTurnForward.pde - Two servo walker. Turn forward.
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

int servoFrontPin=2;
int servoBackPin=3;
 
// pulse length for servo control in microseconds (1 us = 10**-6 s)
int centerTurnPos=1350;
int frontTurnRightUp=centerTurnPos-300;
int frontTurnLeftUp=centerTurnPos+600;
int backTurnRightForward=centerTurnPos-250;
int backTurnLeftForward=centerTurnPos+250;

void pulseServo(int servoPin, int pulseLenUs)
{
	digitalWrite(servoPin, HIGH);
	delayMicroseconds(pulseLenUs);
	digitalWrite(servoPin, LOW);
	delay(20);
}

void moveTurnLeft()
{
	for (int i=0; i<=5; i++) {
		pulseServo(servoFrontPin, frontTurnRightUp);
		pulseServo(servoBackPin, backTurnLeftForward);
	}
	for (int i=0; i<=2; i++) {
		pulseServo(servoFrontPin, centerTurnPos);
		pulseServo(servoBackPin, centerTurnPos);
	}
	for (int i=0; i<=5; i++) {
		pulseServo(servoFrontPin, frontTurnLeftUp);
		pulseServo(servoBackPin, backTurnRightForward);
	}
}
 
void setup()
{
	pinMode(servoFrontPin, OUTPUT);
	pinMode(servoBackPin, OUTPUT);
}
 
void loop()
{
	moveTurnLeft();
}
