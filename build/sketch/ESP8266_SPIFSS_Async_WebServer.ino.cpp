#include <Arduino.h>
#line 1 "C:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\ESP8266_SPIFSS_Async_WebServer.ino"
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SoftwareSerial.h>
#include "instrucciones.h"

AsyncWebServer server(80);
SoftwareSerial ComSerial(D5, D6);

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

  // Calcular Velocidad lectura[2] "velocidad" / "/VEL"
  if (var == "VEL")
  {
    Serial.print("Velocidad: ");
    Serial.println(velocidad);
    return String(velocidad);
  }
  // Trip lectura[3] "trip" / "/TRIP"
  if (var == "TRIP")
  {
    Serial.print("Trip: ");
    Serial.println(trip);
    return String(trip);
  }

  // Odometro lectura[4] "odometro" / "/ODO"
  if (var == "ODO")
  {
    Serial.print("Odometro: ");
    Serial.println(odometro);
    return String(odometro);
  }
  /* return "error";
 }

 String processor2(const String &var)
 {*/

  /*// Temperatura ambiente   lectura[0] "temperatura" / "/TEMPERATURA"
  if (var == "TEMPERATURA")
  {
    Serial.print("Temperatura: ");
    Serial.println(temperatura);
    return String(temperatura);
  }*/

  /*// Humedad lectura[1] "humedad" / "/HUMEDAD"
  if (var == "HUMEDAD")
  {
    Serial.print("Humedad: ");
    Serial.println(humedad);
    return String(humedad);
  }
*/

  // Temperatura bateria lectura[5] "temp_bat" / "/TEMP_BAT"
  if (var == "TEMP_BAT")
  {
    Serial.print("Temperatura bateria: ");
    Serial.println(temp_bat);
    return String(temp_bat);
  }

  // Carga batería lectura[6] "carga" / "/CARGA"
  if (var == "CARGA")
  {
    Serial.print("Carga: ");
    Serial.println(carga);
    return String(carga);
  }

  /*
      if (var == "RETROCESO")
      {
        return Retroceso;
      }
  */
  return "error";
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(9600);
  ComSerial.begin(9600);
  pinMode(Rele_Pin, OUTPUT);
  tiempo_anterior = millis();

  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

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

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Por el momento no cargo el archivo CSS para que analizar en profundidad la funcionalidad
  //  Route to load style.css file
  server.on("/estilos.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/estilos.css", "text/css"); });

  // Route to load main.js file
  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/main.js", "text/js"); });

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

  // Route to set GPIO to CHANGE
  server.on("/RELE=CHANGE", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (digitalRead(Rele_Pin)==0)
      digitalWrite(Rele_Pin, HIGH);
    else
      digitalWrite(Rele_Pin, LOW);
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  /*// Temperatura ambiente   lectura[0] "temperatura" / "/TEMPERATURA"
    server.on("/TEMPERATURA", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", String(), false, processor); });
  */

  /*// Humedad                lectura[1] "humedad"     / "/HUMEDAD"
  server.on("/HUMEDAD", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });*/

  // Calcular Velocidad     lectura[2] "velocidad"   / "/VEL"
  server.on("/VEL", HTTP_GET, [](AsyncWebServerRequest *request)
            {
            // Probar cambio de la pagina
            velocidad++;
            if (velocidad >= 250)
            {
              velocidad = 0;
            }
               request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Trip                   lectura[3] "trip"        / "/TRIP"
  server.on("/TRIP", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Odometro               lectura[4] "odometro"    / "/ODO"
  server.on("/ODO", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Temperatura bateria    lectura[5] "temp_bat"    / "/TEMP_BAT"
  server.on("/TEMP_BAT", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Carga batería          lectura[6] "carga"       / "/CARGA"
  server.on("/CARGA", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  /*
      server.on("/RETROCESO", HTTP_GET, [](AsyncWebServerRequest *request)
                {
        //Retroceso = leerDato('R');
        request->send(SPIFFS, "/index.html", String(), false, processor); });

    */
  server.begin();
}

void loop()
{

  if (tiempo_solicitud >= millis() - tiempo_anterior)
  {
    if (datos_recibidos)
    {
      setearVariables();
      datos_recibidos = 0;
    }

    Serial.println(velocidad);
    tiempo_anterior = millis();
  }
}

void setearVariables()
{
  temperatura = variables[0].toFloat() / 10.0;
  humedad = variables[1].toInt();
  velocidad = variables[2].toInt();
  trip = variables[3].toInt();
  odometro = variables[4].toInt();
  temp_bat = variables[5].toFloat() / 10.0;
  carga = variables[6].toInt();
}

void serialEvent()
{
  if (ComSerial.available())
  {
    datos_recibidos++;
    char cantDatos = ComSerial.read(); // En el string envío la cantidad de datos existentes
    int cant_datos = int(cantDatos);

    for (int indice = 0; indice < cant_datos; indice++)
    {
      if (indice < cant_datos - 1)
      {
        variables[indice] = ComSerial.readStringUntil(',');
      }
      else
      {
        variables[indice] = ComSerial.readStringUntil('/');
      }
    }
  }
}

