// threePing.pde - Read three ping sensors
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

float t=20; // Air temperature (for calculating speed of sound)
int leftPin = 2;
int centerPin = 3;
int rightPin = 4;

float speedCmUs = 0; // Speed of sound, cm / microsecond

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
	//Serial.print("Pulse measure done, round trip time (RTT) us: ");
	//Serial.println(roundTripTimeUs, DEC);
	// divide roundtrip time by 2 to get one-way time
	return (float) roundTripTimeUs / 2 * speedCmUs; 
}

void loop()
{
	Serial.print("Left: ");
	Serial.print(distanceCm(leftPin), DEC);
	Serial.print(" cm  Center: ");
	Serial.print(distanceCm(centerPin), DEC);
	Serial.print(" cm  Right: ");
	Serial.print(distanceCm(rightPin), DEC);
	Serial.println(" cm.");
}

