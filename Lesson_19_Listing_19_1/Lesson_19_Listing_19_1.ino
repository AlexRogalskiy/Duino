#include <Stepper.h>
#define STEPS 200 // Количество шагов
Stepper stepper(STEPS, 8, 9, 10, 11);
// клавиши
int pinButtons1[]={6,7};
int lastButtons1[]={0,0};
int currentButtons1[]={0,0};
int countButtons1=2;
void setup()
{
stepper.setSpeed(50);
}
void loop()
{
// проверка нажатия кнопок
for(int i=0;i<countButtons1;i++)
{
currentButtons1[i] = debounce(lastButtons1[i],pinButtons1[i]);
if (lastButtons1[i] == 0 && currentButtons1[i] == 1)
// если нажатие...
{
if(i==0)
stepper.step(10*STEPS);
else
stepper.step(-10*STEPS);
}
lastButtons1[i] = currentButtons1[i];
}
}
// Функция сглаживания дребезга
int debounce(int last,int pin1)
{
int current = digitalRead(pin1); // Считать состояние кнопки
if (last != current) // если изменилось...
{
delay(5); // ждем 5 м с
current = digitalRead(pin1); // считываем состояние кнопки
return current; // возвращаем состояние кнопки
}
}

