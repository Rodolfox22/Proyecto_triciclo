#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>
#include "instrucciones.h"

const char *ssid = "UNRaf_Libre";
const char *password = "unraf2021";
/*
const char *ssid = "xxx";
const char *password = "yyyyyyy";
*/

/*
const char *ssid_AP = "Triciclo";
const char *password_AP = "unraf2022";
*/
IPAddress ip(192, 168, 0, 10);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

const int Rele_Pin = 2;

String Estado_Pin;
String Velocidad;
String Carga;
String Temperatura_bat;
String Retroceso;
String Humedad;
String Temperatura;
// String Estado_Pin;

AsyncWebServer server(80);

String processor(const String &var)
{

  if (var == "ESTADO_RELE")
  {
    if (digitalRead(Rele_Pin) == 0)
    {
      Estado_Pin = "Encendido";
    }
    else
    {
      Estado_Pin = "Apagado";
    }
    return Estado_Pin;
  }

  else if (var == "VELOCIDAD")
  {
    return Velocidad;
  }

  else if (var == "HUMEDAD")
  {
    return Humedad;
  }

  else if (var == "TEMPERATURA_BAT")
  {
    return Temperatura_bat;
  }

  else if (var == "TEMPERATURA")
  {
    return Temperatura;
  }

  else if (var == "RETROCESO")
  {
    return Retroceso;
  }

  else if (var == "CARGA")
  {
    return Carga;
  }
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(Rele_Pin, OUTPUT);

  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Inicio I2C
  Wire.begin();
  // Connect to Wi-Fi

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());

  // Modo Acces point
  /*
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid_AP, password_AP);

      Serial.print("Servidor: ");
      Serial.println(WiFi.softAPIP());
      Serial.print("Local: ");
      Serial.println(WiFi.localIP());
      delay(1000);
    */

  // TODO como conectar directamente? necesita una instruccion especial para que funcione como servidor?
  
  //?Rutas
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Route to load style.css file
  server.on("/estilos.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/estilos.css", "text/css"); });

  // Route to set GPIO to HIGH
  server.on("/RELE=ON", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    digitalWrite(Rele_Pin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Route to set GPIO to LOW
  server.on("/RELE=OFF", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    digitalWrite(Rele_Pin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/TEMPERATURA_BAT", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
    Temperatura_bat = leerDato('T');
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/TEMPERATURA", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
    Temperatura = leerDato('U');
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/HUMEDAD", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
    Humedad = leerDato('H');
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/VELOCIDAD", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Velocidad = leerDato('V');
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/RETROCESO", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Retroceso = leerDato('R');
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/CARGA", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Carga = leerDato('C');
    request->send(SPIFFS, "/index.html", String(), false, processor); });
  server.begin();
}
void loop()
{
}
