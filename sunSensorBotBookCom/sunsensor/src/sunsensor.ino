/* Sun Sensor for Finland's first satellite, using Elmos E910.86 SPI and Arduino.
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

Connections for I2C:
# BothArduinos-Purpose
ArduinoUnoA5-scl white clock
ArduinoUnoA4-sda green data
ArduinoGND black (slave and master need common ground level)

When the text is top up ("Aalto-1 Sun Sensor Proto V1")
- pin one is top left.
- x grows to left
- y grows down
- when sun is directly in front of the sensor, (x,y)==(0,0)
The connector has 8 pins. 
*/

#include <Wire.h>
#include <SPI.h>

const int iSlaveAddr = 88;
const int sSlaveSelectPin = 10;
const int redLedPin = 8;

int xdeg = -273;
int ydeg = -273;
 
void setup() {
	dInit();
   	iInit();
    sInit();
	spiWrite(0b10010000, 0b00010000); // enable
	spiWrite(0b00000100, 0b00000000); // read active area
	Serial.println("Slave: Initialization completed. ");
	
}
 
void loop() {
	spiWrite(0b00000000, 0b00011000);	// read xy-angle
	delay(100);
}

/* debug, Serial */

void dInit()
{
	Serial.begin(115200); // bit/s	
    Serial.println("");
	Serial.println("");
    Serial.println("Slave: SunSensor starting... Compiled " __DATE__ " " __TIME__ ".");
    Serial.println("Slave: BotBook.com/satellite");
	Serial.println("");

	pinMode(redLedPin, OUTPUT);
	digitalWrite(redLedPin, HIGH);
}

void dPrintXY(int rawx, int rawy)
{
		Serial.print("Slave: xy: ");
		Serial.print("raw:(");
		Serial.print(rawx);
		Serial.print(",");
		Serial.print(rawy);
		Serial.print(")");

		Serial.print(" deg:[");
		Serial.print(xdeg);
		Serial.print(",");
		Serial.print(ydeg);
		Serial.println("]");
}

/* i2c */

void iInit()
{
	Wire.begin(iSlaveAddr);
	Wire.onRequest(iRequested);
}

void iRequested()
{
	Serial.print("Slave: Sending sun position over I2C:	");
	Serial.print("X: ");
	Serial.print(xdeg, DEC);
	Serial.print("	Y: ");
	Serial.println(ydeg, DEC);

	byte xy[2];
	xy[0]=xdeg;
	xy[1]=ydeg;
	Wire.write(xy, 2);
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
	ssSerExplainPacket(combine(res1, res2));
}

/* parsing */

void ssSerExplainPacket(unsigned long int p)
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

		// dPrintXY(x, y);

		if ((xdeg<-75) or (75<xdeg) or (ydeg<-75) or (75<ydeg)) {
			Serial.print(" (out of range) ");
		}
		
		/* 
		Using experimental formula above. 
		Datasheet formula: 6.1 Light Angle Measurement
		Figure shows that xdeg(x) is linear. 
		x			x (raw)	xdeg
		0B000000	0		15 deg
		0B011011	27		90 deg
		0B111000	56		165 deg
		=>
		xdeg	= x/56*(165-15) +15
				= 2.6786*x+15
		*/

	} else if (p>>16-6 == 0b011001) {
		Serial.println("Slave: SPI status:");
		Serial.println("Red/Blue active area?");
		Serial.println("0110 01BB BbbR RRrr");
		serBits(p);
		unsigned long int blue = p >> 5;
		blue = blue & 0b0000000000011111;
		unsigned long int red = p & 0b0000000000011111;
		Serial.print(" Blue active: ");
		Serial.print(blue);
		Serial.print(";  Red active: ");
		Serial.print(red);
		Serial.println();
	} else if (p>>16-9 == 0b011010000) {
		Serial.print("Red gain");
	} else if (p>>16-9 == 0b011011000) {
		Serial.print("Blue gain");
	} else if (p>>16-6 == 0b011100) {
		Serial.println("Status ");
		Serial.println("0111 00XX YYPS ZDDD");
		serBits(p);
		Serial.println("");
		Serial.print(" XX: ");
		int x = p>>8; // lose num digits from right
		x = x & 0b0000000000000011;
		Serial.println(x);
		
		Serial.print(" YY: ");
		int y = p>>6;
		y = y & 0b0000000000000011;
		Serial.println(y);

		Serial.print("  P: ");
		int disPhoto = p>>5;
		disPhoto = disPhoto & 0b0000000000000001;
		Serial.print(disPhoto);
		Serial.println(" (disable photo diodes)");

		Serial.print("  S: ");
		int s = p>>4;
		s = s & 0b0000000000000001;
		Serial.print(s);
		Serial.println(" (measurement enabled)");

		Serial.print("  Z: ");
		int z = p>>3;
		z = z & 0b0000000000000001;
		Serial.print(z);
		Serial.println(" (analog enabled)");

		Serial.print("DDD: ");
		int ddd = p>>3;
		ddd = ddd & 0b0000000000000111;
		Serial.print(ddd);
		Serial.println(" (analog mode)"); 
		
	} // wrong responses:
	  else if (p == 0b1010011111111111) {
		Serial.print("Slave: SPI Error: Wrong response to xy ");
		serBits(p);	
	} else {
		Serial.print("Slave: SPI Error: Unknown message ");
		serBits(p);
	}
	// Serial.println();
}

/* end parsing */

/* bit helpers */

unsigned long int combine(unsigned int left, unsigned int right)
{ // combine two 8 bit words to one 16 bit word
	return (left<<8)+right;
}

void serBits(unsigned long int x) 
{ // x must be 16 bits
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
