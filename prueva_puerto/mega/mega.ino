unsigned long inicio;
unsigned long fin = 0;
String texto_completo;
String palabraRecibida;

void setup()
{
  Serial.begin(115200);  // Comunicación con la consola
  Serial1.begin(115200); // Comunicación con el ESP8266 (Serial1)
}

void loop()
{
  if (Serial1.available())
  { // Si hay datos en Serial1
    inicio = millis();
    static int contador = 0;
    palabraRecibida = Serial1.readString(); // Leer la palabra enviada
    contador++;                                    // Incrementar el contador

    // Mostrar la palabra y el número en el Serial (consola)
    texto_completo = "\nRecibido: ";
    texto_completo += palabraRecibida;
    texto_completo += "\tCuenta: ";
    texto_completo += contador;
    texto_completo += "\tTiempo anterior: ";
    texto_completo += fin;

    Serial.print(texto_completo);
    Serial1.print(texto_completo);
    
    fin = millis() - inicio;
  }
}
