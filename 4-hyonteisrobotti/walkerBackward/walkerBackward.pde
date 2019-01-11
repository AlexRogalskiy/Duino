// walkerBackward.pde - Two servo walker. Backward.
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

int servoFrontPin=2;
int servoBackPin=3;

// pulse length for servo control in microseconds (1 us = 10**-6 s)
int centerPos=1500; 
int frontRightUp=1200;
int frontLeftUp=1800;
int backRightForward=1250;
int backLeftForward=1750;

void pulseServo(int servoPin, int pulseLenUs)
{
	digitalWrite(servoPin, HIGH);
	delayMicroseconds(pulseLenUs);
	digitalWrite(servoPin, LOW);
	delay(20);
}
 
void moveBack()
{
	for (int i=0; i<=5; i++) {
		pulseServo(servoFrontPin, frontRightUp);
		pulseServo(servoBackPin, backRightForward);
	}
	for (int i=0; i<=2; i++) {
		pulseServo(servoFrontPin, centerPos);
		pulseServo(servoBackPin, centerPos);
	}
	for (int i=0; i<=5; i++) {
		pulseServo(servoFrontPin, frontLeftUp);
		pulseServo(servoBackPin, backLeftForward);
	}  
}
 
void setup()
{
	pinMode(servoFrontPin, OUTPUT);
	pinMode(servoBackPin, OUTPUT);
 }
 
void loop()
{
	moveBack();
}
