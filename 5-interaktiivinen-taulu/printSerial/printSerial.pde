// printSerial.pde - Print test data to serial port.	
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

void setup()
{
	Serial.begin(115200); // the only speed for modern Arduino BT
}

void loop()
{
	Serial.print("F ");
	delay(500);
	Serial.print("B ");
	delay(1000);
	Serial.print(" FFFF ");
	delay(500);
	Serial.print(" http://sulautetut.fi ");
	delay(2000);
}
