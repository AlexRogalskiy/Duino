// button.ino - light a led by pressing button
// (c) BotBook.com - Karvinen, Karvinen, Valtokari

int buttonPin=2; // <1>
int ledPin=13;	// <2>
int buttonStatus=LOW;	// <3>

void setup()	// <4>
{
  pinMode(ledPin, OUTPUT);	// <5>
  pinMode(buttonPin, INPUT); // <6>
  digitalWrite(buttonPin, HIGH); // internal pull-up // <7>
}

void loop() // <8>
{
  buttonStatus=digitalRead(buttonPin); // <9>
  if (LOW==buttonStatus) { // <10>
    digitalWrite(ledPin, HIGH); // <11>
  } else {	// <12>
    digitalWrite(ledPin, LOW);	// <13>
  }
} // <14>

