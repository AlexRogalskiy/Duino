// distancePing.pde - Measure distance with ultrasonic ping 
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
 
float t=20; // Air temperature, celcius
int pingPin = 2;
float speedCmUs = 0; // Speed of sound, cm per microsecond
 
void setup()
{
	Serial.begin(115200);
	pinMode(pingPin, OUTPUT);  
	float v=331.5+0.6*t;
	Serial.print(“Speed of Sound in m/s is “);
	Serial.println(v, DEC);
	speedCmUs=v*100/1000/1000;
}
 
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

void loop()
{
	Serial.println(distanceCm(), DEC);
	delay(50);
}
