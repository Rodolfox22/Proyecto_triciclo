#define CANTDATOS 7
#define DeltaTImprime 120000 // muestreo de datos en SD [ms]
#define TIEMPOSUMA 1000
#define TIEMPOGUINHO 200
#define TIEMPODESTELLO 300
#define PINGDERECHO
#define PINGIZQUIERDO
#define PINBALIZA
#define PINLDERECHO
#define PINLIZQUIERDO

void contadorPulsos();
void serialEvent();
void datosRecibidos(String datos);
void enviarDatos();
void loguearDatos();
void imprimirEnSD(String &dato);
float calcularVelocidad(int rueda, int pulsosPorVuelta);
String prepararDatosSD();
void leerBotones();
void setearLuces();
void encender();

// Temperatura ambiente, Humedad, Velocidad, Trip, Odometro, temperatura bateria, Carga batería
int humedad = 80;
float temperatura = 21.2;
int velocidad = 12; // OK
int trip = 0; //OK
int odometro = 432;
float temp_bat = 25.4;
int carga = 80;
// El guiño es el valor que me anuncia que luces deben prender
String guinho = "";
String guinhoActual = "";
String guinhoAnterior = "";
int gDerecho = 0;
int gIzquierdo = 0;
int derecho = 0;
int izquierdo = 0;
int baliza = 0;

bool reloj = false;

const int chipSelect = 10; // SD
String dataString = ".";   // DATO
String inputString = "";
String lecturas = "";
bool datoRecibidoCompleto = false;
bool datoEnviadoCompleto = false;

// Promedio dato
int n_muestras = 10;

// contador pulsos
long contador = 0;
// Diametro de la rueda en mm
int rueda = 4;
// muestreo
unsigned long UltimoTiempoImprime = 0,
              // muestreo
    TiempoImprime = 0,
              // muestreo
    ContadorImprime = 0,
              anteriorSuma = 0,
              anteriorDestello = 0,
              anteriorGuinho = 0;
// muestreo
unsigned long anterior = 0;
float vel = 0;

int cicloJS = 0;
