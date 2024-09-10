//todo: revisar si voy a necesitar envios y timer_destello
let envios = 0,
  trip = 0,
  odometro = 0,
  timer_destello = false;
let lectura = [
  ["humedad", "80"],
  ["temperatura", "21"],
  ["velocidad", "12"],
  ["trip", "155"],
  ["odometro", "432"],
  ["temp_bat", "50"],
  ["carga", "80"],
];
let datos = {};

const TEXTOAPAGADO = "apagado";
const TEXTODERECHA = "derecha";
const TEXTOIZQUIERDA = "izquierda";
const TEXTOBALIZA = "baliza";
const GUINHO = [TEXTOAPAGADO, TEXTODERECHA, TEXTOIZQUIERDA, TEXTOBALIZA];
const verAlarma = { lowBat: false, highTemp: false };
let numGuinho = 0;
let giroDer, giroIz, alarma, hTemp, lBat;
let envioJS = {};
let objetoWakeLock = null;
let comunicacionOk = false;

window.onload = inicio;

function inicio() {
  //pantallaEncendida();
  giroDer = document.getElementById("flecha-der");
  giroIz = document.getElementById("flecha-izq");
  alarma = document.getElementById("alarma-on");
  lBat = document.getElementById("lowbat");
  hTemp = document.getElementById("hightemp");
  document.getElementById("reinicio").onclick = reinicioTrip;
  giroDer.onclick = function () {
    guinhos(TEXTODERECHA);
    //verAlarma.highTemp = !verAlarma.highTemp;
    console.log(verAlarma);
  };
  giroIz.onclick = function () {
    guinhos(TEXTOIZQUIERDA);
    //verAlarma.lowBat = !verAlarma.lowBat;
    console.log(verAlarma);
  };
  document.getElementById("velocidad").onclick = function () {
    guinhos(TEXTOBALIZA);
  };
  
  //Todo: crear baliza implementando una pagina nueva que quede sobre la pantalla indicando las alarmas disponibles, implementar un boton para desactivar alarmas, crear un instructivo para decir que hacer con la alarma

  alarma.onclick = function () {
    guinhos(TEXTOBALIZA);
  };

  document.getElementById("alarma-off").onclick = function () {
    guinhos(TEXTOBALIZA);
  };
  document.getElementById("carga").onclick = function () {
    verAlarma.highTemp = !verAlarma.highTemp;
    console.log("Alarma temperatura");
  };
  document.getElementById("temp_bat").onclick = function () {
    verAlarma.lowBat = !verAlarma.lowBat;
    console.log("Alarma bateria");
  };
}

//Tareas de sincronizacion
setInterval(function () {
  solicitud();
  pruevaVisualizacion();
  actualizar();
  actualizaHora();
}, 5000);

function solicitud() {
  var xhr = new XMLHttpRequest();

  xhr.open("POST", "/ENVIAR", true);
  xhr.setRequestHeader(
    "Content-Type",
    "application/x-www-form-urlencoded; charset=UTF-8"
  );
  // Convertir el objeto JavaScript a una cadena JSON
  const jsonData = JSON.stringify(envioJS);

  const urlEncodedData = "data=" + encodeURIComponent(jsonData);

  // Configurar el controlador de eventos para la carga
  xhr.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      // La solicitud se completó correctamente
      const respuesta = this.response;
      console.log("Respuesta del servidor: " + respuesta);
      console.log(respuesta);
      
      lectura[buscarLectura("velocidad")][1] = respuesta;

      //Procesa los datos Json
      //procesarDatos(respuesta);
    
    } else {
      // Hubo un error en la solicitud
      console.error("Error en la solicitud:", this.status);
    }
  };

  // Enviar la solicitud con los datos JSON
  xhr.send(urlEncodedData);
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
  datos = {};
  datos = JSON.parse(texto);
  lectura[buscarLectura("humedad")][1] = datos.humedad;
  lectura[buscarLectura("temperatura")][1] = datos.temperatura;
  lectura[buscarLectura("velocidad")][1] = datos.velocidad;
  lectura[buscarLectura("trip")][1] = datos.trip;
  lectura[buscarLectura("odometro")][1] = datos.odometro;
  lectura[buscarLectura("temp_bat")][1] = datos.temp_bat;
  lectura[buscarLectura("carga")][1] = datos.carga;
  numGuinho = GUINHO.indexOf(datos.guinho);

  for (let index = 0; index < lectura.length; index++) {
    const parametro = lectura[index][0];
    const valor = lectura[index][1];
    console.log(parametro + ": " + valor);
  }

  console.log(datos);
}

/* Funcionalidad dinamica*/
function reinicioTrip() {
  trip = odometro;
  lectura[buscarLectura("trip")][1] = trip;
  const calculoTrip = odometro - trip;
  console.log("Reinicio trip");
  document.getElementById("trip").innerHTML = calculoTrip;
}

//Logica para determinar si se encienden los guiños
function guinhos(orden) {
  if (numGuinho == GUINHO.indexOf(orden)) {
    orden = TEXTOAPAGADO;
  }
  numGuinho = GUINHO.indexOf(orden);
  envioJS.guinho = orden;
  console.log(envioJS);
}

setInterval(function () {
  giros();
  activarAlarma();
  timer_destello = !timer_destello;
}, 300);

//Indico cuando deben encenderse las flechas
function giros() {
  const accion = timer_destello ? GUINHO[numGuinho] : TEXTOAPAGADO;

  // Removemos las clases de ambas flechas
  giroDer.classList.remove("arrow-on");
  giroIz.classList.remove("arrow-on");

  // Agregamos las clases según la accion
  switch (accion) {
    case TEXTODERECHA:
      giroDer.classList.add("arrow-on");
      break;
    case TEXTOIZQUIERDA:
      giroIz.classList.add("arrow-on");
      break;
    case TEXTOBALIZA:
      giroDer.classList.add("arrow-on");
      giroIz.classList.add("arrow-on");
      break;
    default:
      // En caso de TEXTOAPAGADO u otra accion no reconocida, no se agregan clases
      break;
  }
}

function activarAlarma() {
  const alarma_temp = parseFloat(lectura[buscarLectura("temp_bat")][1]);
  const alarma_bat = parseFloat(lectura[buscarLectura("carga")][1]);
  if (alarma_temp >= 45) {
    verAlarma.highTemp = true;
  } else {
    verAlarma.highTemp = false;
  }

  if (alarma_bat <= 20) {
    verAlarma.lowBat = true;
  } else {
    verAlarma.lowBat = false;
  }

  if (!timer_destello) {
    lBat.style.visibility = "hidden";
    hTemp.style.visibility = "hidden";
    //alarma.style.display = null;
    return;
  }

  if (verAlarma.highTemp) {
    hTemp.style.visibility = "visible";
    //alarma.style.display = "block";
  }

  if (verAlarma.lowBat) {
    lBat.style.visibility = "visible";
    //alarma.style.display = "block";
  }
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

function actualizaHora() {
  const fechaHoraActual = new Date();

  const anho = fechaHoraActual.getUTCFullYear();
  const mes = ("0" + (fechaHoraActual.getUTCMonth() + 1)).slice(-2);
  const dia = ("0" + fechaHoraActual.getUTCDate()).slice(-2);
  const horas = ("0" + fechaHoraActual.getHours()).slice(-2);
  const minutos = ("0" + fechaHoraActual.getMinutes()).slice(-2);
  const segundos = ("0" + fechaHoraActual.getSeconds()).slice(-2);

  const horaFormateada = `${horas}:${minutos}`;
  document.getElementById("hora-actual").textContent = horaFormateada;

  //const tagHora = `${anho}-${mes}-${dia}_${horas}:${minutos}`;
  const tagHora = `${anho}-${mes}-${dia}_${horas}:${minutos}:${segundos}`;
  //console.log(tagHora);
  envioJS.hora = tagHora;
  return tagHora;
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
