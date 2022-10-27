/*setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temp_bat").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/TEMP_BAT", true);
  xhttp.send();
}, 10000);*/

/*setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperatura").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/TEMPERATURA", true);
  xhttp.send();
}, 5000);*/

/*setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humedad").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/HUMEDAD", true);
  xhttp.send();
}, 10000);*/

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("velocidad").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/VEL", true);
  xhttp.send();
}, 5000);

/*setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("retroceso").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/RETROCESO", true);
  xhttp.send();
}, 2000);*/

/*setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("carga").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/CARGA", true);
  xhttp.send();
}, 10000);*/

