/*
 IoT Manager mqtt device client https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
 Based on Basic MQTT example with Authentication
 PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  - connects to an MQTT server, providing userdescr and password
  - publishes config to the topic "/IoTmanager/config/deviceID/"
  - subscribes to the topic "/IoTmanager/hello" ("hello" messages from mobile device) 
  Tested with Arduino IDE 1.6.6 + ESP8266 Community Edition v 2.0.0-stable and PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  ESP8266 Community Edition v 2.0.0-stable have some HTTPS issues. Push notification temporary disabled.
  sketch version : 1.5
  IoT Manager    : any version
  toggle, range, small-badge and power-button widgets demo
//////////////////////////////////////////////////////////
//  Attention for ESP-01 users!
//  at line 167 change value 15 (GPIO15) to another.
//  (on ESP-01 GPIO15 pin connected directly to GND and "red" PWM out may overload GPIO15 line). Repoted by grigorygn http://www.esp8266.com/viewtopic.php?p=40407#p40407
//////////////////////////////////////////////////////////
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include <Wire.h>

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


// config for local mqtt broker by IP address
//IPAddress server(192, 168, 1, 100);                        // for local broker - by address
//int    mqttport = 1883;                                    // default 1883
//String mqttuser =  "";                                     // from broker config
//String mqttpass =  "";                                     // from broker config
//PubSubClient client(wclient, server, mqttport);            // for local broker - by address

String val;
String ids = "";
int oldtime, newtime, pushtime, oldtime2;
int newValue;

const String stat1 = "{\"status\":\"1\"}";
const String stat0 = "{\"status\":\"0\"}";

const int nWidgets = 5;
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

int datarelays[]={0,0,0,0,0,0,0,0};

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
  
  id    [2] = "2";
  page  [2] = "RelaysNodeMcu";
  descr [2] = "Light";
  widget[2] = "toggle";
  //pin[0] = 4;                                            
  defaultVal[2] = 0;                                       // defaultVal status
  inverted[2] = false;
  sTopic[2]   = prefix + "/" + deviceID + "/light";
  color[2]   = "\"color\":\"red\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [3] = "3";
  page  [3] = "RelaysNodeMcu";
  descr [3] = "Pump";
  widget[3] = "toggle";
  //pin[0] = 4;                                            
  defaultVal[3] = 0;                                       // defaultVal status
  inverted[3] = false;
  sTopic[3]   = prefix + "/" + deviceID + "/pump";
  color[3]   = "\"color\":\"red\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [4] = "4";
  page  [4] = "RelaysNodeMcu";
  descr [4] = "Fun";
  widget[4] = "toggle";
  //pin[0] = 4;                                            
  defaultVal[4] = 0;                                       // defaultVal status
  inverted[4] = false;
  sTopic[4]   = prefix + "/" + deviceID + "/fun";
  color[4]   = "\"color\":\"red\"";                       // black, blue, green, orange, red, white, yellow (off - grey)


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
  thing_config[2] = "{\"id\":\"" + id[2] + "\",\"page\":\"" + page[2]+"\",\"descr\":\"" + descr[2] + "\",\"widget\":\"" + widget[2] + "\",\"topic\":\"" + sTopic[2] + "\"," + color[2] + "}";
  thing_config[3] = "{\"id\":\"" + id[3] + "\",\"page\":\"" + page[3]+"\",\"descr\":\"" + descr[3] + "\",\"widget\":\"" + widget[3] + "\",\"topic\":\"" + sTopic[3] + "\"," + color[3] + "}";
  thing_config[4] = "{\"id\":\"" + id[4] + "\",\"page\":\"" + page[4]+"\",\"descr\":\"" + descr[4] + "\",\"widget\":\"" + widget[4] + "\",\"topic\":\"" + sTopic[4] + "\"," + color[4] + "}";
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
    } else if (sub.topic() == sTopic[2] + "/control") {
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[2] = stat0;
    } else {
       newValue = 0; // inverted
       stat[2] = stat1;
    }
    datarelays[0]= newValue;
    set_status_relays();
    pubStatus(sTopic[2], stat[2]);
    } else if (sub.topic() == sTopic[3] + "/control") {
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[3] = stat0;
    } else {
       newValue = 0; // inverted
       stat[3] = stat1;
    }
    datarelays[1]= newValue;
    set_status_relays();
    pubStatus(sTopic[3], stat[3]);
    } else if (sub.topic() == sTopic[4] + "/control") {
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[4] = stat0;
    } else {
       newValue = 0; // inverted
       stat[4] = stat1;
    }
    datarelays[2]= newValue;
    set_status_relays();
    pubStatus(sTopic[4], stat[4]);
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
  
  Wire.beginTransmission(0x20);  // i2c – адрес (A0-0,A1-0,A2-0)
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // настроить PORT A как output (для светодиодов)
  Wire.endTransmission();
  Wire.beginTransmission(0x20);  
  Wire.write(0x01); // IODIRB register
  Wire.write(0x00); // настроить PORT B как output (для реле)
  Wire.endTransmission();
  
  Serial.println();
  Serial.println();
  Serial.println("MQTT client started.");
  Serial.print("Free heap = ");
  Serial.println(ESP.getFreeHeap());
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
        client.subscribe(sTopic[2] + "/control"); // for receiving GPIO messages
        client.subscribe(sTopic[3] + "/control"); // for receiving GPIO messages
        client.subscribe(sTopic[4] + "/control"); // for receiving GPIO messages


        Serial.println("Subscribe: Success");
      } else {
        Serial.println("Connect to MQTT server: FAIL");   
        delay(1000);
      }
    }

    if (client.connected()) {
      newtime = millis();
      if (newtime - oldtime > 10000) { // 10 sec
        float x = random(40,45);
        val = "{\"status\":\"" + String(x)+ "\"}";
        client.publish(sTopic[0] + "/status", val );  // widget 0
        x = random(18,20);
        x=x+(x+random(100))/100;
        val = "{\"status\":\"" + String(x)+ "\"}";
        client.publish(sTopic[1] + "/status", val );  // widget 1
        oldtime = newtime;
      }
      if (newtime - oldtime2 > 60000) { // 60 sec
        pubConfig();
        float x = random(40,45);
        val = "{\"status\":\"" + String(x)+ "\"}";
        client.publish(sTopic[0] + "/status", val );  // widget 0
        x = random(18,20);
        x=x+(x+random(100))/100;
        val = "{\"status\":\"" + String(x)+ "\"}";
        client.publish(sTopic[1] + "/status", val );  // widget 1
        oldtime2 = newtime;
      }
      client.loop();
    }
  }
}

void set_status_relays() {
  int relays=0;
  for(int i=0;i<7;i++)
     relays=relays +( datarelays[i]<<i);
  // записать данные в PORT B
  Wire.beginTransmission(0x20);
  Wire.write(0x13); // address PORT B
  Wire.write(relays);    // PORT B
  Wire.endTransmission();
  delay(100); // пауза
}

