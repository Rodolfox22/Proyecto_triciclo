#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SoftwareSerial.h>

#include "instrucciones.h"

AsyncWebServer server(80);
//D5 Tx --- D6 Rx
SoftwareSerial ComSerial(MYPORT_RX, MYPORT_TX,false);


void setup()
{
  Serial.begin(9600);
  ComSerial.begin(9600);
  if (!ComSerial) { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid EspSoftwareSerial pin configuration, check config"); 
    }
  tiempo_anterior = millis();

  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  modoAP();
  //modoSTA();

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
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/ENVIAR", HTTP_POST, [](AsyncWebServerRequest *request)
            {
     // Leer el cuerpo de la solicitud
    datosJson = request->getParam("plain")->value();
    ComSerial.println(datosJson);
    Serial.print("Datos enviados: ");
    Serial.println(datosJson);
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
    if (ComSerial.available())
    {
      datos_recibidos++;
      String datos = ComSerial.readString();
      Serial.println(datos);
      while(ComSerial.available())
      {
        ComSerial.read();
      }

      return datos;
    }
  }

  return "error";
}
