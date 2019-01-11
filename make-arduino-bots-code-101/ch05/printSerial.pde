// printSerial.pde - Print test data to serial port.    
// (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com

void setup()
{
    Serial.begin(9600); // bit/s
}

void loop()
{
    Serial.print("F ");
    delay(500);
    Serial.print("B ");
    delay(1000);
    Serial.print(" FFFF ");
    delay(500);
    Serial.print(" http://botbook.com ");
    delay(2000);
}
