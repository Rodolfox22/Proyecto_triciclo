// hacer mediicones diferenciales y agregar filtro RC a la entrada
// bajar el SPS y mejor alimentarlo con 3,3v

// #include <Adafruit_ADS1015.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include <ArduinoJson.h>

#include "encabezados.h"

RTC_DS1307 RTC;
StaticJsonDocument<300> estadoPant;
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
  {
    Serial.println("RTC is NOT running!");
    reloj = false;
  }
  else
  {
    Serial.println("RTC running");
    reloj = true;
  }

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

  // attachInterrupt(digitalPinToInterrupt(2), contadorPulsos, RISING);
  // attachInterrupt(digitalPinToInterrupt(18), contadorPulsos, RISING);
  // attachInterrupt(digitalPinToInterrupt(19), contadorPulsos, RISING);
  // attachInterrupt(digitalPinToInterrupt(20), contadorPulsos, RISING);
  // attachInterrupt(digitalPinToInterrupt(21), contadorPulsos, RISING);
  pinMode(PINGDERECHO, INPUT);
  pinMode(PINGIZQUIERDO, INPUT);
  pinMode(PINBALIZA, INPUT);
  pinMode(PINLDERECHO, OUTPUT);
  pinMode(PINLIZQUIERDO, OUTPUT);
}

void loop()
{
  TiempoImprime = millis() - UltimoTiempoImprime;
  if (TiempoImprime > DeltaTImprime)
  {
    UltimoTiempoImprime = millis();
    //   loguearDatos();
  }

  leerBotones();
  setearLuces();

  // print the string when a newline arrives:
  if (datoRecibidoCompleto)
  {
    enviarDatos();
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    datoRecibidoCompleto = false;
  }

  leerBotones();
  comprobar();
  setearLuces();
}

// Cada vez que tengo una interrupcion llamo a esta funcion y me devuelve la cantidad de microsegundo que hay entre pulso y pulso
void contadorPulsos()
{
  contador = micros() - anterior;
  anterior = micros();
}

void datosRecibidos(String datos)
{
  // String json = "{\"text\":\"myText\",\"id\":10,\"status\":true,\"value\":3.14}";

  DeserializationError error = deserializeJson( estadoPant, datos);
  if (error)
  {
    return;
  }

  guinho = estadoPant["guinho"];
  trip = estadoPant["trip"];
  cicloJS = estadoPant["cicloJS"];

  Serial.print("Guinho: ");
  Serial.print(guinho);
  Serial.print("\tTrip: ");
  Serial.print(trip);
  Serial.print("\tCiclo: ");
  Serial.println(cicloJS);
}

void serialEvent()
{
  String datosJS;
  if (Serial1.available())
  {
    datosJS = Serial1.readString();
    datosRecibidos(datosJS);
    datoRecibidoCompleto = true;
  }
}

// Convierte en una palabra los datos recibidos
void enviarDatos()
{
  String valores;
  StaticJsonDocument<300> sensoresJson;

  sensoresJson["humedad"] = humedad;
  sensoresJson["temperatura"] = temperatura;
  sensoresJson["velocidad"] = velocidad;
  sensoresJson["trip"] = trip;
  sensoresJson["odometro"] = odometro;
  sensoresJson["temp_bat"] = temp_bat;
  sensoresJson["carga"] = carga;
  sensoresJson["guinho"] = guinhoActual;

  serializeJson(sensoresJson, valores);
  Serial.println(valores);
  // Se envían los datos al ESP8266
  Serial1.println(valores);
}

void loguearDatos()
{
  String guardarLog = prepararDatosSD();
  guardarLog += "\nOdometro: ";
  guardarLog += odometro;
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

  velocidad = calcularVelocidad(rueda, contador);
  dataString += String(velocidad);
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

// Calcular Velocidad
int calcularVelocidad(int diametro, int pulsos_por_vuelta)
{
  float velocidad_km_h = 0;
  if (pulsos_por_vuelta > 0)
  {
    // circunferencia de la rueda en metros
    float circunferencia = PI * float(diametro) / 100.0;
    // tiempo entre pulsos en segundos
    float tiempo_entre_pulsos_seg = pulsos_por_vuelta / 1000000.0;
    // velocidad en m/s
    float velocidad_m_s = circunferencia / tiempo_entre_pulsos_seg;
    // velocidad en km/h
    velocidad_km_h = velocidad_m_s * 3.6;
  }
  return int(velocidad_km_h);
}

void leerBotones()
{
  if (millis() - anteriorGuinho >= TIEMPOGUINHO)
  {
    if (digitalRead(PINGDERECHO))
    {
      gDerecho = !gDerecho;
      gIzquierdo = 0;
      baliza = 0;
      anteriorGuinho = millis();
      return;
    }

    if (digitalRead(PINGIZQUIERDO))
    {
      gIzquierdo = !gIzquierdo;
      gDerecho = 0;
      baliza = 0;
      anteriorGuinho = millis();
      return;
    }

    if (digitalRead(PINBALIZA))
    {
      baliza = !baliza;
      gIzquierdo = 0;
      gDerecho = 0;
      anteriorGuinho = millis();
      return;
    }

    // Lógica para manejar comandos por puerto serie
    if (guinho == "derecha")
    {
      gDerecho = 1;
      gIzquierdo = 0;
      baliza = 0;
      anteriorGuinho = millis();
      return;
    }

    if (guinho == "izquierda")
    {
      gIzquierdo = 1;
      gDerecho = 0;
      baliza = 0;
      anteriorGuinho = millis();
      return;
    }

    if (guinho == "baliza")
    {
      baliza = 1;
      gIzquierdo = 0;
      gDerecho = 0;
      anteriorGuinho = millis();
      return;
    }
    baliza = 0;
    gIzquierdo = 0;
    gDerecho = 0;
    anteriorGuinho = millis();
  }
}

void setearLuces()
{
  if (millis() - anteriorDestello >= TIEMPODESTELLO)
  {
    anteriorDestello = millis();
    encender();

    if (gDerecho)
    {
      guinhoActual = "derecha";
      derecho = !derecho;
      izquierdo = 0;
      return;
    }

    if (gIzquierdo)
    {
      guinhoActual = "izquierda";
      izquierdo = !izquierdo;
      derecho = 0;
      return;
    }

    if (baliza)
    {
      guinhoActual = "baliza";
      derecho = !derecho;
      izquierdo = derecho;
      return;
    }

    // Si no hay pulsadores activados ni comando en guinho
    guinhoActual = "";
    derecho = 0;
    izquierdo = 0;
    baliza = 0;
  }
}

void encender()
{
  digitalWrite(PINLIZQUIERDO, derecho);
  digitalWrite(PINLDERECHO, izquierdo);
}

// Temperatura ambiente   lectura[0]
// Humedad                lectura[1]
// Calcular Velocidad     lectura[2]
// Trip                   lectura[3]
// Odometro               lectura[4]
// temperatura bateria    lectura[5]
// Carga batería          lectura[6]