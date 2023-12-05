//                        Variable     Id HTML       Etiqueta
// Temperatura ambiente   lectura[0] "temperatura"  "/TEMPERATURA"
// Humedad                lectura[1] "humedad"      "/HUMEDAD"
// Calcular Velocidad     lectura[2] "velocidad"    "/VEL"
// Trip                   lectura[3] "trip"         "/TRIP"
// Odometro               lectura[4] "odometro"     "/ODO"
// temperatura bateria    lectura[5] "temp_bat"     "/TEMPBAT"
// Carga batería          lectura[6] "carga"        "/CARGA"

#define CANTDATOS 7

String dato_recibido = "";
String dato_actual = "";

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

const int Rele_Pin = 2;

unsigned long tiempo_anterior = 0;
unsigned long tiempo_solicitud = 2000;

String Estado_Pin;

int datos_recibidos=0;
String variables[] = {"210","60", "0", "0", "250", "325", "85"};
float temperatura = 21.0;
int humedad = 60;
int velocidad = 0;
int trip = 0;
int odometro = 250;
float temp_bat = 32.5;
int carga = 85;

void setearVariables();
