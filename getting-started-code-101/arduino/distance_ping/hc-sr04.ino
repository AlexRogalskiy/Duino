// hc-sr04.ino - distance using ultrasonic ping sensor
// (c) BotBook.com - Karvinen, Karvinen, Valtokari


int trigPin = 2;	// 1
int echoPin = 3;	// 2
float v=331.5+0.6*20; // m/s	// 3

void setup()	// 4
{
  Serial.begin(115200);	// 5
  pinMode(trigPin, OUTPUT);	// 6
  pinMode(echoPin, INPUT);	// 7
}

float distanceCm(){	// 8
  // send sound pulse	// 9
  digitalWrite(trigPin, LOW);	// 10
  delayMicroseconds(3);	// 11
  digitalWrite(trigPin, HIGH);	// 12
  delayMicroseconds(5);	// 13
  digitalWrite(trigPin, LOW);	// 14

  // listen for echo 	// 15
  float tUs = pulseIn(echoPin, HIGH); // microseconds	// 16
  float t = tUs / 1000.0 / 1000.0 / 2.0; // s	// 17
  float d = t*v; // m	// 18
  return d*100; // cm	// 19
}

void loop()	// 20
{
  int d=distanceCm(); // 21
  Serial.println(d, DEC);	// 22
  delay(200); // ms	// 23
}

