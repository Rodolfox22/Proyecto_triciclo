#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SoftwareSerial.h>
#include "instrucciones.h"

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

  if (var == "VEL")
  {
    Serial.print("Velocidad: ");
    Serial.println(Velocidad);
    return Velocidad;
  }

  /*
    if (var == "HUMEDAD")
    {
      Serial.print("Velocidad: ");
      Serial.println(Humedad);
      return Humedad;
    }

    if (var == "TEMP_BAT")
    {
      Serial.print("Temperatura bateria: ");
      Serial.println(Temp_bat);
      return Temp_bat;
    }

    if (var == "TEMPERATURA")
    {
      Serial.print("Temperatura: ");
      Serial.println(Temperatura);
      return Temperatura;
    }

    if (var == "RETROCESO")
    {
      return Retroceso;
    }

    if (var == "CARGA")
    {
      Serial.print("Carga: ");
      Serial.println(Carga);
      return Carga;
    }
    if (var == "ODOMETRO")
    {
      Serial.print("Odometro: ");
      Serial.println(Odometro);
      return Odometro;
    }
    if (var == "TRIP")
    {
      Serial.print("Trip: ");
      Serial.println(Trip);
      return Trip;
    }*/
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(9600);
  ComSerial.begin(9600);
  pinMode(Rele_Pin, OUTPUT);

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

  /*Por el momento no cargo el archivo CSS para que analizar en profundidad la funcionalidad
  // Route to load style.css file
  server.on("/estilos.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/estilos.css", "text/css"); });*/

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

  /*// Route to set GPIO to CHANGE
  server.on("/RELE=CHANGE", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (digitalRead(Rele_Pin)==0)
      digitalWrite(Rele_Pin, HIGH);
    else
      digitalWrite(Rele_Pin, LOW);
    request->send(SPIFFS, "/index.html", String(), false, processor); });
*/

  server.on("/TEMP_BAT", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      //Temp_bat = leerDato('T');
      request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/TEMPERATURA", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      //Temperatura = leerDato('U');
      request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/HUMEDAD", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      //Humedad = leerDato('H');
      request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/VEL", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      Velocidad = String(num_Velocidad);
      request->send(SPIFFS, "/index.html", String(), false, processor); });
  /*
      server.on("/RETROCESO", HTTP_GET, [](AsyncWebServerRequest *request)
                {
        //Retroceso = leerDato('R');
        request->send(SPIFFS, "/index.html", String(), false, processor); });

      server.on("/CARGA", HTTP_GET, [](AsyncWebServerRequest *request)
                {
        //Carga = leerDato('C');
        request->send(SPIFFS, "/index.html", String(), false, processor); });
      server.begin();
    */
}

void loop()
{
  if (ComSerial.available())
  {
    char cantdatos = ComSerial.read();
    int cant_datos = int(cantdatos);
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
    if (ComSerial.available())
    {
      int descarte = ComSerial.readString();
      Serial.println("Existen datos de descarte")
    }
  }

  if (tiempo_solicitud >= millis() - tiempo_anterior)
  {
    int estado_inicial = estadoInicial();
    if (estado_inicial)
    {
      setearVariables();
      tiempo_anterior = millis();
    }
  }
}

void setearVariables()
{
  float temperatura = float(variables[0]) / 10;
  int humedad = variables[1];
  int velocidad = variables[2];
  int trip = variables[3];
  int odometro = variables[4];
  float temp_bat = float(variables[5]) / 10;
  int carga = variables[0];
}

int estadoInicial()
{
  int igualdad = 0;
  for (int valor = 0; valor < CANTDATOS; valor++)
  {
    if (variables[valor] != primera_lectura[valor])
    {
      igualdad++;
    }
  }
  return igualdad;
}