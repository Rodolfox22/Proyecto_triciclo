//hacer mediicones diferenciales y agregar filtro RC a la entrada
//bajar el SPS y mejor alimentarlo con 3,3v

#include <Adafruit_ADS1015.h> //falta instalar libreria
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h" //falta instalar libreria
RTC_DS1307 RTC;

Adafruit_ADS1115 ads;

const int chipSelect = 10; //SD
String dataString = ".";   //DATO
int n_muestras = 10;       //Promedio dato

#define DeltaTImprime 1000      // muestreo [ms]
long contador = 0;             // contador pulsos
int p = 4;                     //pares polos motor
long  UltimoTiempoImprime   ,  // muestreo
      TiempoImprime         ,  // muestreo
      ContadorImprime       ;  // muestreo
long anterior = 0;             // muestreo
float vel = 0;

void setup() {
  ////////////SERIAL////////////
  Serial.setTimeout(10);
  Serial.begin(115200);

  ////////////RTC////////////
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  else {
    Serial.println("RTC running");
  }
  //RTC.adjust(DateTime(__DATE__, __TIME__)); //Cargarlo una vez para setearlo y despues sacar frase para que cuadno se reinicio no vuelva a la fecha vieja

  ////////////SD////////////
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");

  ////////////ADC///////////
  // Factor de escala
  //ads.setGain(GAIN_TWOTHIRDS);   // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);       // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);       // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);      // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);     // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  ads.setGain(GAIN_SIXTEEN);   // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  ads.begin();   // Iniciar el ADS1115
  ads.setSPS(ADS1115_DR_16SPS);
  //  if (!ads.begin()) {
  //    Serial.println("Failed to initialize ADS.");
  //    while (1);
  //  }
  //  else {
  //    Serial.println("ADS initialized.");
  //  }

  /////////CONTADOR////////
  attachInterrupt(digitalPinToInterrupt(3), contador_pulsos, RISING);
}

void loop() {
  TiempoImprime = millis() - UltimoTiempoImprime;
  if (TiempoImprime > DeltaTImprime) {
    ////////////Leer fecha////////////
    String dataString = "";
    DateTime now = RTC.now();
    dataString += String(now.day(), DEC);
    dataString += "/";
    dataString += String(now.month(), DEC);
    dataString += "/";
    dataString += String(now.year(), DEC);
    dataString += ",";
    dataString += String(now.hour(), DEC);
    dataString += ":";
    dataString += String(now.minute(), DEC);
    dataString += ":";
    dataString += String(now.second(), DEC);
    dataString += ",";

    ////////////ADS1115 Tomar valor promedio////////////
    //    int16_t adc0, adc3;
    //    //adc0 = ads.readADC_SingleEnded(0);
    //    //adc3 = ads.readADC_SingleEnded(3);
    //    float vsensor_V = 0;
    //    float vsensor_I = 0;
    //    for (int i = 0; i < n_muestras; i++)
    //    {
    //      ads.readADC_SingleEnded(0);
    //
    //      ads.readADC_SingleEnded(3);
    //      vsensor_V = vsensor_V + adc0;
    //      vsensor_I = vsensor_I + adc3;
    //    }
    //    vsensor_V = vsensor_V / n_muestras;
    //    vsensor_I = vsensor_I / n_muestras;
    //    //vsensor_V = vsensor_V / 4.489;
    //    //vsensor_I = vsensor_I * 0.833;

    int16_t tension;
    int16_t corriente;
    /* Be sure to update this value based on the IC and the gain settings! */
//    float multiplier = 0.007812F;
//    corriente = ads.readADC_Differential_0_1();
//    tension = ads.readADC_Differential_2_3();
//    dataString += String(tension*multiplier*-0.2245);
//    dataString += ",";
//    dataString += String(corriente*multiplier*-0.833);
//    dataString += ",";

    ////////////MEDIR VEL////////////
    float vel = 0;
    if (contador > 0) {
      float pulsos_sec = 1000000 / contador;
      float rad_s = (pulsos_sec / p) * 2 * 3.14;
      //float vel = rad_s*26*0.5*2.54*0.01*3.6;
      float vel = rad_s * 9.554;//rpm
    }
    dataString += String(vel);
    dataString += ",";
    ////////////Imprimir valor////////////
    Serial.println("//////////////Dato completo//////////////");
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      Serial.println(dataString);
    }
    else {
      Serial.println("Error SD");
    }
    UltimoTiempoImprime = millis();
    contador = 0;
  }
}

void contador_pulsos() {
  contador = micros() - anterior;
  anterior = micros();
}
