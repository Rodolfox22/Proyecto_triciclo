// hacer mediicones diferenciales y agregar filtro RC a la entrada
// bajar el SPS y mejor alimentarlo con 3,3v

#include <Adafruit_ADS1x15.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "encabezados.h"

RTC_DS1307 RTC;
Adafruit_ADS1115 ads;

void setup()
{
  ////////////SERIAL////////////
  Serial.setTimeout(10);
  Serial.begin(115200);
  Serial1.begin(115200);

  ////////////RTC////////////
  Wire.begin();
  RTC.begin();
  if (!RTC.isrunning())
    Serial.println("RTC is NOT running!");
  else
    Serial.println("RTC running");

  // RTC.adjust(DateTime(__DATE__, __TIME__)); //Cargarlo una vez para setearlo y despues sacar frase para que cuando se reinicio no vuelva a la fecha vieja

  ////////////SD////////////
  Serial.print("Inicializando tarjeta SD...");
  if (!SD.begin(chipSelect))
  {
    Serial.println("Fallo la tarjeta, o no esta presente");
    while (1)
      continue;
  }
  Serial.println("Tarjeta inicializada.");

  ////////////ADC///////////
  // Factor de escala
  // ads.setGain(GAIN_TWOTHIRDS);   // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);       // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);       // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);      // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);     // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  ads.setGain(GAIN_SIXTEEN); // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  ads.begin();               // Iniciar el ADS1115
  ads.setSPS(ADS1115_DR_16SPS);
  //  if (!ads.begin()) {
  //    Serial.println("Failed to initialize ADS.");
  //    while (1);
  //  }
  //  else {
  //    Serial.println("ADS initialized.");
  //  }

  /////////CONTADOR////////
  attachInterrupt(digitalPinToInterrupt(3), contador_pulsos, RISING);

  // Inicializa la lectura de datos
  for (int i = 0; i < CANTDATOS; i++)
  {
    lectura[i] = 0;
  }
}

void loop()
{
  TiempoImprime = millis() - UltimoTiempoImprime;
  if (TiempoImprime > DeltaTImprime)
    logearDatos();
}

void contador_pulsos()
{
  contador = micros() - anterior;
  anterior = micros();
}

void serialEvent()
{
  while (Serial1.available())
  {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '_')
    {
      datoRecibidoCompleto = true;
    }
  }
}

void enviarDato(String dato)
{
  Serial1.print(dato);
}

void procesarDato()
{
  String datoCompleto = "";
  float temp = 0;
  for (int i = 0; i < CANTDATOS; i++)
  {
    temp = floatToString(lectura[i]);

    if (lecturaEtiquetas[i] = "ODO")
      temp = floatToString(lectura[i], true);

    datoCompleto += lecturaEtiquetas[i];
    datoCompleto += ":";
    datoCompleto += temp;
    datoCompleto += "-";
  }

  datoCompleto += "_";
  Serial1.print(datoCompleto);
}

String floatToString(float numero, bool ceros, int largo, int decimal)
{
  char caracter[largo + 1];
  String palabra;

  dtostrf(numero, largo, decimal, caracter);
  palabra = String(caracter);

  if (ceros)
  {
    palabra.replace(" ", "0");
    return palabra;
  }

  palabra.replace(" ", "");
  return palabra;
}

void logearDatos()
{
  leerFecha();
  valorPromedio();
  calcularVelocidad();
  imprimirEnSD();
}

void leerFecha()
{
  String dataString = "";
  DateTime now = RTC.now();
  dataString += String(now.day(), DEC);
  dataString += "/";
  dataString += String(now.month(), DEC);
  dataString += "/";
  dataString += String(now.year(), DEC);
  dataString += ",";
  dataString += String(now.hour(), DEC);
  dataString += ":";
  dataString += String(now.minute(), DEC);
  dataString += ":";
  dataString += String(now.second(), DEC);
  dataString += ",";
}

void calcularVelocidad()
{
  float vel = 0;
  if (contador > 0)
  {
    float pulsos_sec = 1000000 / contador;
    float rad_s = (pulsos_sec / p) * 2 * 3.14;
    // float vel = rad_s*26*0.5*2.54*0.01*3.6;
    float vel = rad_s * 9.554; // rpm
  }
  dataString += String(vel);
  dataString += ",";
}

void imprimirEnSD()
{
  Serial.println("//////////////Dato completo//////////////");
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile)
  {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString);
  }
  else
  {
    Serial.println("Error SD");
  }

  UltimoTiempoImprime = millis();
  contador = 0;
}

void valorPromedio()
{
  // ADS1115 Tomar valor promedio
  //     int16_t adc0, adc3;
  //     //adc0 = ads.readADC_SingleEnded(0);
  //     //adc3 = ads.readADC_SingleEnded(3);
  //     float vsensor_V = 0;
  //     float vsensor_I = 0;
  //     for (int i = 0; i < n_muestras; i++)
  //     {
  //       ads.readADC_SingleEnded(0);
  //
  //       ads.readADC_SingleEnded(3);
  //       vsensor_V = vsensor_V + adc0;
  //       vsensor_I = vsensor_I + adc3;
  //     }
  //     vsensor_V = vsensor_V / n_muestras;
  //     vsensor_I = vsensor_I / n_muestras;
  //     //vsensor_V = vsensor_V / 4.489;
  //     //vsensor_I = vsensor_I * 0.833;

  int16_t tension;
  int16_t corriente;
  /* Be sure to update this value based on the IC and the gain settings! */
  //    float multiplier = 0.007812F;
  //    corriente = ads.readADC_Differential_0_1();
  //    tension = ads.readADC_Differential_2_3();
  //    dataString += String(tension*multiplier*-0.2245);
  //    dataString += ",";
  //    dataString += String(corriente*multiplier*-0.833);
  //    dataString += ",";
}
