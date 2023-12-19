#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SoftwareSerial.h>

#include "instrucciones.h"

AsyncWebServer server(80);
SoftwareSerial ComSerial(MYPORT_RX, MYPORT_TX, false);

int variable_RX = 0;
int variable_TX = 0;
//const char *matriz[2] = {"OFF", "ON"};

    void
    setup()
{
  Serial.begin(9600);
  pinMode(MYPORT_RX, INPUT);
  pinMode(MYPORT_TX, OUTPUT);

  // ComSerial.begin(9600);
  /*if (!ComSerial)
  { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid EspSoftwareSerial pin configuration, check config");
  }*/
  tiempo_anterior = millis();

  if (!SPIFFS.begin())
  {
    //Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  modoAP();
  // modoSTA();

  servidor();
}

void loop()
{
}

void servidor()
{ // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  //  Route to load style.css file
  server.on("/estilos.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/estilos.css", "text/css"); });

  //  Route to load icon file
  server.on("/Unraf.svg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/Unraf.svg", "image/svg+xml"); });

  // Route to load main.js file
  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/main.js", "text/js"); });

  server.on("/ACTUALIZAR", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    //Serial.println("Leyendo puerto serie");
    String datoSerie = "";
    /*if (true)
  {
    datoSerie = "Dato recibido: ";
    datoSerie += matriz[variable_RX];
    datoSerie += "\tDato enviado: ";
    datoSerie += matriz[variable_TX];
    variable_TX = !variable_TX;
    Serial.println(datoSerie);
*/
    if (ComSerial.available()) 
  {
      //datos_recibidos++;
      datoSerie = ComSerial.readString();
      //Serial.println(datoSerie);
    while (ComSerial.available()) {
      ComSerial.read();
      //Serial.print(".");
    }
    //datoSerie="{\"humedad\":76.2,\"temperatura\":22.5,\"velocidad\":7,\"trip\":0,\"odometro\":0,\"temp_bat\":22.25,\"carga\":0,\"guinho\":\"derecha\"}";
    request->send(200, "text/plain", datoSerie); // Devolver datos
  } else {
    Serial.println("No es posible recibir datos desde el puerto Serial");
    request->send(500, "text/plain", "Error al procesar la solicitud");
  } });

  server.on("/ENVIAR", HTTP_POST, [](AsyncWebServerRequest *request)
            {
     // Leer el cuerpo de la solicitud
    datosJson = request->getParam("plain")->value();
    //ComSerial.println(datosJson);
    //Serial.print("Datos enviados: ");
    //Serial.println(datosJson);
    // Enviar respuesta al cliente
    request->send(201, "text/plain", "Datos recibidos correctamente"); });

  server.begin();
}

void modoAP()
{
  delay(1000);
  Serial.println("Connecting to WiFi..");

  // Modo Acces point

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid_AP, password_AP);

  Serial.print("Servidor: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("Local: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}

void modoSTA()
{
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {

    delay(1000);
    Serial.println("Connecting to WiFi..");

    Serial.println(WiFi.localIP());
  }
}

String processor(const String &var)
{
  if (var == "ACTUALIZAR")
  {
   /* if (//ComSerial.available())
    {
      datos_recibidos++;
      String datos = // ComSerial.readString();
          Serial.println(datos);
      while (//ComSerial.available())
      {
        // ComSerial.read();
      }

      return datos;
    }*/
  }

  return "error";
}
