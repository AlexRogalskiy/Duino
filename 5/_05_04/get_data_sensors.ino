// Процедуры получения 
// данных с датчиков
//

// ************************************************
// получение данных влажности с DHT
float get_data_humidity() {
   float h = dht.readHumidity();
   return h;
}

// ************************************************
// получение данных с датчика DS18B20
float get_data_ds18b20()  {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  int Temp;
  float fTemp=0.0;
  
  if ( !ds.search(addr)) {
    Serial.print("No more addresses.\n");
    ds.reset_search();
    return 999;
  }
  // вывод в монитор уникального адреса 1-Wire устройства
  for( i = 0; i < 8; i++)  {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }
  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.print("CRC is not valid!\n");
    return 999;
  }
  if ( addr[0] != 0x28) {
    Serial.print("Device is not a DS18S20 family device.\n");
    return 999;
  }
  ds.reset();
  ds.select(addr);
  // запустить конвертацию температуры датчиком
  ds.write(0x44,1);
  delay(750); // ждем 750 мс
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); 
  // считываем ОЗУ датчика
  for ( i = 0; i < 9; i++) { 
    data[i] = ds.read();
  }
  Temp=((data[1]<<8)+data[0]);
  fTemp=1.0*Temp/16+(float(Temp%16))*1.0/16;

  return fTemp;
}

// ************************************************
// получение данных с датчика SoilMoisture
float get_data_soilmoisture()  {
  // выбор входа мультиплексора CD4051 – y0 (000)
  digitalWrite(14,LOW);
  digitalWrite(13,LOW);
  digitalWrite(15,LOW);
  // получение значения с аналогового вывода датчика
  int avalue=analogRead(SOILMOISTUREPIN);
  Serial.print("avalue =");Serial.println(avalue);
  // масштабируем значение в проценты
  avalue=constrain(avalue, MINVALUESOILMOISTURE,MAXVALUESOILMOISTURE);
  int moisture=map(avalue, MINVALUESOILMOISTURE, 
                    MAXVALUESOILMOISTURE,100,0);
  return (float)moisture;
}

// ************************************************
// получение данных с датчика уровня воды
float get_data_levelwater()  {
  // выбор входа мультиплексора CD4051 – y1 (001)
  digitalWrite(14,HIGH);
  digitalWrite(13,LOW);
  digitalWrite(15,LOW);
  // получение значения с аналогового вывода датчика
  int avalue=analogRead(LEVELWATERPIN);
  return (float) avalue;
}
// ************************************************
// получение данных содержания пропана с датчика MQ2
float get_data_ppmpropan()  {
  // выбор входа мультиплексора CD4051 – y2 (010)
  digitalWrite(14,LOW);
  digitalWrite(13,HIGH);
  digitalWrite(15,LOW);    
  Serial.println(mq2.readRatio());
  // получение значения 
  float value=mq2.readLPG();
  
  return value;
}
// получение данных содержания метана с датчика MQ2
float get_data_ppmmethan()  {
  // выбор входа мультиплексора CD4051 – y2 (010)
  digitalWrite(14,LOW);
  digitalWrite(13,HIGH);
  digitalWrite(15,LOW);    
  Serial.println(mq2.readRatio());
  // получение значения 
  float value=mq2.readMethane();
  
  return value;
}
// получение данных содержания дыма с датчика MQ2
float get_data_ppmsmoke()  {
  // выбор входа мультиплексора CD4051 – y2 (010)
  digitalWrite(14,LOW);
  digitalWrite(13,HIGH);
  digitalWrite(15,LOW);    
  Serial.println(mq2.readRatio());
  // получение значения 
  float value=mq2.readSmoke();
  
  return value;
}
// ************************************************
// получение данных с датчика MQ7
float get_data_ppmcarbonmonoxide()  {
      // выбор входа мультиплексора CD4051 – y3 (011)
      digitalWrite(14,HIGH);
      digitalWrite(13,HIGH);
      digitalWrite(15,LOW);     
  Serial.println(mq7.readRatio());
  // получение значения 
  float value=mq7.readCarbonMonoxide();
  
  return value;
}
