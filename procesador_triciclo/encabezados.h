void contador_pulsos();
void serialEvent();
void enviarDato(String = "_");
void procesarDato();
String floatToString(float, bool = false; int = 8, int = 2);
void loguearDatos();
void leerFecha();
void valorPromedio();
void calcularVelocidad();
void imprimirEnSD();

#define CANTDATOS 10
#define DeltaTImprime 1000 // muestreo [ms]

const int chipSelect = 10; // SD
String dataString = ".";   // DATO
String inputString = "";
bool datoRecibidoCompleto = false;
bool datoEnviadoCompleto = false;

float lectura[CANTDATOS];
String lecturaEtiquetas[CANTDATOS] = {"TAMB", "HAMB", "VEL", "DIS","ODO","REL","RET",   "TBAT", "VBAT", "IBAT"};

int n_muestras = 10; // Promedio dato

long contador = 0;        // contador pulsos
int p = 4;                // pares polos motor
long UltimoTiempoImprime, // muestreo
    TiempoImprime,        // muestreo
    ContadorImprime;      // muestreo
long anterior = 0;        // muestreo
float vel = 0;
