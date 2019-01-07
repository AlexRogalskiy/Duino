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
// вывод главного экрана
void view_display_main() {
  // очистить экран
  tft.fillScreen(BLACK);
  // вывод заголовка экрана
  tft.setTextColor(MAGENTA);  
  tft.setTextSize(3);
  for(int i=0;i<6;i++) {
     tft.setCursor(15, xmenu[i]);
     tft.print(strM1[i]);
  } 
}
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
  // ardyino-kit  
  tft.setTextSize(3);
  tft.setTextColor(MAGENTA);  
  tft.setCursor(10, 270);
  tft.print("arduino-kit");
  // вывод для возврата в главное меню  
}
// ************************************************
// вывести текущее значение для датчика
void set_display_data_sensor(int pos,float var,unsigned int color) {
   tft.setTextColor(color);  
   tft.setCursor(100, xsensors[pos]);
   tft.print(var);

}

