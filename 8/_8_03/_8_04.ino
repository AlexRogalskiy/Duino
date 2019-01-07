#include <ESP8266WiFi.h>
 
const char* ssid     = "my_point ";
const char* password = "my_point pass ";
 
const char* host = "data.sparkfun.com";
const char* streamId   = "---------------------";
const char* privateKey = "---------------------";
 
void setup() {
  Serial.begin(115200);
  delay(10000);
 
  // подсоединение к WiFi точке доступа
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
 
float value = 0;
 
void loop() {
  delay(10000);
 
  Serial.print("connecting to ");
  Serial.println(host);
   
  // создаем TCP соединение
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
   
  // создаем строку URL
  String url = "/input/";
  url += streamId;
  url += "?private_key=";
  url += privateKey;
  url += "&battery=";
  value=3.3*analogRead(A0)/1024;
  url += value;
   
  Serial.print("Requesting URL: ");
  Serial.println(url);
   
  // отправить данные на сервер
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
   
  // ответ сервера
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
   
  Serial.println();
  Serial.println("closing connection");
}
