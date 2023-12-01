let envios = 0;
const lectura = [
  ["humedad", "/HUMEDAD"],
  ["velocidad", "/VEL"],
  ["trip", "/TRIP"],
  ["odometro", "/ODO"],
  ["temp_bat", "/TEMP_BAT"],
  ["carga", "/CARGA"],
];

setInterval(function () {
  /*for (const elemId in lectura) {
    solicitud(lectura[elemId][0], lectura[elemId][1]);
  }*/
  solicitud(lectura[1][0], lectura[1][1]);
  //ejemplo(lectura[1][0], lectura[1][1]);
}, 1500);

function solicitud(elementoId, url) {
  console.log(elementoId);
  console.log(url);
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById(elementoId).innerHTML = this.responseText;
      console.log(this.responseText);
    }
  };
  xhttp.open("GET", url, true);
  xhttp.setRequestHeader("Cache-Control", "no-cache");
  xhttp.send();
  console.log(envios);
  envios++;
}

function solicitudAsincrona(elementoId, url) {
  return new Promise((resolve, reject) => {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        resolve(
          (document.getElementById(elementoId).innerHTML = this.responseText)
        );
        console.log(this.responseText);
      } else {
        reject(new Error("Error en la solicitud al servidor"));
      }
    };
    xhttp.open("GET", url, true);
    xhttp.send();
  });
}

// Ejemplo de uso con await
async function ejemplo(elemento, direccion) {
  try {
    const resultado = await solicitudAsincrona(elemento, direccion);
    console.log("Respuesta del servidor:", resultado);
  } catch (error) {
    console.error("Error:", error.message);
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
