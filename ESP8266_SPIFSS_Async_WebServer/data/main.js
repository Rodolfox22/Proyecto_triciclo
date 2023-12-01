let envios = 0;
const variables = [
  ["humedad", "/HUMEDAD"],
  ["velocidad", "/VEL"],
  ["trip", "/TRIP"],
  ["odometro", "/ODO"],
  ["temp_bat", "/TEMP_BAT"],
  ["carga", "/CARGA"],
];

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
  //console.log(envios);
  envios++;
}

function hacerSolicitud(elementoId, url) {
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
async function ejemplo() {
  try {
    const resultado = await hacerSolicitud(variables[1][0], variables[1][1]);
    console.log("Respuesta del servidor:", resultado);
  } catch (error) {
    console.error("Error:", error.message);
  }
}

/*setInterval(function () {
  // Temperatura ambiente   lectura[0]
  var xhttpTempAmb = new XMLHttpRequest();
  xhttpTempAmb.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperatura").innerHTML = this.responseText;
    }
  };
  xhttpTempAmb.open("GET", "/TEMPERATURA", true);
  xhttp.setRequestHeader("Cache-Control", "no-cache");
  xhttpTempAmb.send();

  // Humedad                lectura[1]
  var xhttpHumedad = new XMLHttpRequest();
  xhttpHumedad.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humedad").innerHTML = this.responseText;
    }
  };
  xhttpHumedad.open("GET", "/HUMEDAD", true);
  xhttp.setRequestHeader("Cache-Control", "no-cache");
  xhttpHumedad.send();
  
  // Temperatura bateria    lectura[5]
  var xhttpTemBat = new XMLHttpRequest();
  xhttpTemBat.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temp_bat").innerHTML = this.responseText;
    }
  };
  xhttpTemBat.open("GET", "/TEMP_BAT", true);
  //xhttp.setRequestHeader("Cache-Control", "no-cache");
  xhttpTemBat.send();
  
  // Carga batería          lectura[6]
  var xhttpCarga = new XMLHttpRequest();
  xhttpCarga.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("carga").innerHTML = this.responseText;
    }
  };
  xhttpCarga.open("GET", "/CARGA", true);
 // xhttp.setRequestHeader("Cache-Control", "no-cache");
  xhttpCarga.send();
  
}, 10000);*/

setInterval(function () {
  // Velocidad              lectura[2]
  /*var xhttpVelocidad = new XMLHttpRequest();
  xhttpVelocidad.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("velocidad").innerHTML = this.responseText;
      console.log(this.responseText);
    }
  };
  xhttpVelocidad.open("GET", "/VEL", true);
  xhttpVelocidad.setRequestHeader("Cache-Control", "no-cache");
  xhttpVelocidad.send();*/
  //console.log(envios);
  // Trip                   lectura[3]
  /*var xhttpTrip = new XMLHttpRequest();
  xhttpTrip.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("trip").innerHTML = this.responseText;
    }
  };
  xhttpTrip.open("GET", "/TRIP", true);
  //xhttp.setRequestHeader("Cache-Control", "no-cache");
  xhttpTrip.send();

  // Odometro               lectura[4]
  var xhttpOdometro = new XMLHttpRequest();
  xhttpOdometro.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("odometro").innerHTML = this.responseText;
    }
  };
  xhttpOdometro.open("GET", "/ODO", true);
  xhttp.setRequestHeader("Cache-Control", "no-cache");
  xhttpOdometro.send();



  //Retroceso
  var xhttpRetroceso = new XMLHttpRequest();
  xhttpRetroceso.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("retroceso").innerHTML = this.responseText;
  }
};
  xhttpRetroceso.open("GET", "/RETROCESO", true);
  xhttpRetroceso.send();*/
  /*for (const elemId in variables) {
    solicitud(variables[elemId][0], variables[elemId][1]);
  }*/
  //solicitud(variables[1][0], variables[1][1]);
  ejemplo();
}, 1500);

//                        Variable     Id HTML       Etiqueta
// Temperatura ambiente   lectura[0] "temperatura"  "/TEMPERATURA"
// Humedad                lectura[1] "humedad"      "/HUMEDAD"
// Calcular Velocidad     lectura[2] "velocidad"    "/VEL"
// Trip                   lectura[3] "trip"         "/TRIP"
// Odometro               lectura[4] "odometro"     "/ODO"
// temperatura bateria    lectura[5] "temp_bat"     "/TEMP_BAT"
// Carga batería          lectura[6] "carga"        "/CARGA"
