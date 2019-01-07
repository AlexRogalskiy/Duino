
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char *ssid =  "MacBook-Pro-Victor";            // cannot be longer than 32 characters!
const char *pass =  "19101966";       // WiFi password

String prefix   = "/IoTmanager";     // global prefix for all topics - must be some as mobile device
String deviceID = "SmarhouseNodemcu";   // thing ID - unique device id in our project

WiFiClient wclient;

// config for cloud mqtt broker by DNS hostname ( for example, cloudmqtt.com use: m20.cloudmqtt.com - EU, m11.cloudmqtt.com - USA )
String mqttServerName = "m13.cloudmqtt.com";            // for cloud broker - by hostname, from CloudMQTT account data
int    mqttport = 18274;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
String mqttuser =  "nodemcu1";                              // from CloudMQTT account data
String mqttpass =  "191066";                              // from CloudMQTT account data
PubSubClient client(wclient, mqttServerName, mqttport); // for cloud broker - by hostname

// подключение библиотеки DTH
#include "DHT.h"
// константы
#define DHTPIN 4           // пин (D2) подключения контакта DATA
#define DHTTYPE DHT22      // датчик DHT 22 
// создание экземпляра объекта DHT
DHT dht(DHTPIN, DHTTYPE);

#define DSD18B20PIN 5      // пин подключения контакта DATA (D1, GPIO5)
// подключение библиотеки
#include <OneWire.h>
// создание экземпляра OneWire
OneWire  ds(DSD18B20PIN); 


String val;
String ids = "";
int oldtime, newtime, pushtime, oldtime2;
int newValue;

const String stat1 = "{\"status\":\"1\"}";
const String stat0 = "{\"status\":\"0\"}";

const int nWidgets = 2;
String stat        [nWidgets];
String sTopic      [nWidgets];
String color       [nWidgets];
String style       [nWidgets];
String badge       [nWidgets];
String widget      [nWidgets];
String descr       [nWidgets];
String page        [nWidgets];
String thing_config[nWidgets];
String id          [nWidgets];
int    pin         [nWidgets];
float    defaultVal  [nWidgets];
bool   inverted    [nWidgets];

// Push notifications
const char* host = "onesignal.com";
WiFiClientSecure httpClient;
const int httpsPort = 443;
String url = "/api/v1/notifications";

void push(String msg) {
  Serial.println("PUSH: try to send push notification...");
  if (ids.length() == 0) {
     Serial.println("PUSH: ids not received, push failed");
     return;
  }
  if (!httpClient.connect(host, httpsPort)) {
     Serial.println("PUSH: connection failed");
     return;
  }
  String data = "{\"app_id\": \"8871958c-5f52-11e5-8f7a-c36f5770ade9\",\"include_player_ids\":[\"" + ids + "\"],\"android_group\":\"IoT Manager\",\"contents\": {\"en\": \"" + msg + "\"}}";
  httpClient.println("POST " + url + " HTTP/1.1");
  httpClient.print("Host:");
  httpClient.println(host);
  httpClient.println("User-Agent: esp8266.Arduino.IoTmanager");
  httpClient.print("Content-Length: ");
  httpClient.println(data.length());
  httpClient.println("Content-Type: application/json");
  httpClient.println("Connection: close");
  httpClient.println();
  httpClient.println(data);
  httpClient.println();
  Serial.println(data);
  Serial.println("PUSH: done.");
}
String setStatus ( String s ) {
  String stat = "{\"status\":\"" + s + "\"}";
  return stat;
}
String setStatus ( int s ) {
  String stat = "{\"status\":\"" + String(s) + "\"}";
  return stat;
}
void initVar() {


  id    [0] = "0";
  page  [0] = "SensorsNodemcu";
  descr  [0] = "DHT22";
  widget[0] = "small-badge";
  //pin   [0] = A0;                                          // ADC
  sTopic[0] = prefix + "/" + deviceID + "/DHT22";
  badge [0] = "\"badge\":\"badge-calm\"";                  // see http://ionicframework.com/docs/components/#colors
  style [0]   = "\"style\":\"font-size:150%;\"";

  id    [1] = "1";
  page  [1] = "SensorsNodemcu";
  descr  [1] = "DS18b20";
  widget[1] = "small-badge";
  //pin   [1] = A0;                                          // ADC
  sTopic[1] = prefix + "/" + deviceID + "/DS18b20";
  badge [1] = "\"badge\":\"badge-calm\"";                  // see http://ionicframework.com/docs/components/#colors
  style [1]   = "\"style\":\"font-size:150%;\"";
  

  for (int i = 0; i < nWidgets; i++) {
    if (inverted[i]) {
      if (defaultVal[i]>0) {
         stat[i] = setStatus(0);
      } else {
         stat[i] = setStatus(1);
      }
    } else {
       stat[i] = setStatus(defaultVal[i]);
    }
  }      

  thing_config[0] = "{\"id\":\"" + id[0] + "\",\"page\":\"" + page[0]+"\",\"descr\":\"" + descr[0] + "\",\"widget\":\"" + widget[0] + "\",\"topic\":\"" + sTopic[0] + "\"," + badge[0] + "," + style[0] + "}";  // DHT11
  thing_config[1] = "{\"id\":\"" + id[1] + "\",\"page\":\"" + page[1]+"\",\"descr\":\"" + descr[1] + "\",\"widget\":\"" + widget[1] + "\",\"topic\":\"" + sTopic[1] + "\"," + badge[1] + "," + style[1] + "}";  // ds18b20
  //thing_config[2] = "{\"id\":\"" + id[2] + "\",\"page\":\"" + page[2]+"\",\"descr\":\"" + descr[2] + "\",\"widget\":\"" + widget[2] + "\",\"topic\":\"" + sTopic[2] + "\"," + color[2] + "}";   // GPIO switched On/Off by mobile widget toggle

}
// send confirmation
void pubStatus(String t, String payload) {  
    if (client.publish(t + "/status", payload)) { 
       Serial.println("Publish new status for " + t + ", value: " + payload);
    } else {
       Serial.println("Publish new status for " + t + " FAIL!");
    }
}
void pubConfig() {
  bool success;
  success = client.publish(MQTT::Publish(prefix, deviceID).set_qos(1));
  if (success) {
      delay(500);
      for (int i = 0; i < nWidgets; i = i + 1) {
        success = client.publish(MQTT::Publish(prefix + "/" + deviceID + "/config", thing_config[i]).set_qos(1));
        if (success) {
          Serial.println("Publish config: Success (" + thing_config[i] + ")");
        } else {
          Serial.println("Publish config FAIL! ("    + thing_config[i] + ")");
        }
        delay(150);
      }      
  }
  if (success) {
     Serial.println("Publish config: Success");
  } else {
     Serial.println("Publish config: FAIL");
  }
  for (int i = 0; i < nWidgets; i = i + 1) {
      pubStatus(sTopic[i], stat[i]);
      delay(100);
  }      
}
void callback(const MQTT::Publish& sub) {
  Serial.print("Get data from subscribed topic ");
  Serial.print(sub.topic());
  Serial.print(" => ");
  Serial.println(sub.payload_string());

  if (sub.topic() == sTopic[2] + "/control") {
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[2] = stat0;
    } else {
       newValue = 0; // inverted
       stat[2] = stat1;
    }
    digitalWrite(pin[2],newValue);
    pubStatus(sTopic[2], stat[2]);
    } else if (sub.topic() == sTopic[0] + "/control") {
   // ADC : nothing, display only
    } else if (sub.topic() == sTopic[1] + "/control") {
   // ADC : nothing, display only
    } else if (sub.topic() == prefix) {
    if (sub.payload_string() == "HELLO") {
      pubConfig();
    }
 }
}

void setup() {
  initVar();
  pinMode(pin[0], INPUT);
  stat[0] = setStatus(digitalRead(pin[0]));
  pinMode(pin[1], INPUT);
  stat[1] = setStatus(digitalRead(pin[1]));
  // Setup console
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("MQTT client started.");
  Serial.print("Free heap = ");
  Serial.println(ESP.getFreeHeap());
  dht.begin();         // запуск DHT
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting via WiFi to ");
    Serial.print(ssid);
    Serial.println("...");

    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      return;
    }

    Serial.println("");
    Serial.println("WiFi connect: Success");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      Serial.println("Connecting to MQTT server ...");
      bool success;
      if (mqttuser.length() > 0) {
        success = client.connect( MQTT::Connect( deviceID ).set_auth(mqttuser, mqttpass) );
      } else {
        success = client.connect( deviceID );
      }
      if (success) {
        client.set_callback(callback);
        Serial.println("Connect to MQTT server: Success");
        pubConfig();


        Serial.println("Subscribe: Success");
      } else {
        Serial.println("Connect to MQTT server: FAIL");   
        delay(1000);
      }
    }

    if (client.connected()) {
      newtime = millis();
      if (newtime - oldtime > 10000) { // 10 sec
        float x = get_data_humidity();
        val = "{\"status\":\"" + String(x)+ "\"}";
        client.publish(sTopic[0] + "/status", val );  // widget 0
        x = get_data_ds18b20();
        val = "{\"status\":\"" + String(x)+ "\"}";
        client.publish(sTopic[1] + "/status", val );  // widget 1
        oldtime = newtime;
      }
      if (newtime - oldtime2 > 60000) { // 60 sec
        pubConfig();
        float x = get_data_humidity();
        val = "{\"status\":\"" + String(x)+ "\"}";
        client.publish(sTopic[0] + "/status", val );  // widget 0
        x = get_data_ds18b20();
        val = "{\"status\":\"" + String(x)+ "\"}";
        client.publish(sTopic[1] + "/status", val );  // widget 1
        oldtime2 = newtime;
      }
      client.loop();
    }
  }
}

float get_data_humidity() {
   float h = dht.readHumidity();
   return h;
}

float get_data_ds18b20(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float fTemp;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return 999;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return 999;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return 999;
  } 

  ds.reset();
  ds.select(addr);
  // запустить конвертацию температуры датчиком
  ds.write(0x44, 1);          
  delay(1000);     // ждем 750 мс
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);       
  // считываем ОЗУ датчика
  for ( i = 0; i < 9; i++) {           
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  // перевод полученных данных в значение температуры
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; 
    if (data[7] == 0x10) {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  
    else if (cfg == 0x20) raw = raw & ~3; 
    else if (cfg == 0x40) raw = raw & ~1; 
  }
  fTemp = (float)raw / 16.0;

  return fTemp;
}

