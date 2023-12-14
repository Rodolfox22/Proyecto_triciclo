//todo: revisar si voy a necesitar envios y timer_destello
let envios = 0,
  trip = 0,
  timer_destello = 0;
let lectura = [
  ["humedad", "80"],
  ["temperatura", "21"],
  ["velocidad", "12"],
  ["trip", "155"],
  ["odometro", "432"],
  ["temp_bat", "25"],
  ["carga", "80"],
];

let cuentaGuinho = 0;
const GUINHO = ["", "derecha", "izquierda", "baliza"];
const TEXTODERECHA = "derecha";
const TEXTOIZQUIERDA = "izquierda";
const TEXTOBALIZA = "baliza";
let giroDer, giroIz;
let envioJS = {};
let objetoWakeLock = null;

window.onload = inicio;

function inicio() {
  pantallaEncendida();
  giroDer = document.getElementById("flecha-der");
  giroIz = document.getElementById("flecha-izq");
  document.getElementById("reinicio").onclick = reinicioTrip;
  giroDer.onclick = function () {
    guinhos(TEXTODERECHA);
  };
  giroIz.onclick = function () {
    guinhos(TEXTOIZQUIERDA);
  };
  document.getElementById("velocidad").onclick = function () {
    guinhos(TEXTOBALIZA);
  };
}

setInterval(function () {
  solicitud("/ACTUALIZAR");
  datosOdometro();
  actualizar();
  enviar();
}, 1000);

function solicitud(url) {
  var xhttp = new XMLHttpRequest();

  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      const respuesta = this.responseText;
      procesarDatos(respuesta);
    } /*else {
      // Hubo un error en la solicitud
      console.error("Error en la solicitud:", this.status);
    }*/
  };
  xhttp.open("GET", url, true);
  xhttp.send();
}

function enviar() {
  setTimeout(() => {
    var xhr = new XMLHttpRequest();

    xhr.open("POST", "/ENVIAR", true);
    //xhr.setRequestHeader("Content-Type", "application/json");
    // Convertir el objeto JavaScript a una cadena JSON
    const jsonData = JSON.stringify(envioJS);

    // Configurar el controlador de eventos para la carga
    xhr.onload = function () {
      if (this.status == 200) {
        // La solicitud se completó correctamente
        console.log(this.responseText);
      } /*else {
        // Hubo un error en la solicitud
        console.error("Error en la solicitud:", this.status);
      }*/
    };

    // Enviar la solicitud con los datos JSON
    xhr.send(jsonData);
  }, 200);
}

function datosOdometro() {
  envios++;
  //Todo: borrar estas lineas una vez esté listo el programa
  const calculoTrip = envios - trip;
  lectura[buscarLectura("odometro")][1] = envios;
  lectura[buscarLectura("trip")][1] = calculoTrip;
  envioJS.trip = trip;
  envioJS.cicloJS = envios;
}

function actualizar() {
  //console.log("Actualizando...");
  for (const datos in lectura) {
    const elementoId = lectura[datos][0];
    const variable = lectura[datos][1];

    document.getElementById(elementoId).innerHTML = variable;
  }
}

function procesarDatos(texto) {
  //TODO: Aca recibo el texto serial, y tengo que desdoblarlo para realizar todos los innerHTML
  const datos = JSON.parse(texto);
  lectura[buscarLectura("humedad")][1] = datos.humedad;
  lectura[buscarLectura("temperatura")][1] = datos.temperatura;
  lectura[buscarLectura("velocidad")][1] = datos.velocidad;
  lectura[buscarLectura("trip")][1] = datos.trip;
  lectura[buscarLectura("odometro")][1] = datos.odometro;
  lectura[buscarLectura("temp_bat")][1] = datos.temp_bat;
  lectura[buscarLectura("carga")][1] = datos.carga;
  cuentaGuinho = GUINHO.indexOf(datos.guinho);

  console.log(texto);
}

/* Funcionalidad dinamica*/

function reinicioTrip() {
  trip = envios;
  const calculoTrip = envios - trip;
  console.log("Reinicio trip");
  document.getElementById("trip").innerHTML = calculoTrip;
}

function guinhos(orden) {
  if (cuentaGuinho == GUINHO.indexOf(orden)) {
    orden = "";
  }
  cuentaGuinho = GUINHO.indexOf(orden);
  envioJS.guinho = orden;
  console.log(envioJS);
}

setInterval(function () {
  giros();
  timer_destello++;
}, 300);

function giros() {
  let guinho = "";
  if (timer_destello % 2 == 0) {
    guinho = GUINHO[cuentaGuinho];
  } else {
    guinho = "";
  }

  if (guinho == TEXTODERECHA) {
    giroDer.classList.add("arrow-on");
    giroIz.classList.remove("arrow-on");
    return;
  }

  if (guinho == TEXTOIZQUIERDA) {
    giroIz.classList.add("arrow-on");
    giroDer.classList.remove("arrow-on");
    return;
  }

  if (guinho == TEXTOBALIZA) {
    giroDer.classList.add("arrow-on");
    giroIz.classList.add("arrow-on");
    return;
  }

  giroDer.classList.remove("arrow-on");
  giroIz.classList.remove("arrow-on");
}

/*Extras*/

function buscarLectura(valor) {
  let indice = -1;
  for (let i = 0; i < lectura.length; i++) {
    if (lectura[i][0] === valor) {
      indice = i;
      break;
    }
  }
  return indice;
}

// Verificar si la API está disponible en el navegador
async function pantallaEncendida() {
  if ("wakeLock" in navigator) {
    // Solicitar el bloqueo de la pantalla
    try {
      objetoWakeLock = await navigator.wakeLock.request("screen");
      console.log("Pantalla bloqueada con éxito");
      //console.log(objetoWakeLock);
      // Puedes liberar el bloqueo cuando ya no lo necesites
      // wakeLock.release();
    } catch (error) {
      console.error("Error al bloquear la pantalla:", error);
    }
  } else {
    console.warn("La API Wake Lock no está disponible en este navegador.");
    alert(
      "No se encuentra disponible el servicio para mantener la pantalla encendida."
    );
  }
}
//                        Variable     Id HTML       Etiqueta
// Temperatura ambiente   lectura[0] "temperatura"  "/TEMPERATURA"
// Humedad                lectura[1] "humedad"      "/HUMEDAD"
// Calcular Velocidad     lectura[2] "velocidad"    "/VEL"
// Trip                   lectura[3] "trip"         "/TRIP"
// Odometro               lectura[4] "odometro"     "/ODO"
// temperatura bateria    lectura[5] "temp_bat"     "/TEMP_BAT"
// Carga batería          lectura[6] "carga"        "/CARGA"
