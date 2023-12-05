// hacer mediicones diferenciales y agregar filtro RC a la entrada
// bajar el SPS y mejor alimentarlo con 3,3v

// #include <Adafruit_ADS1015.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "encabezados.h"

RTC_DS1307 RTC;
// Adafruit_ADS1115 ads;

void setup()
{
  ////////////SERIAL////////////
  Serial.setTimeout(10);
  Serial.begin(9600);
  Serial1.begin(9600);

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
    // while (1)
    //  continue;
  }
  else
    Serial.println("Tarjeta inicializada.");

  ////////////ADC///////////
  // Factor de escala
  // ads.setGain(GAIN_TWOTHIRDS);   // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);       // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);       // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);      // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);     // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV

  //  ads.setGain(GAIN_SIXTEEN); // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  //  ads.begin();               // Iniciar el ADS1115
  //  ads.setSPS(ADS1115_DR_16SPS);

  //  if (!ads.begin()) {
  //    Serial.println("Failed to initialize ADS.");
  //    while (1);
  //  }
  //  else {
  //    Serial.println("ADS initialized.");
  //  }

  /////////   CONTADOR   ////////
  attachInterrupt(digitalPinToInterrupt(3), contadorPulsos, RISING);

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
    //   loguearDatos();

    // testeo el muestreo del envio de datos por el puerto
    if (TIEMPOSUMA <= millis() - anteriorSuma)
    {
      anteriorSuma = millis();
      lectura[3]++;
      if (lectura[3] > 250)
        lectura[3] = 1;

      Serial.print("Trip: ");
      Serial.println(lectura[3]);

      lecturas = procesarDato(lectura[], CANTDATOS);
      Serial1.print(lecturas);
      Serial.println(lecturas);
    }

  // print the string when a newline arrives:
  if (datoRecibidoCompleto)
  {
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    datoRecibidoCompleto = false;
  }
}

// Cada vez que tengo una interrupcion llamo a esta funcion y me devuelve la cantidad de microsegundo que hay entre pulso y pulso
void contadorPulsos()
{
  contador = micros() - anterior;
  anterior = micros();
}

void serialEvent()
{
  while (Serial1.available())
  {
    char inChar = (char)Serial1.read();
    inputString += inChar;
    if (inChar == '\n')
    {
      datoRecibidoCompleto = true;
    }
  }
}

// Convierte en una palabra los datos recibidos
String procesarDato(int lecturadatos[])
{
  String datoCompleto = "Cantidad de datos:" + String(CANTDATOS);
  String separador = ",";

  for (int i = 0; i < CANTDATOS; i++)
  {
    datoCompleto += String(lecturadatos[i]);

    if (i < CANTDATOS - 1) // comento esta linea para probar si funciona el codigo
      datoCompleto += separador;
  }

  datoCompleto += "/";

  ultimoDato = datoCompleto;
  // Serial1.print(datoCompleto);
  return datoCompleto;
}

void loguearDatos()
{
  String guardarLog = prepararDatosSD();
  imprimirEnSD(guardarLog);
}

// Prepara los datos para guardar en la tarjeta
String prepararDatosSD()
{
  String logActual = "";
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

  int vel = calcularVelocidad(rueda, contador);
  dataString += String(vel);
  dataString += ",";

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

  /*int16_t tension;
  int16_t corriente;
  // Be sure to update this value based on the IC and the gain settings!
  float multiplier = 0.007812F;*/
  //    corriente = ads.readADC_Differential_0_1();
  //    tension = ads.readADC_Differential_2_3();
  //    dataString += String(tension*multiplier*-0.2245);
  //    dataString += ",";
  //    dataString += String(float(corriente) * multiplier * -0.833);
  //    dataString += ",";

  return dataString;
}

// Imprime los datos en la tarjeta
void imprimirEnSD(String &logActual)
{
  Serial.println("//////////////Dato completo//////////////");
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile)
  {
    dataFile.println(logActual);
    dataFile.close();
    Serial.println(logActual);
  }
  else
  {
    Serial.println("Error SD");
  }

  UltimoTiempoImprime = millis();
  contador = 0;
}

// Calcular Velocidad     lectura[2]
int calcularVelocidad(int diametro, int pulsos_por_vuelta)
{
  float velocidad_km_h = 0;
  if (pulsos_por_vuelta > 0)
  {
    float circunferencia = PI * float(diametro) / 100;              // circunferencia de la rueda en metros
    float tiempo_entre_pulsos_seg = pulsos_por_vuelta / 1000000.0;  // tiempo entre pulsos en segundos
    float velocidad_m_s = circunferencia / tiempo_entre_pulsos_seg; // velocidad en m/s
    velocidad_km_h = velocidad_m_s * 3.6;                           // velocidad en km/h
  }
  return int(velocidad_km_h);
}

// Temperatura ambiente   lectura[0]
// Humedad                lectura[1]
// Calcular Velocidad     lectura[2]
// Trip                   lectura[3]
// Odometro               lectura[4]
// temperatura bateria    lectura[5]
// Carga batería          lectura[6]