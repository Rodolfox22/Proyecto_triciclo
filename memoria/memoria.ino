/* estado_led_memoria.ino
Código de https://www.filipeflop.com/

El primer intento se utiliza en un archivo para guardar el estado on/off
de un LED. Presionando el botón podemos cambiar el estado. Incluso si la
energía se apaga, el estado del LED se guardará en la memoria. Cuando
conectamos nuevamente la placa, el sistema busca el estado guardado en la
memoria y actualiza el estado del LED.

Es importante notar que el programa debe grabarse dos veces. La primera
vez la línea 62 ( escritura de archivo ( "off", "/state.txt") ) debe ser
sin comentar, creando un archivo en la memoria. En la segunda y demás
programaciones, la línea debe ser comentada, pues el archivo ya existirá
en la memoria.
*/

#include "FS.h"

int led = D1;
int llave = D0;

/**
 * escribe contenido en un archivo
 * string state - contenido a escribir en el archivo
 * string path - archivo que se va a escribir
 */

void writeFile(String state, String path)
{
  // Abre el archivo para escribir ("w" write))
  // Sobreescribe el contenido del archivo
  File rFile = SPIFFS.open(path, "w+");
  if (!rFile)
  {
    Serial.println("Error al abrir archivo!");
  }
  else
  {
    rFile.println(state);
    Serial.print("grabó estado: ");
    Serial.println(state);
  }
  rFile.close();
}

/**
 * el contenido de un archivo
 * param cadena path - archivo que se va a leer
 * return string - contenido leído del archivo
 */

String readFile(String path)
{
  File rFile = SPIFFS.open(path, "r");
  if (!rFile)
  {
    Serial.println("Error al abrir archivo!");
  }
  String content = rFile.readStringUntil('\r');
  Serial.print("lectura de estado: ");
  Serial.println(content);
  rFile.close();
  return content;
}

/**
 * @desc inicializa el sistema de archivos
 */

void openFS(void)
{
  // Abre el sistema de archivos
  if (!SPIFFS.begin())
  {
    Serial.println("\nError al abrir el sistema de archivos");
  }
  else
  {
    Serial.println("\nSistema de archivos abierto con éxito!");
  }
}

void setup()
{
  pinMode(led, OUTPUT);
  pinMode(llave, INPUT);
  Serial.begin(9600);
  openFS();

  // en el primer upload de programa el archivo state.txt se debe crear con el contenido "off"
  // en el segundo upload la línea debe ser comentada.
  // writeFile("off", "/state.txt"); // esta línea debe ser comentada

  // comprueba el último estado del LED y se activa de acuerdo
  String state = readFile("/state.txt");
  if (state == "on")
  {
    digitalWrite(led, HIGH);
  }
  else if (state == "off")
  {
    digitalWrite(led, LOW);
  }
}

void loop()
{
  /*
   * comprueba el estado anterior que salvo en el archivo,
   * activa el LED de acuerdo y escribe el nuevo
   * estado del archivo.
   */
  if (digitalRead(llave) == LOW) // chave
  {
    String state = readFile("/state.txt");
    if (state == "off")
    {
      writeFile("on", "/state.txt");
      digitalWrite(led, HIGH);
    }
    else if (state == "on")
    {
      writeFile("off", "/state.txt");
      digitalWrite(led, LOW);
    }
    while (digitalRead(llave) == LOW)
      ;
  }
}