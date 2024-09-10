#include <SoftwareSerial.h>

SoftwareSerial espSerial(D5, D6); // RX, TX
int contador = 0;
const int ledPin = LED_BUILTIN; // LED integrado en el ESP8266 (en el pin D4)
unsigned long inicio;
unsigned long fin = 0;

void setup()
{
  Serial.begin(115200);    // Comunicación con la consola
  espSerial.begin(115200); // Comunicación con el Arduino Mega

  pinMode(ledPin, OUTPUT); // Configurar el LED como salida
  Serial.println("Transmisión de datos al Arduino Mega...");
}

void loop()
{
  digitalWrite(ledPin, LOW); // LOW enciende el LED en el ESP8266
  delay(250);
  // Encender el LED por 250 ms
  digitalWrite(ledPin, HIGH); // HIGH apaga el LED
  inicio = millis();
  espSerial.println(fin);
  Serial.println(contador);
  contador++;
  if (espSerial.available())
  {
    String palabraRecibida = espSerial.readString(); // Leer la palabra enviada
    Serial.println(palabraRecibida);
  }
  fin = millis() - inicio;
  delay(1000); // Esperar un segundo antes de enviar otra vez
}
