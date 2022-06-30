# 1 "c:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\ESP8266_SPIFSS_Async_WebServer.ino"
# 2 "c:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\ESP8266_SPIFSS_Async_WebServer.ino" 2
# 3 "c:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\ESP8266_SPIFSS_Async_WebServer.ino" 2
# 4 "c:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\ESP8266_SPIFSS_Async_WebServer.ino" 2
# 5 "c:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\ESP8266_SPIFSS_Async_WebServer.ino" 2

# 7 "c:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\ESP8266_SPIFSS_Async_WebServer.ino" 2

const char *ssid = "UNRaf_Libre";
const char *password = "unraf2021";
/*

const char *ssid = "xxxxxxxxx";

const char *password = "yyyyyyy";

*/
# 15 "c:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\ESP8266_SPIFSS_Async_WebServer.ino"
/*

const char *ssid_AP = "Triciclo";

const char *password_AP = "unraf2022";

*/
# 19 "c:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\ESP8266_SPIFSS_Async_WebServer.ino"
IPAddress ip(192, 168, 0, 10);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

const int Rele_Pin = 2;

unsigned long demora = 0;
unsigned long tiempo_solicitud = 5000;

String Estado_Pin;
String Velocidad = "10";
String Carga = "20";
String Temp_bat = "35";
String Retroceso;
String Humedad = "75";
String Temperatura = "10";
String Temporal;

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

  else if (var == "VEL")
  {
    Serial.print("Velocidad: ");
    Serial.println(Velocidad);
    return Velocidad;
  }

  else if (var == "HUMEDAD")
  {
    Serial.print("Velocidad: ");
    Serial.println(Velocidad);
    return Humedad;
  }

  else if (var == "TEMP_BAT")
  {
    Serial.print("Temperatura bateria: ");
    Serial.println(Temp_bat);
    return Temp_bat;
  }

  else if (var == "TEMPERATURA")
  {
    Serial.print("Temperatura: ");
    Serial.println(Temperatura);
    return Temperatura;
  }

  else if (var == "RETROCESO")
  {
    return Retroceso;
  }

  else if (var == "CARGA")
  {
    Serial.print("Carga: ");
    Serial.println(Carga);
    return Carga;
  }
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(Rele_Pin, 0x01);

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
# 133 "c:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\ESP8266_SPIFSS_Async_WebServer.ino"
  // TODO como conectar directamente? necesita una instruccion especial para que funcione como servidor?

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Route to load style.css file
  server.on("/estilos.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/estilos.css", "text/css"); });

  // Route to load main.js file
  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/main.js", "text/js"); });

  // Route to set GPIO to HIGH
  server.on("/RELE=ON", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    digitalWrite(Rele_Pin, 0x0);
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Route to set GPIO to LOW
  server.on("/RELE=OFF", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    digitalWrite(Rele_Pin, 0x1);
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Route to set GPIO to CHANGE
  server.on("/RELE=CHANGE", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (digitalRead(Rele_Pin)==0)
      digitalWrite(Rele_Pin, 0x1);
    else
      digitalWrite(Rele_Pin, 0x0);
    request->send(SPIFFS, "/index.html", String(), false, processor); });

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

  server.on("/VELOCIDAD", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    //Velocidad = leerDato('V');
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/RETROCESO", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    //Retroceso = leerDato('R');
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/CARGA", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    //Carga = leerDato('C');
    request->send(SPIFFS, "/index.html", String(), false, processor); });
  server.begin();
}
void loop()
{
  unsigned long tiempo_inicio = 0;
  unsigned long tiempo_fin = 0;
  unsigned long tiempo_lectura = 0;

  if (millis() - demora == tiempo_solicitud)
  {/*

    tiempo_inicio = millis();



    dato_recibido = leerDato('C');



    tiempo_fin = millis();

    tiempo_lectura = tiempo_fin - tiempo_inicio;

    Serial.print("Tiempo de lectura: ");

    Serial.println(tiempo_lectura);

    Serial.println("Datos: ", dato_recibido);

    demora = millis();*/
# 217 "c:\\Users\\Usuario\\Documents\\Arduino\\Proyecto_triciclo\\ESP8266_SPIFSS_Async_WebServer\\ESP8266_SPIFSS_Async_WebServer.ino"
  }
}
