// distance_ping.ino - distance using ultrasonic ping sensor
// (c) BotBook.com - Karvinen, Karvinen, Valtokari

int pingPin = 2;
float v=331.5+0.6*20; // m/s  // 1

void setup()
{
  Serial.begin(9600);
}

float distanceM(){
  // send sound pulse  // 2
  pinMode(pingPin, OUTPUT); // 3
  digitalWrite(pingPin, LOW);
  delayMicroseconds(3); // 4
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5); // 5
  digitalWrite(pingPin, LOW);

  // listen for echo
  pinMode(pingPin, INPUT); 
  float tUs = pulseIn(pingPin, HIGH); // microseconds  // 6
  float t = tUs / 1000.0 / 1000.0; // s  // 7
  return t/2 * v; // m  // 8
}

void loop()
{
  Serial.println(distanceM()*100, DEC);  // 9
  delay(200); // ms
}
