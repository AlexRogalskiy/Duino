// vaijyvahti.pde - Shake if something comes near
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

float limitCm=90; 
float t=20; // Air temperature, celcius
int pingPin = 2;
int motorPin = 5;
float speedCmUs = 0; // Speed of sound, cm per microsecond

void setup()
{
	pinMode(motorPin, OUTPUT);
	pinMode(pingPin, OUTPUT);  
	float v=331.5+0.6*t;
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
	float dist=distanceCm();
	if (dist<limitCm){
		digitalWrite(motorPin, HIGH);
	}
	else {
		digitalWrite(motorPin, LOW);
	}
	delay(20);
}
