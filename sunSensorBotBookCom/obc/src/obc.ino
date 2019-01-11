/* Master OBC for Aalto-1 i2c
Copyright 2013 http://botbook.com/satellite Tero Karvinen & Kimmo Karvinen

ArduinoUnoA5-scl white clock
ArduinoUnoA4-sda green data
ArduinoGND black (slave and master need common ground level) 

*/

#include <Wire.h>

const int slaveAddr = 88;

int xdeg = -274;
int ydeg = -274;

void setup()
{
	Wire.begin();
	dInit();
}

void loop()
{
	iUpdateXY();
    delay(300); // update speed mostly limited by debug serial printing
}

/* debug, serial */

void dInit()
{
    Serial.begin(115200); // bit/s
    Serial.println("");
	Serial.println("");
    Serial.println("Master: OBC Emulator starting... Compiled " __DATE__ " " __TIME__ ".");
    Serial.println("Master: BotBook.com/satellite");
   	Serial.println("");
   	// delay(3000); // only needed for user to read texts
}

/* i2c */

void iUpdateXY()
{
	Serial.println("Master: requesting sun position (XY) over I2C...");
	Wire.requestFrom(slaveAddr, 2); // bytes
	if (Wire.available()) {
		xdeg = Wire.read() - 256; // TODO: proper sign handling
	} else {
		Serial.println("Master: Error: Sun sensor (I2C slave) sent too few bytes. ");
	}	
	if (Wire.available()) {
		ydeg = Wire.read() - 256;
	} else {
		Serial.println("Master: Error: Sun sensor (I2C slave) sent too few bytes. ");
	}
	Serial.print("Master: Received sun position over I2C:	");
	Serial.print("X: ");
	Serial.print(xdeg, DEC);
	Serial.print("	Y: ");
	Serial.println(ydeg, DEC);
}



