#include <AFMotor.h>
#define POTE A0
#define TIEMPO 1000

// motores DC
AF_DCMotor motor(4);

int potenciometro = 0;
int velocidad = 0;
unsigned long tiempoAnterior;
int velocidadAnterior = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Puerto listo");
}
void loop()
{
  potenciometro = analogRead(POTE);
  velocidad = map(potenciometro, 0, 1024, 50, 255);
  motor.setSpeed(velocidad);
  if (velocidad > 80)
  {
    motor.run(FORWARD);
  }
  else
  {
    motor.run(RELEASE);
  }
  
  if (velocidad != velocidadAnterior && millis() - tiempoAnterior >= TIEMPO)
  {
    Serial.print(velocidad);
    Serial.print("\t\t");
    Serial.println(potenciometro);
  }
}
