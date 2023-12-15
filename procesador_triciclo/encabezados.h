#define DeltaTImprime 120000 // muestreo de datos en SD [ms]
#define TIEMPODISTANCIA 1000 // refresco de datos de velocidad y distancia
#define TIEMPOGUINHO 50      // refresco de datos de lectura de botones
#define TIEMPODESTELLO 600   // tiempo de destello de guiños
#define PINGDERECHO 2
#define PINGIZQUIERDO 4
#define PINBALIZA 5
#define PINLDERECHO 6
#define PINLIZQUIERDO 7
#define PINDS18B20 9
#define PINDHT 11
#define PINCARGABAT A0
#define PINCONTADOR 3
#define PINSD 53

void contadorPulsos();
void serialEvent();
void datosRecibidos(String datos);
void enviarDatos();
void loguearDatos();
void imprimirEnSD(String &dato);
void guardarOdometro(String &logActual);
int buscarEnSD(String &variableBuscada);
int calcularVelocidad(int rueda, int pulsosPorVuelta);
void calcularDistancia();
String prepararDatosSD();
void leerBotones();
void escucharPuerto();
void setearLuces();
void encender();
void recuperarDatos();
void verOdometro();
String valorGuinho(int variable, const String &texto);

// Temperatura ambiente, Humedad, Velocidad, Trip, Odometro, temperatura bateria, Carga batería
float humedad = 80;       // humedad_ambiente
float temperatura = 21.2; // temperatura_ambiente
int velocidad = 12;       // OK
int trip = 0;             // OK
int odometro = 432;       // OK
float temp_bat = 25.4;    // temperatura_ok
int carga = 80;           // porcentaje_carga
volatile int pulsosTotales = 0;

// El guiño es el valor que me anuncia que luces deben prender
const String textoApagado = "apagado";
const String textoDerecha = "derecha";
const String textoIzquierda = "izquierda";
const String textoBaliza = "baliza";

String guinho = "";
String guinhoActual = textoApagado;
String guinhoAnterior = "";
int gDerecho = 0;
int gIzquierdo = 0;
int derecho = 0;
int izquierdo = 0;
int baliza = 0;

bool reloj = false;
bool datoInicial = true;

const int chipSelect = PINSD; // SD
String dataString = ".";      // DATO
String inputString = "";
String lecturas = "";
bool datoRecibidoCompleto = false;
bool datoESP8266Completo = false;

// Promedio dato
int n_muestras = 20;

// contador pulsos
long contador = 0;
// Diametro de la rueda en mm
int rueda = 4;
// muestreo
unsigned long UltimoTiempoImprime = 0;
// muestreo
unsigned long ContadorImprime = 0;
unsigned long anteriorSuma = 0;
unsigned long anteriorDestello = 0;
unsigned long anteriorGuinho = 0;
unsigned long anteriorDistancia = 0;
unsigned long anteriorEnvio = 0;
// muestreo
unsigned long anterior = 0;
float vel = 0;

int cicloJS = 0;

void inicializar()
{
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

  /////////   CONTADOR   ////////
  attachInterrupt(digitalPinToInterrupt(PINCONTADOR), contadorPulsos, RISING);

  pinMode(PINGDERECHO, INPUT_PULLUP);
  pinMode(PINGIZQUIERDO, INPUT_PULLUP);
  pinMode(PINBALIZA, INPUT_PULLUP);
  pinMode(PINLDERECHO, OUTPUT);
  pinMode(PINLIZQUIERDO, OUTPUT);
  pinMode(PINCARGABAT, INPUT);
}