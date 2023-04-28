setInterval(function () {
  // Temperatura ambiente   lectura[0]
  var xhttpTempAmb = new XMLHttpRequest();
  xhttpTempAmb.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperatura").innerHTML = this.responseText;
    }
  };
  xhttpTempAmb.open("GET", "/TEMPERATURA", true);
  xhttpTempAmb.send();

  // Humedad                lectura[1]
  var xhttpHumedad = new XMLHttpRequest();
  xhttpHumedad.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humedad").innerHTML = this.responseText;
    }
  };
  xhttpHumedad.open("GET", "/HUMEDAD", true);
  xhttpHumedad.send();

  // Velocidad              lectura[2]
  var xhttpVelocidad = new XMLHttpRequest();
  xhttpVelocidad.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("velocidad").innerHTML = this.responseText;
    }
  };
  xhttpVelocidad.open("GET", "/VEL", true);
  xhttpVelocidad.send();

  // Trip                   lectura[3]
  var xhttpTrip = new XMLHttpRequest();
  xhttpTrip.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("trip").innerHTML = this.responseText;
    }
  };
  xhttpTrip.open("GET", "/TRIP", true);
  xhttpTrip.send();

  // Odometro               lectura[4]
  var xhttpOdometro = new XMLHttpRequest();
  xhttpOdometro.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("odometro").innerHTML = this.responseText;
    }
  };
  xhttpOdometro.open("GET", "/ODO", true);
  xhttpOdometro.send();

  // Temperatura bateria    lectura[5]
  var xhttpTemBat = new XMLHttpRequest();
  xhttpTemBat.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temp_bat").innerHTML = this.responseText;
    }
  };
  xhttpTemBat.open("GET", "/TEMP_BAT", true);
  xhttpTemBat.send();

  // Carga batería          lectura[6]
  var xhttpCarga = new XMLHttpRequest();
  xhttpCarga.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("carga").innerHTML = this.responseText;
    }
  };
  xhttpCarga.open("GET", "/CARGA", true);
  xhttpCarga.send();

  /*//Retroceso
  var xhttpRetroceso = new XMLHttpRequest();
  xhttpRetroceso.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
    document.getElementById("retroceso").innerHTML = this.responseText;
  }
  };
  xhttpRetroceso.open("GET", "/RETROCESO", true);
  xhttpRetroceso.send();*/
}, 5000);

//                        Variable     Id HTML       Etiqueta
// Temperatura ambiente   lectura[0] "temperatura"  "/TEMPERATURA"
// Humedad                lectura[1] "humedad"      "/HUMEDAD"
// Calcular Velocidad     lectura[2] "velocidad"    "/VEL"
// Trip                   lectura[3] "trip"         "/TRIP"
// Odometro               lectura[4] "odometro"     "/ODO"
// temperatura bateria    lectura[5] "temp_bat"     "/TEMP_BAT"
// Carga batería          lectura[6] "carga"        "/CARGA"
