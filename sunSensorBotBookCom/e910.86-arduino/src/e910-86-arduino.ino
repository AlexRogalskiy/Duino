/* Read sun position with Elmos E910.86 SPI on Arduino.
Copyright 2013 http://botbook.com/satellite Tero Karvinen & Kimmo Karvinen

Connections for SPI:
# Arduino-Purpose-E910.86
ard13-sck-e1  white
ard11-mosi-e2 green
ard12-miso-e3 yellow
ard10-cs-e4   brown
ard5v-reset-e5	red
ardGnd-gnd-e6	black
ard5v-vcc-e7	red
ardA3-analogSOUT-e8 blue 

When the text is top up ("Aalto-1 Sun Sensor Proto V1")
- pin one is top left.
- x grows to left
- y grows down
- when sun is directly in front of the sensor, (x,y)==(0,0)
The connector has 8 pins. 
The protoboard is made excatly as descibed on E910.86 data sheet. 

SPI functions start with 's'. 
*/

#include <SPI.h>

const int sSlaveSelectPin = 10;

int xdeg = -273;
int ydeg = -273;
 
void setup() {
	Serial.begin(115200); // bit/s	
    Serial.println("BotBook.com/satellite");
    sInit();
	spiWrite(0b10010000, 0b00010000); // enable
}
 
void loop() {
	spiWrite(0b00000000, 0b00011000);	// read xy-angle
	delay(100);
}

/* spi */

void sInit()
{
	Serial.println("sInit()");
	SPI.begin(); 
	Serial.println("SPI.begin() done. ");
	pinMode (sSlaveSelectPin, OUTPUT);
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	digitalWrite(sSlaveSelectPin, HIGH);
	Serial.println("sInit() done. ");
}

int spiWrite(int byte1, int byte2) {
	digitalWrite(sSlaveSelectPin, LOW);
	delay(1);
	unsigned int res1 = SPI.transfer(byte1);
	delay(1);
	unsigned int res2 = SPI.transfer(byte2);
	delay(1);
	digitalWrite(sSlaveSelectPin, HIGH); 
	sSerExplainPacket(combine(res1, res2));
}

/* parsing */

void sSerExplainPacket(unsigned long int p)
{
	if (p>>16-4 == 0b0100) {

		int x = p & 0b0000111111000000;
		x = x>>6;
		const int xzero = 29;
		const int xmax = 51; // at 75 deg
		xdeg = (x-xzero) * 75/(xmax-xzero); 
		int y = p & 0b0000000000111111;

		const int yzero = 29;
		const int ymax = 51; // at 75 deg
		ydeg = (y-yzero) * 75/(ymax-yzero);

		Serial.print("xy: ");
		Serial.print("raw:(");
		Serial.print(x);
		Serial.print(",");
		Serial.print(y);
		Serial.print(")");

		Serial.print(" deg:[");
		Serial.print(xdeg);
		Serial.print(",");
		Serial.print(ydeg);
		Serial.println("]");

		if ((xdeg<-75) or (75<xdeg) or (ydeg<-75) or (75<ydeg)) {
			Serial.print(" (angle out of range) ");
		}
	} else {
		Serial.println("SPI Error: Unknown message ");
		serBits(p);
	}
}

/* end parsing */

/* bit helpers */

unsigned long int combine(unsigned int left, unsigned int right)
{ // combine two 8 bit words to one 16 bit word
	return (left<<8)+right;
}

void serBits(unsigned long int x) 
{ // Print bits of x. x must be 16 bits.
	for(int i=16; i>0; i--) {
		unsigned int b = 0b1000000000000000 & x;
		b = b>>(16-1);
		Serial.print(b, DEC);
		if ((i-1) % 4 == 0)
			Serial.print(' '); 
		x=x<<1;
	}
}

/* end bit helpers */
