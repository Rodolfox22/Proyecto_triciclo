#include <Wire.h>

unsigned long inicio = 0;
unsigned long fin = 0;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
}

void loop()
{
  inicio = millis();
  Wire.beginTransmission(8);                                                            // Dirección del dispositivo esclavo (puedes cambiarla según tu configuración)
  Wire.write("{\"humedad\":79.2,\"temperatura\":25.3,\"velocidad\":13,
  \"trip\":8,\"temp_bat\":32,\"odometro\":220,
  \"carga\":55,\"guinho\":\"apagado\""); // Aquí puedes enviar los datos de tu archivo de texto
  Wire.endTransmission();
  fin = millis();
  unsigned long tiempo = fin - inicio;
  Serial.println(tiempo);
  delay(5000); // Espera 5 segundos antes de enviar nuevamente
}

