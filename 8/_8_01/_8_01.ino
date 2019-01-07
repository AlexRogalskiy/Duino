#define SIM800board Serial1
#define SIM800baud 9600

String hh;
char buffer[100];
char end_c[2];

void setup() {
  Serial.begin(9600);
  SIM800board.begin(SIM800baud);
  Serial.println("Start");
  end_c[0] = 0x1a;
  end_c[1] = '\0';
  delay(2000);
}

void loop() {
  if (Serial.available()) {
    hh = Serial.readStringUntil('\n');
    hh.toCharArray(buffer, hh.length() + 1);
    if (hh.indexOf("end") == 0) {
      SIM800board.write(end_c);
      Serial.println("end");
    } else{
      SIM800board.write(buffer);
      SIM800board.write('\n');
    }
  }
  if (SIM800board.available()) {
    Serial.write(SIM800board.read());
  }
}
