#ifndef defines_h
#define defines_h

//// константы
#define DHTPIN 22           // пин подключения контакта DHT22
#define DS18B20PIN 23           // пин подключения контакта DS18B20
#define DHTTYPE DHT11      // датчик DHT 11
#define SOILMOISTUREPIN A8      // пин подключения контакта A0
// значение полного полива
#define MINVALUESOILMOISTURE 220
// значение критической сухости
#define MAXVALUESOILMOISTURE 900
#define LEVELWATERPIN A9      // пин подключения контакта S
// пороговое значение протечки 
#define LEVELWATER 100
// пин, к которому подключен датчик MQ-2
#define MQ2PIN         A10
// пин, к которому подключен датчик MQ-7
#define MQ7PIN         A11

// ********************************************
// параметры датчиков для срабатывания
// светодиодной индикации и звуковой сигнализации

#define LIMIT_DHT22MIN 18             // влажность воздуха низкая
#define LIMIT_DHT22MAX 90             // влажность воздуха высокая
#define LIMIT_DS18B20MIN 15           // температура низкая
#define LIMIT_DS18B20MAX 30           // температура высокая
#define LIMIT_SOILMOISTUREMIN 300     // влажность почвы - чрезмерная влажность
#define LIMIT_SOILMOISTUREMAX 600     // влажность почвы - сухость
#define LIMIT_LEVELWATER 100          // вода
#define LIMIT_FAIR 100                // огонь
#define LIMIT_MQ2P 30                 // пропан
#define LIMIT_MQ2M 30                 // метан
#define LIMIT_MQ2S 30                 // дым
#define LIMIT_MQ7 30                  // CO

// пин, к которому подключен динамик
#define SOUNDPIN         42

//*********************************************

#define COUNT_SENSORS 10  // макс. кол-во датчиков
#define COUNT_RELAYS 8  // макс. кол-во реле
#define RELAY_ON 1      // для включения реле
#define RELAY_OFF 0      // для выключения реле

#define INTERVAL_GET_DATA 5000  // интервала измерений, мс

// цвета для дисплея
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// для экрана menu
char strM1[10][12]={"           ","  SENSORS  ","  RELAYS   ",
                    "  ALARMS   ",
                   "arduino-kit"
                   };
// для экрана сенсоров
char strS1[10][6]={"Humid","Temp ","Soil ","Water","Fire ",
                   "GProp","GMeth","Smoke","CO   ","SR501"
                   };
char strS2[10][4]={" % "," \xF7\x43"," % ","   ","   ",
                   "ppm","ppm","ppm","ppm","   " 
                   };
// для экрана реле
char strR1[8][6]={"Light","Pump "," Fan ","  -  ","  -  ",
                   "  -  ","  -  ","  -  "
                   };

// для экрана будильников

// сенсорный экран
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define MINPRESSURE 50
#define MAXPRESSURE 1000



#endif
