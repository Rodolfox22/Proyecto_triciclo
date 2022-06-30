  String dato_recibido;

String leerDato(char dato)
{
  String palabra;
  unsigned long tiempo_espera = 50;
  unsigned long tiempo_proceso;

  Wire.beginTransmission(23); // Comenzar a comunicarse con esclavo #23
  Wire.write(dato);
  Wire.endTransmission(); // Finalizar comunicación

  Wire.requestFrom(23, 1); // Solicitar 1 byte del esclavo #23

  byte len = Wire.read();
  Wire.requestFrom(23, (int)len); // Solicitar 'len' byte del esclavo #23

  //tiempo_proceso = millis();
  while (Wire.available()) // hay datos disponibles?
  {
   /* if (millis() - tiempo_proceso >= tiempo_espera)
    {
      return "error";
    }*/

    char c = Wire.read(); // recibir byte como un caracter
    palabra += c;
    Serial.print(c); // enviar caracter al monitor
  }
  return palabra;
}

