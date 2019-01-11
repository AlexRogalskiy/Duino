// dcMotor.pde - Run DC-motor to one direction
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

int motorPin = 5; 

void setup()
{
	pinMode(motorPin, OUTPUT);   
}

void loop()
{ 
	digitalWrite(motorPin, HIGH);
	delay(20);
}
