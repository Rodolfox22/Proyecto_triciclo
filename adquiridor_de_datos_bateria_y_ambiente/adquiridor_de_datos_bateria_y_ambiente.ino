
/* DATOS AUTOR:
 *  NOMBRE:   HUGO GALAVERNA
 *  LUGAR:    ARGENTINA,SANTA FE, RAFAELA
 *  ESTUDIOS: TECNICO ELECTROMECANICO, TECNICO SUP. MANTENIMIENTO INDUSTRIAL, TECNICO EN MECATRONICA
 *  TEL:      03492607569
 *  CORREO:   HUGOGALAVERNA@GMAIL.COM
  DECLARACIONES DE ENTRADAS
  * A0 (lectura carga bateria)
  * 6 (lectura sensor DS18B20)
  * 7 (lectura sensor DHT22)
  DATOS DE REFERENCIA PARA TOMAR A LA APLICACION

  * p= carga bateria en %
    t=temperatura bateria en °c
    h=humedad ambiente en %
    T=temperatura ambiente en °c

*/
// --------------------------------------------------------LIBRERIAS ----------------------------------------------------
#include <OneWire.h>
#include <DallasTemperature.h> // sensor temperatura de bateria
#include <DHT.h>               // ojo descargar la ultima version,  la 1.112 y 1.300 no funcionan * sensor de temperatura y humedad ambiente
#include <Wire.h>              //comunicacion I2C
//---------------------------------------------------------VARIABLES------------------------------------------------------

// comunicacion
String message;
bool S = false;
void eventoRecepcion();
void eventoSolicitud();

// variables medicion de carga
#define NUM_muestras 20           // número de muestras analógicas a tomar para la lectura
int sum = 0;                      // suma de muestras hechas
unsigned char muestras_count = 0; // numero de la muestra contadas
float voltaje = 0.0;              // voltaje calculado
float voltaje_de_bateria = 0.0;   // voltaje de bateria
float diferencia_tension_carga_descarga = 0.0;
int porcentaje_carga = 0;
int const medicion_voltaje = A0; // lectura de voltaje
int Porcentaje_carga = 0;

// variables de delay------------------

unsigned long tiempoUltimaLectura = 0; // Para guardar el tiempo de la última lectura
unsigned long retardo = 500;
unsigned long tiempoUltimaLectura_1 = 0; // Para guardar el tiempo de la última lectura
unsigned long retardo_1 = 2000;

// varibles sensor temperatura sensor DS18B20 -----------------------
float Temperatura = 0.0;
float temperatura_ok = 0.0;
float error_absoluto = 1.0;
OneWire ourWire(9);                  // Se establece el pin 6  como bus OneWire
DallasTemperature sensors(&ourWire); // Se declara una variable u objeto para nuestro sensor

// variables sensor humedad y temperatura DHT-------------------------

#define DHTTYPE DHT22  // Sensor DHT22
const int DHTPin = 10; //  Pin donde está conectado el sensor
float humedad_ambiente = 0.0;
float temperatura_ambiente = 0.0;
DHT dht(DHTPin, DHTTYPE); // definimos pin y tipo de sensor para libreria

// variables para maestro

float p = 0.0;
float t = 0.0;
float h = 0.0;
float T = 0.0;
//---------------------------------------------------------INICIO PROGRAMA-------------------------------------------------------------------------------------------
void setup() //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
{
  //Serial.begin(9600);
  sensors.begin(); // Se inicia el sensor de temperatura DS18B20
  dht.begin();     // Se inicia el sensor de humedad y temperatura

  // comunicacion------------------------------------

  Wire.begin(23);                  // unirse al bus i2c con la direccion #23
  Wire.onRequest(eventoSolicitud); // registrar evento de solicitud de datos
  Wire.onReceive(eventoRecepcion); // registrar evento de recepcion de datos
}

void loop() //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
{
  if (millis() - tiempoUltimaLectura > 3000) // delay de encendido realizado con millis para no clavar el micro
  {

    // BUCLE MEDICION DE CARGA-----------------------------------------------------------------------------
    while (muestras_count < NUM_muestras)
    {
      sum += analogRead(medicion_voltaje); // tomar un número de muestras analógicas y sumarlas
      muestras_count++;
    }
    /*calcular la tensión de utilizar 5,0 para una tensión 5.0V ADC de referencia 5.015V es la tensión de referencia calibrada
     */
    voltaje = ((float)sum / (float)NUM_muestras * 5.015) / 1024.0;
    // Envia voltaje para su visualización en tensión Serial Monitor
    // multiplicado por 11 cuando se utiliza divisor de tensión que
    // divide por 11.
    // 11.132 es el valor calibrado del división de tensión

    voltaje_de_bateria = voltaje * 11.132;
    diferencia_tension_carga_descarga = (voltaje_de_bateria - 46.4*0.0); // 46.4 voltaje de referencia tomado para bateria descargada y 50.8 para bateria cargada

    if (diferencia_tension_carga_descarga >= 0)
    {
      Porcentaje_carga = map(diferencia_tension_carga_descarga, 0.00, 55, 0, 100); // convierte el promedio de las tensiones medidas a porcentaje de carga bateria
    }
    else
    {
      Porcentaje_carga = 0;
    }

    muestras_count = 0;
    sum = 0;

    // bucle lectura de sensor  DS18B20------------------------------------------------------------

    sensors.requestTemperatures();            // Se envía el comando para leer la temperatura
    Temperatura = sensors.getTempCByIndex(0); // Se obtiene la temperatura en ºC
    temperatura_ok = (Temperatura - error_absoluto);
    tiempoUltimaLectura = millis(); // actualizamos el tiempo de la última lectura
  }

  if (millis() - tiempoUltimaLectura_1 > retardo_1) // delay de encendido realizado con millis para no clavar el micro
  {

    //--------- bucle Lectura del Sensor humedad-----------------------------------------------------
    humedad_ambiente = dht.readHumidity();        // Leemos la Humedad
    temperatura_ambiente = dht.readTemperature(); // Leemos la temperatura en grados Celsius

    if (isnan(humedad_ambiente) || isnan(temperatura_ambiente))
    {
      //Serial.println("falla en lectura sensor DHT22!");
      return;
    }

    // impresion monitor serie-----------------------------------------------------------------------------

    /*Serial.print("\t\t--- Bateria ---\nTension: ");
    Serial.print(voltaje_de_bateria);
    Serial.print(" V\t");

    Serial.print("Carga: ");
    Serial.print(Porcentaje_carga);
    Serial.print(" %\t");

    Serial.print("Temperatura: ");
    Serial.print(temperatura_ok);
    Serial.println(" °C\t");

    Serial.print("\t\t--- Ambiente ---\nHumedad: ");
    Serial.print(humedad_ambiente);
    Serial.print(" %\t");

    Serial.print("Temperatura: ");
    Serial.print(temperatura_ambiente);
    Serial.println(" °C");*/
/*
    Serial.print("diferencia_tension_carga_descarga : ");
    Serial.print(diferencia_tension_carga_descarga);
    Serial.println(" v ");*/

    //---------------actualizacion de valores para maestros
    p = Porcentaje_carga;     // carga bateria
    t = temperatura_ok;       // temperatura bateria
    h = humedad_ambiente;     // humedad ambiente
    T = temperatura_ambiente; // temperatura ambiente

    tiempoUltimaLectura_1 = millis(); // actualizamos el tiempo de la última lectura
  }

} // fin void loop---------------------------------------------------------------------------------------------------

// funciones -----------------------------------------------------------------------------------
void eventoRecepcion()
{
  if (Wire.read() == 'T')
  {
    S = true;
    message = String(temperatura_ok) + '\n';
    return;
  }

  if (Wire.read() == 'C')
  {
    S = true;
    message = String(Porcentaje_carga) + '\n';
    return;
  }
  if (Wire.read() == 'U')
  {
    S = true;
    message = String(temperatura_ambiente) + '\n';
    return;
  }
  if (Wire.read() == 'H')
  {
    S = true;
    message = String(humedad_ambiente) + '\n';
    return;
  }
}

// function that executes whenever data is requested by master

void eventoSolicitud()
{

  if (S == true)
  {
    Wire.write(message.length());
    S = false;
  }
  else
  {
    Wire.println(message);
  }
}
