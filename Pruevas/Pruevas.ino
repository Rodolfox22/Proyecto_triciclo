void enviarDato(String = "_");
void procesarDato();
void cambiarDato();

#define CANTDATOS 10
#define DeltaTImprime 2000

String dataString = "."; // DATO
String inputString = "";
bool datoRecibidoCompleto = false;
bool datoEnviadoCompleto = false;

int lectura[CANTDATOS] = {23, 70, 10, 100, 542, 1050, 78, 35, 132, 120};
String lecturaEtiquetas[CANTDATOS] = {"TAMB", "HAMB", "VEL", "DIS", "ODO", "REL", "RET", "TBAT", "VBAT", "IBAT"};

long UltimoTiempoImprime, // muestreo
    TiempoImprime;        // muestreo

void setup()
{
  ////////////SERIAL////////////
  Serial.setTimeout(10);
  Serial1.setTimeout(10);
  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop()
{
  TiempoImprime = millis() - UltimoTiempoImprime;
  if (TiempoImprime > DeltaTImprime)
  {
    Serial.println(".");
    logearDatos();
    UltimoTiempoImprime = millis();
  }
  delay(100);
  Serial.print(".");
}

void serialEvent()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '_')
    {
      datoRecibidoCompleto = true;
    }
  }
}

void enviarDato(String dato)
{
  Serial.print(dato);
  Serial1.print(dato);
}

void procesarDato()
{
  String datoCompleto = "";

  for (int i = 0; i < CANTDATOS; i++)
  {
    datoCompleto += lecturaEtiquetas[i];
    datoCompleto += ": ";
    datoCompleto += lectura[i];
    if (i < CANTDATOS - 1)
      datoCompleto += ";";
  }

  datoCompleto += "_";

  enviarDato(datoCompleto);
}

void cambiarDato()
{
  for (int i = 0; i < CANTDATOS; i++)
  {
    lectura[i] = random(23; 2359);
  }
}