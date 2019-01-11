// pot.ino - control LED blinking speed with potentiometer
// (c) BotBook.com - Karvinen, Karvinen, Valtokari

int potPin=A0; // <1>
int ledPin=13;	// <2>
int x=0; // 0..1023 // <3>

void setup() {	// <4>
  pinMode(ledPin, OUTPUT);	// <5>
}

void loop() {	// <6>
  x=analogRead(potPin); // <7>
  digitalWrite(ledPin, HIGH);	// <8>
  delay(x/10); // <9>
  digitalWrite(ledPin, LOW);	// <10>
  delay(x/10);	// <11>
} // <12>

