// threePingLed.pde - Light a LED if hand is near. 
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

int leftPin = 2;
int centerPin = 3;
int rightPin = 4;

int leftLedPin = 5;
int centerLedPin = 6;
int rightLedPin = 7;

int maxD = 20; // cm

float t=20; // Air temperature (for calculating speed of sound)
float speedCmUs = -1; // Speed of sound, cm / microsecond

void setup()
{
	Serial.begin(115200);
	// Calculate the speed of sound for given temperature t degrees centigrade (C). 
	float v=331.5+0.6*t; // speed of sound at given temperature, m/s
	Serial.print("Speed of Sound in m/s is ");
	Serial.println(v, DEC);
	speedCmUs=v*100/1000000; // speed of sound in cm per microsecond. 
}

float distanceCm(int pingPin){
	// command Ping sensor to send the pulse:
	delay(10);
	pinMode(pingPin, OUTPUT);
	digitalWrite(pingPin, LOW);
	delayMicroseconds(3);
	digitalWrite(pingPin, HIGH);
	delayMicroseconds(5);
	digitalWrite(pingPin, LOW);
	// read the answer:
	pinMode(pingPin, INPUT);
	// Connect to +5 V with internal pullup resistor:
	digitalWrite(pingPin, HIGH);
	long int roundTripTimeUs;
	roundTripTimeUs = pulseIn(pingPin, HIGH, 1000);
	// divide roundtrip time by 2 to get one-way time
	return (float) roundTripTimeUs / 2 * speedCmUs; 
}

int ping(int pingPin, int ledPin)
{
	int d = distanceCm(pingPin); // cm
	pinMode(ledPin, OUTPUT);
	if (d<maxD) {
		digitalWrite(ledPin, HIGH);
	} else {
		digitalWrite(ledPin, LOW);
	}
	Serial.print(d, DEC);
	Serial.print(" cm.	");
}
 
void loop()
{
	ping(leftPin, leftLedPin);
	ping(centerPin, centerLedPin);
	ping(rightPin, rightLedPin);
	Serial.println("");
}
