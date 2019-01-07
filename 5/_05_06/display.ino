// Процедуры вывода данных
// на дисплей
// shield TFT 2.4 240x320

// ************************************************
// инициализация дисплея
void ini_display() {
  tft.reset();
  uint16_t identifier = tft.readID();  
  Serial.print(F("LCD driver chip: "));
  Serial.println(identifier, HEX);
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setRotation(0);
  Serial.println(F("start!"));
}
// ************************************************
// вывод экрана с показаниями датчиков
void view_display_sensors() {
  // очистить экран
  tft.fillScreen(BLACK);
  // вывод заголовка экрана
  tft.setCursor(10, 15);
  tft.setTextColor(MAGENTA);  
  tft.setTextSize(3);
  tft.println("Sensors:");
  // вывод списка датчиков
  tft.setTextColor(YELLOW);  
  tft.setTextSize(2);
  for(int i=0;i<COUNT_SENSORS;i++) {
     tft.setCursor(10, xsensors[i]);
     tft.print(strS1[i]);
  } 
  // вывод списка значений датчиков
 
  // вывод единиц измерения
  tft.setTextColor(YELLOW);  
  tft.setTextSize(2);
  for(int i=0;i<COUNT_SENSORS;i++) {
     tft.setCursor(200, xsensors[i]);
     tft.print(strS2[i]);
  }
  // вывод для возврата в главное меню  
}
// ************************************************
// вывести текущее значение для датчика
void set_display_data_sensor(int pos,float var,unsigned int color) {
   tft.setTextColor(color);  
   tft.setCursor(100, xsensors[pos]);
   tft.print(var);

}

