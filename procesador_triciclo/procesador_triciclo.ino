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
  // Serial.setTimeout(10);
  Serial.begin(9600);
  Serial1.begin(115200);

  inicializar();
  inicializarLecturas();
  recuperarDatos();
}

void loop()
{
  lecturasSensores();
  actualizarSensores();
  crearJson();
  calcularDistancia();
  loguearDatos();

  leerBotones();
  setearLuces();

  enviarDatos();
  // visualizaI2C();
}

// Cada vez que tengo una interrupcion llamo a esta funcion y me devuelve la cantidad de microsegundo que hay entre pulso y pulso
void contadorPulsos()
{
  contador++;
}

void datosRecibidos(String &datos)
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
  /*
    Serial.print("Guinho: ");
    Serial.print(guinho);
    Serial.print("\tTrip: ");
    Serial.print(trip);
    Serial.print("\tCiclo: ");
    Serial.println(cicloJS);*/
}

void leerServidor()
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
  }

  // Sentencia para provar la recepcion de datos desde puerto serie
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
  }
}

void crearJson()
{
  StaticJsonDocument<300> sensoresJson;

  // TODO: trabajar en la recepcion de la hora desde la página
  sensoresJson["humedad"] = humedad;
  sensoresJson["temperatura"] = temperatura;
  sensoresJson["velocidad"] = velocidad;
  sensoresJson["trip"] = trip;
  sensoresJson["odometro"] = odometro;
  sensoresJson["temp_bat"] = temp_bat;
  sensoresJson["carga"] = carga;
  sensoresJson["guinho"] = guinhoActual;

  serializeJson(sensoresJson, valoresJson);
}

// Convierte en una palabra los datos recibidos
void enviarDatos()
{
  if (Serial1.available())
  {
    leerServidor();

    Serial.println("Json");
    Serial.println(valoresJson);
    valoresJson = "";
    //  Se envían los datos al ESP8266
    Serial1.println(valoresJson);
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
    datosOdo += distanciaTotal;
    datosOdo += ",#trip#";
    datosOdo += trip;
    datosOdo += ",";
    guardarOdometro(datosOdo);
  }
}

// Prepara los datos para guardar en la tarjeta
String prepararDatosSD()
{
  dataString += "";
  String logActual = "";
  if (reloj)
  {
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
  }

  dataString += ",#velocidad#";
  dataString += String(velocidad);
  dataString += ",#temp_bat#";
  dataString += temp_bat;
  dataString += ",#carga#";
  dataString += carga;
  dataString += ",#temperatura#";
  dataString += temperatura;
  dataString += ",#humedad#";
  dataString += humedad;
  return dataString;
}

// Imprime los datos en la tarjeta
void imprimirEnSD(String &logActual)
{
  // Serial.println("//////////////Dato completo datalog//////////////");
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
  // Serial.println("Guardando en archivo odo.txt");
  File dataFile = SD.open("odo.txt", FILE_WRITE | O_TRUNC);
  if (dataFile)
  {
    dataFile.println(logActual);
    dataFile.close();
    // Serial.println(logActual);
  }
  else
  {
    Serial.println("Error SD");
  }

  UltimoTiempoImprime = millis();
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

          // Serial.println("Dato encontrado: " + dato);
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

// Calcula la velocidad y la distancia recorrida
void calcularDistancia()
{
  if (millis() - anteriorDistancia >= TIEMPODISTANCIA)
  {
    //Serial.print(contador);
    velocidad = calcularVelocidad();
    distanciaTotal += float(contador) * desarrollo_m / 1000.0;
    odometro = int(distanciaTotal);
    anteriorDistancia = millis();
    /*Serial.print("\tVelocidad: ");
    Serial.print(velocidad);
    Serial.print("\tOdometro: ");
    Serial.println(odometro);*/
  }
}

// Calcular Velocidad
int calcularVelocidad()
{
  // velocidad en m/s
  float velocidad_m_s = desarrollo_m * (float(contador) / float(TIEMPODISTANCIA));
  // velocidad en km/h
  float velocidad_km_h = velocidad_m_s * 360.0;
  contador = 0;
  return int(velocidad_km_h);
}

void leerBotones()
{
  // Los botones son de logica inversa, por lo que al presionar se envía un '0' lógico
  // Variables para detectar flanco descendente
  static int anteriorPinD = HIGH;
  static int anteriorPinI = HIGH;
  static int anteriorPinB = HIGH;

  // Espero que pace un tiempo entre una lectura y otra para detectar solo flanco descendente
  if (millis() - anteriorGuinho >= TIEMPOGUINHO)
  {
    int pinDerecho = digitalRead(PINGDERECHO);
    int pinIzquierdo = digitalRead(PINGIZQUIERDO);
    int pinBaliza = digitalRead(PINBALIZA);

    // Detecto si hay una pulsacion
    if (!pinDerecho && anteriorPinD)
    {
      Serial.println("Boton derecho");
      gDerecho = !gDerecho;
      gIzquierdo = 0;
      baliza = 0;
      // Asigna al valor guinhoActual, el valor de texto correspondiente a la orden
      guinhoActual = valorGuinho(gDerecho, textoDerecha);
      anteriorPinD = pinDerecho;
      return;
    }
    anteriorPinD = pinDerecho;

    // Detecto si hay una pulsacion
    if (!pinIzquierdo && anteriorPinI)
    {
      Serial.println("Boton izquierdo");
      gIzquierdo = !gIzquierdo;
      gDerecho = 0;
      baliza = 0;
      // Asigna al valor guinhoActual, el valor de texto correspondiente a la orden
      guinhoActual = valorGuinho(gIzquierdo, textoIzquierda);
      anteriorPinI = pinIzquierdo;
      anteriorGuinho = millis();
      return;
    }
    anteriorPinI = pinIzquierdo;

    // Detecto si hay una pulsacion
    if (!pinBaliza && anteriorPinB)
    {
      Serial.println("Boton baliza");
      baliza = !baliza;
      gIzquierdo = 0;
      gDerecho = 0;
      // Asigna al valor guinhoActual, el valor de texto correspondiente a la orden
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
  /*//Escucho el puerto serie
datoRecibidoCompleto = false;
Serial.println("Datos del puerto");
*/

  leerEntradas();

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

// Lee las entradas digitales
void leerEntradas()
{
  int inDerecho = digitalRead(INDERECHA);
  int inIzquierdo = digitalRead(INIZQUIERDA);
  //Serial.println("Leer entradas");
  if (inDerecho)
  {
    guinho = textoDerecha;
  }

  if (inIzquierdo)
  {
    guinho = textoIzquierda;
  }

  if (inDerecho & inIzquierdo)
  {
    guinho = textoBaliza;
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
      digitalWrite(OUTDERECHA, HIGH);
      digitalWrite(OUTIZQUIERDA, LOW);
      derecho = !derecho;
      izquierdo = 0;
      return;
    }

    if (gIzquierdo)
    {
      digitalWrite(OUTDERECHA, LOW);
      digitalWrite(OUTIZQUIERDA, HIGH);
      izquierdo = !izquierdo;
      derecho = 0;
      return;
    }

    if (baliza)
    {
      digitalWrite(OUTDERECHA, HIGH);
      digitalWrite(OUTIZQUIERDA, HIGH);
      derecho = !derecho;
      izquierdo = derecho;
      return;
    }

    // Si no hay pulsadores activados ni comando en guinho
    // guinhoActual = "";
    digitalWrite(OUTDERECHA, LOW);
    digitalWrite(OUTIZQUIERDA, LOW);
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
  distanciaTotal = buscarEnSD("odometro");
  trip = buscarEnSD("trip");
  Serial.print("Pulsos: ");
  Serial.print(distanciaTotal);
  Serial.print("\tTrip: ");
  Serial.println(trip);
}

// Setea los valores de variables para que estén disponibles al momento de la transmisión de datos
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
  Serial.print(distanciaTotal);
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

// Enciende o apaga la orden de guiños correspondiente a cada boton
String valorGuinho(int variable, const String &texto)
{
  if (variable)
  {
    return texto;
  }
  return textoApagado;
}

// Solo para depuración
void visualizaI2C()
{
  if (valoresJson.length() > 0)
  {
    Serial.println("Datos recibidos: " + valoresJson);
    valoresJson = ""; // Limpiar el buffer después de procesar
  }
}

// Temperatura ambiente   lectura[0]
// Humedad                lectura[1]
// Calcular Velocidad     lectura[2]
// Trip                   lectura[3]
// Odometro               lectura[4]
// temperatura bateria    lectura[5]
// Carga batería          lectura[6]