// hacer mediicones diferenciales y agregar filtro RC a la entrada
// bajar el SPS y mejor alimentarlo con 3,3v

// #include <Adafruit_ADS1015.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include <ArduinoJson.h>

RTC_DS1307 RTC;
#include "encabezados.h"
#include "datos_bateria.hpp"

StaticJsonDocument<300> estadoPant;

void setup()
{
  ////////////SERIAL////////////
  Serial.setTimeout(10);
  Serial.begin(9600);
  Serial1.begin(9600);

  inicializar();
  inicializarLecturas();
  recuperarDatos();
}

void loop()
{
  lecturasSensores();
  actualizarSensores();
  calcularDistancia();
  loguearDatos();

  leerBotones();
  setearLuces();

  enviarDatos();
}

// Cada vez que tengo una interrupcion llamo a esta funcion y me devuelve la cantidad de microsegundo que hay entre pulso y pulso
void contadorPulsos()
{
  contador = micros() - anterior;
  anterior = micros();
  pulsosTotales++;
}

void datosRecibidos(String datos)
{
  // String json = "{\"text\":\"myText\",\"id\":10,\"status\":true,\"value\":3.14}";

  DeserializationError error = deserializeJson(estadoPant, datos);
  if (error)
  {
    Serial.println("Los datos recibidos por el puerto serie son incompatibles con Json");
    return;
  }

  guinho = String(estadoPant["guinho"].as<const char *>());
  trip = estadoPant["trip"].as<int>();
  cicloJS = estadoPant["cicloJS"].as<int>();

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
    Serial.println("Datos recibidos por Serial1");
    Serial.println(datosJS);

    // Borra cualquier resto de datos recibidos
    while (Serial1.available())
    {
      Serial1.read();
    }
    datoESP8266Completo = true;
    datoRecibidoCompleto = true;
  }

  if (Serial.available())
  {
    guinho = Serial.readStringUntil('\n');
    Serial.print("Datos recibidos por Serial, guinho:");
    Serial.println(guinho);

    // Borra cualquier resto de datos recibidos
    while (Serial.available())
    {
      Serial.read();
    }
    datoRecibidoCompleto = true;
  }
}

// Convierte en una palabra los datos recibidos
void enviarDatos()
{
  if (datoESP8266Completo)
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
    Serial.println("Json");
    Serial.println(valores);
    // Se envían los datos al ESP8266
    Serial1.println(valores);
    datoESP8266Completo = false;
  }
}

// Guarda estadísticas y datos importantes en la tarjeta SD
void loguearDatos()
{
  if (millis() - UltimoTiempoImprime > DeltaTImprime)
  {
    UltimoTiempoImprime = millis();
    String guardarLog = prepararDatosSD();

    imprimirEnSD(guardarLog);

    String datosOdo = "#odometro#";
    datosOdo += pulsosTotales;
    datosOdo += ",#trip#";
    datosOdo += trip;
    datosOdo += ",";
    guardarOdometro(datosOdo);
  }
}

// Prepara los datos para guardar en la tarjeta
String prepararDatosSD()
{
  String logActual = "";
  DateTime now = RTC.now();
  dataString = String(now.day(), DEC);
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
  dataString += ",#velocidad#";
  dataString += String(velocidad);
  dataString += ",#temp_bat#";
  dataString += temp_bat;
  dataString += ",#carga#";
  dataString += carga;
  dataString += ",#temperatura#";
  dataString += temperatura;
  return dataString;
}

// Imprime los datos en la tarjeta
void imprimirEnSD(String &logActual)
{
  Serial.println("//////////////Dato completo datalog//////////////");
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
}

void guardarOdometro(String &logActual)
{
  Serial.println("Guardando en archivo odo.txt");
  File dataFile = SD.open("odo.txt", FILE_WRITE | O_TRUNC);
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

int buscarEnSD(const String &variableBuscada)
{
  File archivo = SD.open("odo.txt");
  String buscar = "#" + variableBuscada + "#";
  String dato = "0";
  int valor = 0;
  if (archivo)
  {
    while (archivo.available())
    {
      String linea = archivo.readStringUntil('\n');

      // Buscar la palabra clave  en la línea
      int inicio = linea.indexOf(buscar);
      if (inicio != -1)
      {
        // Encontrado, ahora buscar la coma después de la palabra clave
        int fin = linea.indexOf(',', inicio);
        if (fin != -1)
        {
          // Extraer el dato entre la palabra clave y la coma
          dato = linea.substring(inicio + buscar.length(), fin);

          //Serial.println("Dato encontrado: " + dato);
          valor = dato.toInt();
        }
      }
    }
    archivo.close();
  }
  else
  {
    Serial.println("Error al abrir el archivo.");
  }
  return valor;
}

// Calcular Velocidad
int calcularVelocidad(int diametro, int pulsos_por_vuelta)
{
  float velocidad_km_h = 0;
  if (pulsos_por_vuelta > 0)
  {
    // circunferencia de la rueda en metros
    // todo averiguar porque divide por 100
    float circunferencia = PI * float(diametro) / 100.0;

    // tiempo entre pulsos en segundos
    float tiempo_entre_pulsos_seg = float(pulsos_por_vuelta) / 1000000.0;
    // velocidad en m/s
    float velocidad_m_s = circunferencia / tiempo_entre_pulsos_seg;
    // velocidad en km/h
    velocidad_km_h = velocidad_m_s * 3.6;
  }
  return int(velocidad_km_h);
}

// Calcula la velocidad y la distancia recorrida
void calcularDistancia()
{
  if (millis() - anteriorDistancia >= TIEMPODISTANCIA)
  {
    float temp = int(float(pulsosTotales) * float(rueda) * PI / 1000.0);
    velocidad = calcularVelocidad(rueda, contador);
    odometro = int(temp);
    // verOdometro();
    anteriorDistancia = millis();
  }
}

void leerBotones()
{
  // Los botones son de logica inversa, por lo que al presionar se envía un '0' lógico
  // Variables para detectar flanco descendente
  static int anteriorPinD = HIGH;
  static int anteriorPinI = HIGH;
  static int anteriorPinB = HIGH;

  if (millis() - anteriorGuinho >= TIEMPOGUINHO)
  {
    int pinDerecho = digitalRead(PINGDERECHO);
    int pinIzquierdo = digitalRead(PINGIZQUIERDO);
    int pinBaliza = digitalRead(PINBALIZA);

    if (!pinDerecho && anteriorPinD)
    {
      Serial.println("Boton derecho");
      gDerecho = !gDerecho;
      gIzquierdo = 0;
      baliza = 0;
      guinhoActual = valorGuinho(gDerecho, textoDerecha);
      anteriorPinD = pinDerecho;
      return;
    }
    anteriorPinD = pinDerecho;

    if (!pinIzquierdo && anteriorPinI)
    {
      Serial.println("Boton izquierdo");
      gIzquierdo = !gIzquierdo;
      gDerecho = 0;
      baliza = 0;
      guinhoActual = valorGuinho(gIzquierdo, textoIzquierda);
      anteriorPinI = pinIzquierdo;
      anteriorGuinho = millis();
      return;
    }
    anteriorPinI = pinIzquierdo;

    if (!pinBaliza && anteriorPinB)
    {
      Serial.println("Boton baliza");
      baliza = !baliza;
      gIzquierdo = 0;
      gDerecho = 0;
      guinhoActual = valorGuinho(baliza, textoBaliza);
      anteriorPinB = pinBaliza;
      anteriorGuinho = millis();
      return;
    }
    anteriorPinB = pinBaliza;

    escucharPuerto();

    if (guinhoActual == textoApagado)
    {
      baliza = 0;
      gIzquierdo = 0;
      gDerecho = 0;
      anteriorGuinho = millis();
    }
  }
}

// Lógica para manejar comandos por puerto serie
void escucharPuerto()
{
  if (datoRecibidoCompleto)
  {
    datoRecibidoCompleto = false;
    Serial.println("Datos del puerto");

    if (guinho.equals(textoDerecha))
    {
      Serial.println("Luz derecha");
      gDerecho = 1;
      gIzquierdo = 0;
      baliza = 0;
      guinhoActual = textoDerecha;
      anteriorGuinho = millis();
      return;
    }

    if (guinho.equals(textoIzquierda))
    {
      Serial.println("Luz izquierda");
      gIzquierdo = 1;
      gDerecho = 0;
      baliza = 0;
      guinhoActual = textoIzquierda;
      anteriorGuinho = millis();
      return;
    }

    if (guinho.equals(textoBaliza))
    {
      Serial.println("Baliza encendida");
      baliza = 1;
      gIzquierdo = 0;
      gDerecho = 0;
      guinhoActual = textoBaliza;
      anteriorGuinho = millis();
      return;
    }

    if (guinho == textoApagado)
    {
      baliza = 0;
      gIzquierdo = 0;
      gDerecho = 0;
      guinhoActual = textoApagado;
      anteriorGuinho = millis();
      return;
    }
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
      derecho = !derecho;
      izquierdo = 0;
      return;
    }

    if (gIzquierdo)
    {
      izquierdo = !izquierdo;
      derecho = 0;
      return;
    }

    if (baliza)
    {
      derecho = !derecho;
      izquierdo = derecho;
      return;
    }

    // Si no hay pulsadores activados ni comando en guinho
    // guinhoActual = "";
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

// Recupera datos útiles para odometro y trip
void recuperarDatos()
{
  pulsosTotales = buscarEnSD("odometro");
  trip = buscarEnSD("trip");
  Serial.print("Pulsos: ");
  Serial.print(pulsosTotales);
  Serial.print("\tTrip: ");
  Serial.println(trip);
}

// Setea los valores de variables para que estén disponibles a la hora de la transmisión de datos
void actualizarSensores()
{
  humedad = humedad_ambiente;
  temperatura = temperatura_ambiente;
  temp_bat = temperatura_ok;
  carga = porcentaje_carga;
}

void verOdometro()
{
  Serial.print("Pulsos: ");
  Serial.print(pulsosTotales);
  Serial.print("\tOdometro: ");
  Serial.println(odometro);
  Serial.print("\tDerecho: ");
  Serial.print(gDerecho);
  Serial.print("\tBaliza: ");
  Serial.print(baliza);
  Serial.print("\tIzquierdo: ");
  Serial.print(gIzquierdo);
  Serial.print("\tGuinho: ");
  Serial.println(guinhoActual);
}

String valorGuinho(int variable, const String &texto)
{
  if (variable)
  {
    return texto;
  }
  return textoApagado;
}

// Temperatura ambiente   lectura[0]
// Humedad                lectura[1]
// Calcular Velocidad     lectura[2]
// Trip                   lectura[3]
// Odometro               lectura[4]
// temperatura bateria    lectura[5]
// Carga batería          lectura[6]