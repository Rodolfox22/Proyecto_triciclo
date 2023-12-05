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
let direccion = "derecha";

window.onload = inicio;

function inicio() {
  document.getElementById("reinicio").onclick = reinicio;
}

function reinicio() {
  trip = 0;
  document.getElementById("trip").innerHTML = trip;
  if (direccion == "derecha") {
    direccion = "izquierda";
  } else if (direccion == "izquierda") {
    direccion = "baliza";
  } else if (direccion == "baliza") {
    direccion = "";
  } else {
    direccion = "derecha";
  }
  console.log(direccion);
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

function giros() {
  const giroDer = document.getElementById("flecha-der");
  const giroIz = document.getElementById("flecha-izq");

  if ((direccion = "derecha")) {
    destellador(giroDer);
    console.log("Derecha");
  }

  if ((direccion = "izquierda")) {
    destellador(giroIz);
  }

  if ((direccion = "baliza")) {
    destellador(giroDer, giroIz);
  }
}

function destellador(luz, luz1=null) {
  function destellar() {
    luz.classList.add("arrow-on");
    luz1.classList.add("arrow-on");
    //console.log("Encendido");

    setTimeout(() => {
      luz.classList.remove("arrow-on");
      luz1.classList.remove("arrow-on");
      //console.log(luz);
      // Llamar recursivamente después de un tiempo
      setTimeout(destellar, 400);
    }, 400);
  }
  destellar();
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
