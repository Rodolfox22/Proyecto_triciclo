#define MY_TX D8
#define MY_RX D7
#define TIEMPO 8000
#define LED1 7
#define LED2 6

unsigned long tiempoAnterior = 0;
int enviado = 0;
int anteriorRecibido = 0;

void setup()
{
  pinMode(MY_TX, OUTPUT);
  pinMode(MY_RX, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  Serial.begin(9600);
}

void loop()
{
  int recibido = digitalRead(MY_RX);
  if (recibido != anteriorRecibido)
  {
    digitalWrite(LED1, recibido);
    anteriorRecibido = recibido;
  }
  
  if (millis() - tiempoAnterior >= TIEMPO)
  {
    digitalWrite(MY_TX, enviado);
    digitalWrite(LED2, enviado);
    Serial.print("Enviado: ");
    Serial.print(enviado);
    Serial.print("\tRecibido: ");
    Serial.println(recibido);
    enviado = !enviado;
    tiempoAnterior = millis();
  }
}
