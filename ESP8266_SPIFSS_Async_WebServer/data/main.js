let envios = 0,
  trip = 0;
let lectura = [
  ["humedad", "80"],
  ["temperatura", "21"],
  ["velocidad", "12"],
  ["trip", "155"],
  ["odometro", "432"],
  ["temp_bat", "25"],
  ["carga", "80"],
];

window.onload = inicio;

function inicio() {
  document.getElementById("reinicio").onclick = reinicio;
}

function reinicio() {
  trip = 0;
  document.getElementById("trip").innerHTML = trip;
}

setInterval(function () {
  solicitud("/ACTUALIZAR");
}, 1500);

function solicitud(url) {
  var xhttp = new XMLHttpRequest();
  //console.log("Solicitud");
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      const respuesta = this.responseText;
      procesarDatos(respuesta);
      //document.getElementById(elementoId).innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", url, true);
  //xhttp.setRequestHeader("Cache-Control", "no-cache");
  xhttp.send();
  //console.log(envios);
  envios++;
  trip++;
  lectura[4][1] = envios;
  lectura[3][1] = trip;
  actualizar();
  giros();
}

function actualizar() {
  for (const datos in lectura) {
    const elementoId = lectura[datos][0];
    const variable = lectura[datos][1];
    //console.log(elementoId);
    //console.log(variable);

    document.getElementById(elementoId).innerHTML = variable;
  }
}

function procesarDatos(texto) {
  //TODO: Aca recibo el texto serial, y tengo que desdoblarlo para realizar todos los innerHTML
  /*datos_recibidos++;
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
    }*/
  console.log(texto);
  //const cant_datos = texto.slice();
}

//                        Variable     Id HTML       Etiqueta
// Temperatura ambiente   lectura[0] "temperatura"  "/TEMPERATURA"
// Humedad                lectura[1] "humedad"      "/HUMEDAD"
// Calcular Velocidad     lectura[2] "velocidad"    "/VEL"
// Trip                   lectura[3] "trip"         "/TRIP"
// Odometro               lectura[4] "odometro"     "/ODO"
// temperatura bateria    lectura[5] "temp_bat"     "/TEMP_BAT"
// Carga batería          lectura[6] "carga"        "/CARGA"

// Verificar si la API está disponible en el navegador
if ('wakeLock' in navigator) {
  // Solicitar el bloqueo de la pantalla
  navigator.wakeLock.request('screen')
    .then((wakeLock) => {
      console.log('Pantalla bloqueada con éxito');

      // Puedes liberar el bloqueo cuando ya no lo necesites
      // wakeLock.release();
    })
    .catch((error) => {
      console.error('Error al bloquear la pantalla:', error);
    });
} else {
  console.warn('La API Wake Lock no está disponible en este navegador.');
}
