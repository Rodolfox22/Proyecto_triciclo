setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperatura_bat").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/TEMPERATURA_BAT", true);
  xhttp.send();
}, 10000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperatura").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/TEMPERATURA", true);
  xhttp.send();
}, 10000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humedad").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/HUMEDAD", true);
  xhttp.send();
}, 10000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("velocidad").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/VELOCIDAD", true);
  xhttp.send();
}, 500);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("retroceso").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/RETROCESO", true);
  xhttp.send();
}, 1000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("carga").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/CARGA", true);
  xhttp.send();
}, 1000);

