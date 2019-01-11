// serial.ino - send text from Arduino to computer serial monitor
// (c) BotBook.com - Karvinen, Karvinen, Valtokari

void setup()	// 1
{
  Serial.begin(9600); // bit/s	// 2
}

void loop()	// 3
{
  Serial.println("Greetings from Arduino, see you at botbook.com! "); // 4
  delay(1000); // 5
}

