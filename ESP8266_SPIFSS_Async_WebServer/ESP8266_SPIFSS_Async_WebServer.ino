#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
// #include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include "instrucciones.h"

AsyncWebServer server(80);
StaticJsonDocument<300> estadoPant;

void setup()
{
  Serial.begin(9600);
  // espSerial.begin(115200);
  tiempo_anterior = millis();

  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  modoAP();
  // modoSTA();

  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);

  servidor();
}

void loop()
{
}

void servidor()
{ // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String()); });

  //  Route to load style.css file
  server.on("/estilos.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/estilos.css", "text/css"); });

  //  Route to load icon file
  server.on("/Unraf.svg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/Unraf.svg", "image/svg+xml"); });

  server.on("/bateria.svg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/bateria.svg", "image/svg+xml"); });

  server.on("/temp.svg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/temp.svg", "image/svg+xml"); });

  // Route to load main.js file
  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/main.js", "text/js"); });

  server.on("/ENVIAR", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("data", true)) {   // Verifica si hay un parámetro 'data' en el cuerpo de la solicitud
      AsyncWebParameter* p = request->getParam("data", true);  // Obtener el parámetro enviado con el nombre 'data'
      String dataRecibida = p->value();  // Almacenar los datos recibidos
      
      /*// Escribir los datos recibidos en el puerto serie
      espSerial.println(dataRecibida);
      Serial.println("Datos recibidos desde el cliente: " + dataRecibida);*/
      
      /*//Espera respuesta del puerto serie
      unsigned long startMillis = millis();
      while (!espSerial.available())
      {
       if (millis() - startMillis > 2000) {  // Timeout de 2 segundos
        request->send(500, "text/plain", "Error: No se recibió respuesta en el tiempo esperado.");
        return;  // Salir del manejador si no hay respuesta
      }
      }
      */

    // Leer la respuesta del puerto serie
    //String respuesta = espSerial.readString();

    /*// Lineas para saber si funciona la llamada al servidor
    conteo++;
    if (conteo>99)
    {
      conteo=0;
    }
    
    String respuesta = String(conteo);*/

    salidaParalelo(dataRecibida);


    String respuesta = entradaParalelo();
    
    if (respuesta.length() > 0) {
      // Enviar la respuesta del dispositivo conectado al cliente
      request->send(200, "text/plain", respuesta);
    } else {
      request->send(500, "text/plain", "Error: Respuesta vacía del dispositivo.");
    }
    } else {
      // Si no se recibe el parámetro 'data'
      request->send(400, "text/plain", "Falta el parámetro 'data'.");
    } });

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
    // Serial.println("Connecting to WiFi..");
    // Serial.println(WiFi.localIP());
  }
}

void salidaParalelo(String &datos)
{
  // Serial.println(datos);
  DeserializationError error = deserializeJson(estadoPant, datos);
  if (error)
  {
    Serial.println("Los datos recibidos por el puerto serie son incompatibles con Json");
    return;
  }

  String guinho = String(estadoPant["guinho"].as<const char *>());

  Serial.println(guinho);
  if (guinho.equals(textoDerecha))
  {
    digitalWrite(D8, HIGH);
    digitalWrite(D7, LOW);
    return;
  }

  if (guinho.equals(textoIzquierda))
  {
    digitalWrite(D7, HIGH);
    digitalWrite(D8, LOW);
    return;
  }

  if (guinho.equals(textoBaliza))
  {
    digitalWrite(D7, HIGH);
    digitalWrite(D8, HIGH);
    return;
  }

  digitalWrite(D7, LOW);
  digitalWrite(D8, LOW);
}

String entradaParalelo()
{
  String respuesta = textoApagado;
  int derechoMega = digitalRead(D5);
  int izquierdoMega = digitalRead(D6);
  /*Serial.print("Derecho: ");
  Serial.println(derechoMega);
  Serial.print("Izquierdo: ");
  Serial.println(izquierdoMega);*/

  if (derechoMega)
  {
    respuesta = textoDerecha;
  }

  if (izquierdoMega)
  {
    respuesta = textoIzquierda;
  }

  if (derechoMega & izquierdoMega)
  {
    respuesta = textoBaliza;
  }

  if (!derechoMega & !izquierdoMega)
  {
    respuesta = textoApagado;
  }

  return respuesta;
}
