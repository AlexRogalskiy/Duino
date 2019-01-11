// interactivePaintingSensor.pde - Detect direction of hand wave. 
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

int slide=0; // -3 left slide; 0 neutral state; +3 right slide; 
bool left=false;
bool center=false;
bool right=false;

int leftPin = 2;
int centerPin = 3;
int rightPin = 4;

int ledPin = 13;
int leftLedPin = 5;
int centerLedPin = 6;
int rightLedPin = 7;

int maxD = 20; // cm

long int lastTouch=-1;
int resetAfter=2000; // ms
int afterSlideDelay=500; // ms; all slides ignored after successfull slide
int afterSlideOppositeDelay=1500; // left slides ignored after successfull right slide (and vice versa)

float t=20; // Air temperature (for calculating speed of sound)
float speedCmUs = -1; // Speed of sound, cm / microsecond

void setup()
{
	Serial.begin(115200);
	// Calculate the speed of sound for given temperature t degrees centigrade (C). 
	float v=331.5+0.6*t; // speed of sound at given temperature, m/s
	Serial.print("Interactive painting initialized - http://sulautetut.fi");
	speedCmUs=v*100/1000000; // speed of sound in cm per microsecond. 
}

float distanceCm(int pingPin){
	// command Ping sensor to send the pulse:
	delay(5);
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

bool ping(int pingPin, int ledPin)
{
	// three point average
	int d = distanceCm(pingPin); // cm
	d = (d+distanceCm(pingPin))/2;
	//d = (d+distanceCm(pingPin))/2;

	bool pinActivated=false;
	pinMode(ledPin, OUTPUT);
	if (d<maxD) {
		digitalWrite(ledPin, HIGH);
		pinActivated=true;
	} else {
		digitalWrite(ledPin, LOW);
		pinActivated=false;
	}
	//Serial.print(d, DEC);
	//Serial.print(" cm.	");
	return pinActivated;
}

bool slideNow()
{
	if (slide>0)
		 Serial.println("F");
	if (slide<0)
		 Serial.println("B");
	digitalWrite(ledPin, HIGH);
	delay(afterSlideDelay);
	slide=0;
}

void loop()
{
	left=ping(leftPin, leftLedPin);
	center=ping(centerPin, centerLedPin);
	right=ping(rightPin, rightLedPin);

	if (left || center || right) {
		lastTouch=millis(); 
	}

	if (millis()-lastTouch>resetAfter ) {
		slide=0;
		digitalWrite(ledPin, LOW);
		// Serial.println("Reset slide and timer. ");
	}

	if (slide>=0) { // only if we are not already in opposite move
		if ( (left) && (!right) )
			slide=1;
		if (center && (slide==1))
			slide=2;
		if (right && (slide==2))
			slideNow();
	}

	if (slide<=0) {	
		if (right && (!left))
			slide=-1;
		if (center && slide==-1)
			slide=-2;
		if (left && slide==-2) {
			slideNow();
		}
	}
}

