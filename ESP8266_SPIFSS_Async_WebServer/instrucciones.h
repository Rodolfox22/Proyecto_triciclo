//                        Variable     Id HTML       Etiqueta
// Temperatura ambiente   lectura[0] "temperatura"  "/TEMPERATURA"
// Humedad                lectura[1] "humedad"      "/HUMEDAD"
// Calcular Velocidad     lectura[2] "velocidad"    "/VEL"
// Trip                   lectura[3] "trip"         "/TRIP"
// Odometro               lectura[4] "odometro"     "/ODO"
// temperatura bateria    lectura[5] "temp_bat"     "/TEMPBAT"
// Carga batería          lectura[6] "carga"        "/CARGA"

String datosJson;

/*
const char *ssid = "UNRaf_Libre";
const char *password = "unraf2021";
*/

const char *ssid = "Wi-Fi BRITEZ";
const char *password = "50376britez";

const char *ssid_AP = "Triciclo";
const char *password_AP = "unraf2022";

IPAddress ip(192, 168, 0, 20);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

unsigned long tiempo_anterior = 0;
unsigned long tiempo_solicitud = 2000;

int datos_recibidos = 0;
float temperatura = 21.0;
int humedad = 60;
int velocidad = 0;
int trip = 0;
int odometro = 250;
float temp_bat = 32.5;
int carga = 85;

void setearVariables();
void servidor();
String processor(const String &var);
void modoAP();
void modoSTA();
