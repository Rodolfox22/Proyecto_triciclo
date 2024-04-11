//todo: revisar si voy a necesitar envios y timer_destello
let envios = 0,
  trip = 0,
  odometro = 0,
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

const TEXTOAPAGADO = "apagado";
const TEXTODERECHA = "derecha";
const TEXTOIZQUIERDA = "izquierda";
const TEXTOBALIZA = "baliza";
const GUINHO = [TEXTOAPAGADO, TEXTODERECHA, TEXTOIZQUIERDA, TEXTOBALIZA];
let cuentaGuinho = 0;
let giroDer, giroIz;
let envioJS = {};
let objetoWakeLock = null;
let comunicacionOk = false;

window.onload = inicio;

function inicio() {
  //pantallaEncendida();
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
  pruevaVisualizacion();
  actualizar();
  enviar();
}, 5000);

function solicitud(url) {
  var xhttp = new XMLHttpRequest();

  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      try {
        const respuesta = this.responseText;
        procesarDatos(respuesta);
        console.log(respuesta);
        comunicacionOk = true;
      } catch (error) {
        const respuestaError = this.responseText;
        console.log(respuestaError);
        console.error("Error al analizar JSON:", error);
      }
    } else {
      console.error("Error en la solicitud:", this.status);
    }
  };
  xhttp.open("GET", url, true);
  xhttp.send();
}

function enviar() {
  if (comunicacionOk) {
    setTimeout(() => {
      var xhr = new XMLHttpRequest();

      xhr.open("POST", "/ENVIAR", true);
      xhr.setRequestHeader("Content-Type", "application/json; charset=UTF-8");
      // Convertir el objeto JavaScript a una cadena JSON
      const jsonData = JSON.stringify(envioJS);

      // Configurar el controlador de eventos para la carga
      xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 201) {
          // La solicitud se completó correctamente

          console.log("Texto: " + this.responseText);
          console.log(this.response);
        } else {
          // Hubo un error en la solicitud
          console.error("Error en la solicitud:", this.status);
        }
      };

      // Enviar la solicitud con los datos JSON
      xhr.send(jsonData);
    }, 200);
    comunicacionOk = false;
  }
}

function pruevaVisualizacion() {
  if (!comunicacionOk) {
    envios++;
    odometro = envios;
    //Todo: borrar estas lineas una vez esté listo el programa
    const calculoTrip = envios - trip;
    lectura[buscarLectura("trip")][1] = calculoTrip;
    envioJS.trip = trip;
    lectura[buscarLectura("odometro")][1] = envios;
    envioJS.cicloJS = envios;
  }
}

//Leo todos los datos del arreglo para insertarlos en la pagina  por medio de un metodo innerHTML
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
  console.log("Leyendo datos del puerto serie");
  const datos = JSON.parse(texto);
  lectura[buscarLectura("humedad")][1] = datos.humedad;
  lectura[buscarLectura("temperatura")][1] = datos.temperatura;
  lectura[buscarLectura("velocidad")][1] = datos.velocidad;
  lectura[buscarLectura("trip")][1] = datos.trip;
  lectura[buscarLectura("odometro")][1] = datos.odometro;
  lectura[buscarLectura("temp_bat")][1] = datos.temp_bat;
  lectura[buscarLectura("carga")][1] = datos.carga;
  cuentaGuinho = GUINHO.indexOf(datos.guinho);

  for (let index = 0; index < lectura.length; index++) {
    const parametro = lectura[index][0];
    const valor = lectura[index][1];
    console.log(parametro + ": " + valor);
  }

  console.log(texto);
}

/* Funcionalidad dinamica*/

function reinicioTrip() {
  trip = odometro;
  lectura[buscarLectura("trip")][1] = trip;
  const calculoTrip = odometro - trip;
  console.log("Reinicio trip");
  document.getElementById("trip").innerHTML = calculoTrip;
}

//Logica para determinar si se encienden los guinhos
function guinhos(orden) {
  if (cuentaGuinho == GUINHO.indexOf(orden)) {
    orden = TEXTOAPAGADO;
  }
  cuentaGuinho = GUINHO.indexOf(orden);
  envioJS.guinho = orden;
  console.log(envioJS);
}

setInterval(function () {
  giros();
  timer_destello++;
}, 300);

//Indico cuando deben encenderse las flechas
function giros() {
  let figuraGuinho = TEXTOAPAGADO;
  if (timer_destello % 2 == 0) {
    figuraGuinho = GUINHO[cuentaGuinho];
  } else {
    figuraGuinho = TEXTOAPAGADO;
  }

  if (figuraGuinho == TEXTODERECHA) {
    giroDer.classList.add("arrow-on");
    giroIz.classList.remove("arrow-on");
    return;
  }

  if (figuraGuinho == TEXTOIZQUIERDA) {
    giroIz.classList.add("arrow-on");
    giroDer.classList.remove("arrow-on");
    return;
  }

  if (figuraGuinho == TEXTOBALIZA) {
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

/*
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
}*/
//                        Variable     Id HTML       Etiqueta
// Temperatura ambiente   lectura[0] "temperatura"  "/TEMPERATURA"
// Humedad                lectura[1] "humedad"      "/HUMEDAD"
// Calcular Velocidad     lectura[2] "velocidad"    "/VEL"
// Trip                   lectura[3] "trip"         "/TRIP"
// Odometro               lectura[4] "odometro"     "/ODO"
// temperatura bateria    lectura[5] "temp_bat"     "/TEMP_BAT"
// Carga batería          lectura[6] "carga"        "/CARGA"
