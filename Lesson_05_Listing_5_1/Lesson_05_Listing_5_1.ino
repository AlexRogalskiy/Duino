const int RED=11; // вывод красной ноги RGB-светодиода
const int GREEN=10; // вывод зеленой ноги RGB-светодиода
const int BLUE=9; // вывод синей ноги RGB-светодиода
int red; // переменная для хранения R-составляющей цвета
int green; // переменная для хранения G-составляющей цвета
int blue; // переменная для хранения B-составляющей цвета
void setup()
{;}
void loop()
{
// от красного к желтому
red=255;green=0;blue=0;
for(green=0;green<=255;green++)
setRGB(red,green,blue);
// от желтому к зеленому
for(red=255;red>=0;red--)
setRGB(red,green,blue);
// от зеленого к голубому
for(blue=0;blue<=255;blue++)
setRGB(red,green,blue);
// от голубого к синему
for(green=255;green>=0;green--)
setRGB(red,green,blue);
// от синего к фиолетовому
for(red=0;red<=255;red++)
setRGB(red,green,blue);
delay(2000);
}
// функция установки цвета RGB-светодиода
void setRGB(int r,int g,int b)
{
analogWrite(RED,r);
analogWrite(GREEN,g);
analogWrite(BLUE,b);
delay(10);
}
