/*
 Basic MQTT example 
 
  - connects to an MQTT server
  - publishes "hello world" to the topic "potato"
  - subscribes to the topic "batata"
*/

#define GSMAPN "internet.mts.ru" //APN
#define GSMUSER "mts"            //APN USER
#define GSMPASSWORD "mts"        //APN PASSWORD

#include <Time.h>
#include <sim800Client.h>
#include <PubSubClientHotlog.h>
#include <TimeAlarms.h>

sim800Client s800;
char imeicode[16];

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  char mypl[48];
  Serial.println(length);
  memcpy(mypl,payload,length);
  mypl[length]=char(0);
  Serial.print("receive: ");
  Serial.print(topic);
  Serial.print("->");
  Serial.println(mypl);
}

String prefix   = "/IoTmanager";     // global prefix for all topics - must be some as mobile device
String deviceID = "SmarhouseArduinoMega";   // thing ID - unique device id in our project
char server[] = "m13.cloudmqtt.com";  //Mqtt Server
int    mqttport = 18274;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
String mqttuser =  "nodemcu1";                              // from CloudMQTT account data
String mqttpass =  "191066";                              // from CloudMQTT account data

PubSubClient client(server, mqttport, callback, s800); // Pass the server, port, callback, and client.
unsigned long millis1=0;

// подключение библиотеки DTH
#include "DHT.h"
// константы
#define DHTPIN 22           // пин (D2) подключения контакта DATA
#define DHTTYPE DHT22      // датчик DHT 22 
// создание экземпляра объекта DHT
DHT dht(DHTPIN, DHTTYPE);

const int nWidgets = 1;
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

void pub()
{
   char charBufVar1[50];
   char charBufVar2[20];
   sTopic[0].toCharArray(charBufVar1, 50); 
   Serial.println("Publish data");
   pubConfig();
   float x = get_data_humidity();
   String val = "{\"status\":\"" + String(x)+ "\"}";
   val.toCharArray(charBufVar2, 50); 
   client.publish(charBufVar1,charBufVar2);  // widget 0
 }
void pubConfig() {
   char charBufVar1[50];
   char charBufVar2[100];
   Serial.println("Publish config");
   String s1=prefix + "/" + deviceID + "/config";
   s1.toCharArray(charBufVar1, 50); 
   thing_config[0].toCharArray(charBufVar2, 50); 
   client.publish(charBufVar1,charBufVar2);  // config
}

void setup()
{
  
  Serial.begin(9600);
  Serial.println("SIM800 Shield testing.");
  for (int i=0; i<10; i++){
    delay(5000);
    Serial.println("try to init sim800");

#ifdef HARDWARESERIAL
    if (s800.init( 7, 6)) break; // RX, Tx GPRS IcomsatV1.1
#else
    if (s800.init(&Serial1)) break;
#endif

  }
  initVar();
  Serial.println("try to setup sim800");
  s800.setup();
  s800.stop();
  s800.TCPstop();
  s800.getIMEI(imeicode);
  Serial.print("IMEI: ");
  Serial.println(imeicode);


  while (s800.TCPstart(GSMAPN,GSMUSER,GSMPASSWORD)) {  //receive the response 7 true 0  false.
    Serial.println("TCPstart failed");
    s800.TCPstop();
    delay(1000);
  }
  Serial.println("TCPstart started");

 while (client.connect("arduinokit", "nodemcu1", "191066")) {
    Serial.println("connect failed");
    delay(1000);
  }

  Serial.println("connected");
  client.publish("potato","hello world");// topico potato, msg helloworld.
  client.subscribe("potato"); // topic potato.
  Alarm.timerRepeat(5, pub);             // timer

}

void loop()
{

    client.loop();
    Alarm.delay(10000); 

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
  

    if (inverted[0]) {
      if (defaultVal[0]>0) {
         stat[0] = setStatus(0);
      } else {
         stat[0] = setStatus(1);
      }
    } else {
       stat[0] = setStatus(defaultVal[0]);
    }
      

  thing_config[0] = "{\"id\":\"" + id[0] + "\",\"page\":\"" + page[0]+"\",\"descr\":\"" + descr[0] + "\",\"widget\":\"" + widget[0] + "\",\"topic\":\"" + sTopic[0] + "\"," + badge[0] + "," + style[0] + "}";  // DHT11

}
String setStatus ( String s ) {
  String stat = "{\"status\":\"" + s + "\"}";
  return stat;
}
String setStatus ( int s ) {
  String stat = "{\"status\":\"" + String(s) + "\"}";
  return stat;
}

float get_data_humidity() {
   //float h = dht.readHumidity();
   return 43;
}

