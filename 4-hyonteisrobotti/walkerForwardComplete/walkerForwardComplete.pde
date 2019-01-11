// walkerForwardComplete.pde - Two servo walker. 
// Complete code with obstacle avoidance
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

/* Movement - global variables */

int servoFrontPin=2;
int servoBackPin=3;

// pulse length for servo control in microseconds (1 us = 10**-6 s)
int centerPos=1500; 
int frontRightUp=1200;
int frontLeftUp=1800;
int backRightForward=1250;
int backLeftForward=1750;

int centerTurnPos=1350;
int frontTurnRightUp=centerTurnPos-300;
int frontTurnLeftUp=centerTurnPos+600;
int backTurnRightForward=centerTurnPos-250;
int backTurnLeftForward=centerTurnPos+250;

/* Ping distance measurement - global variables */

float t=20; // Air temperature, celcius
int pingPin = 4;
float speedCmUs = 0; // Speed of sound, cm per microsecond
float distanceFront=0; //cm
int startAvoidanceDistance=20; //cm

float distanceCm(){
	pinMode(pingPin, OUTPUT);
	digitalWrite(pingPin, LOW);
	delayMicroseconds(3);
	digitalWrite(pingPin, HIGH);
	delayMicroseconds(5);
	digitalWrite(pingPin, LOW);
	pinMode(pingPin, INPUT);
	digitalWrite(pingPin, HIGH);
	long int roundTripTimeUs = pulseIn(pingPin, HIGH);
	return (float) roundTripTimeUs / 2 * speedCmUs; 
}

void pulseServo(int servoPin, int pulseLenUs)
{
	digitalWrite(servoPin, HIGH);
	delayMicroseconds(pulseLenUs);
	digitalWrite(servoPin, LOW);
	delay(20);
}

void center()
{
	for (int i=0; i<=10; i++) {
		pulseServo(servoFrontPin, centerPos);
		pulseServo(servoBackPin, centerPos);
	}
}

void moveForward()
{
	for (int i=0; i<=5; i++) {
		pulseServo(servoFrontPin, frontRightUp);
		pulseServo(servoBackPin, backLeftForward);
	}
	for (int i=0; i<=2; i++) {
		pulseServo(servoFrontPin, centerPos);
		pulseServo(servoBackPin, centerPos);
	}
	for (int i=0; i<=5; i++) {
		pulseServo(servoFrontPin, frontLeftUp);
		pulseServo(servoBackPin, backRightForward);
	}  
}

void moveBackRight()
{
	for (int i=0; i<=5; i++) {
		pulseServo(servoFrontPin, frontRightUp);
		pulseServo(servoBackPin, backRightForward-100);
	}
	for (int i=0; i<=2; i++) {
		pulseServo(servoFrontPin, centerPos);
		pulseServo(servoBackPin, centerPos-100);
	}
	for (int i=0; i<=5; i++) {
		pulseServo(servoFrontPin, frontLeftUp+150);
		pulseServo(servoBackPin, backLeftForward-100);
	}
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
	pinMode(pingPin, OUTPUT);
	float v=331.5+0.6*t;
	speedCmUs=v*100/1000/1000;
}
 
void loop()
{
	distanceFront=distanceCm();
	if (distanceFront<startAvoidanceDistance) {
		for (int i=0; i<=8; i++) {
			moveBackRight();
		}
		for (int i=0; i<=10; i++) {
			moveTurnLeft();
		}
	} else {
		moveForward();
	}
}

