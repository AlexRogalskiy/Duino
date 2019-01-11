// distanceUltraSonic.pde - Measure distance using Ping sensor 
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

float t=20; // Air temperature (for calculating speed of sound)
int pingPin = 2; // Digital pin connected to Ping “SIG” pin
float speedCmUs = 0; // Speed of sound, cm / microsecond

void setup()
{
	Serial.begin(115200);
	pinMode(pingPin, OUTPUT);  
	// Calculate the speed of sound for given temperature t degrees centigrade (C). 
	float v=331.5+0.6*t; // speed of sound at given temperature, m/s
	Serial.print("Speed of Sound in m/s is ");
	Serial.println(v, DEC);
	speedCmUs=v*100/1000000; // speed of sound in cm per microsecond. 
}

float distanceCm(){
	// command Ping sensor to send the pulse:
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
	long int roundTripTimeUs = pulseIn(pingPin, HIGH);
	// divide roundtrip time by 2 to get one-way time
	return (float) roundTripTimeUs / 2 * speedCmUs; 
}

void loop()
{
	Serial.println(distanceCm(), DEC);
	delay(200);
}
