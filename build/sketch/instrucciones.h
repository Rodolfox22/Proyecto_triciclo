#line 1 "c:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\instrucciones.h"
String leerDato(char dato)
{
  String palabra;

  Wire.beginTransmission(23); // Comenzar a comunicarse con esclavo #23
  Wire.write(dato);
  Wire.endTransmission(); // Finalizar comunicación

  Wire.requestFrom(23, 1); // Solicitar 1 byte del esclavo #23

  byte len = Wire.read();
  Wire.requestFrom(23, (int)len); // Solicitar 'len' byte del esclavo #23

  while (Wire.available()) // hay datos disponibles?
  {
    char c = Wire.read(); // recibir byte como un caracter
    palabra += c;
    Serial.print(c); // enviar caracter al monitor
  }
  return palabra;
}