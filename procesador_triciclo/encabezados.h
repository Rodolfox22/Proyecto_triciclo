#define CANTDATOS 7
#define DeltaTImprime 120000 // muestreo de datos en SD [ms]
#define TIEMPOSUMA 1000

void contadorPulsos();
void serialEvent();
void datosRecibidos(String datos);
void enviarDatos();
void loguearDatos();
void imprimirEnSD(String &dato);
float calcularVelocidad(int rueda, int pulsosPorVuelta);
String procesarDato(int lecturadatos[]);
String prepararDatosSD();

// Temperatura ambiente, Humedad, Velocidad, Trip, Odometro, temperatura bateria, Carga batería
int lectura[CANTDATOS] = 225, 20, 8, 21, 36477, 348, 82;

const int chipSelect = 10; // SD
String dataString = ".";   // DATO
String inputString = "";
String lecturas = "";
bool datoRecibidoCompleto = false;
bool datoEnviadoCompleto = false;

int n_muestras = 10; // Promedio dato

long contador = 0;                 // contador pulsos
int rueda = 4;                     // Diametro de la rueda en mm
unsigned long UltimoTiempoImprime, // muestreo
    TiempoImprime,                 // muestreo
    ContadorImprime,               // muestreo
    anteriorSuma;
unsigned long anterior = 0; // muestreo
float vel = 0;

String ultimoDato = "";

// El guiño es el valor que me anuncia que luces deben prender
String guinho = "";
// Este valor es el que me avisa en que valor del odometro inicia el trip
int trip = 0;

int cicloJS = 0;
