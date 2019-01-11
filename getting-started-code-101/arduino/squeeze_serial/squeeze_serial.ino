// squeeze_serial.ino - flexiforce squeeze level to serial
// (c) BotBook.com - Karvinen, Karvinen, Valtokari

int squeezePin=A0;	// <1>
int x=-1; // 0..1023  // <2>

void setup() {	// <3>
  Serial.begin(9600); // bit/s // <4>
}

void loop() {	// <5>
  x=analogRead(squeezePin);  // <6>
  Serial.println(x);  // <7>
  delay(500); // ms	// <8>
}

