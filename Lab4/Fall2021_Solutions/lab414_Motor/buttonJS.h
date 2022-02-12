/*
 * HTML and Javascript code
 */
const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html>
  <body>
    <a href="/H">Backwards (1)</a> PWM03.<br>
    <a href="/L">Forwards (0) </a> PWM03.<br>
    <button type="button" onclick="hit()"> mybutton </button>
   <span id="buttonlabel">  </span>  <br>
   <br>
   <br>
   <input type="range" min="1" max="100" value="1"  id="slider">
   <span id="sliderlabel">  </span> <br>
  </body>

  <script>
    function hit() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "hit", true);
      xhttp.send();
    }

    setInterval(updateLabel, 1000);
    //updateLabel();
    function updateLabel() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("buttonlabel").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "LEDstate", true);
      xhttp.send();
    }

    slider.onchange = function() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("sliderlabel").innerHTML = this.responseText;
        }
      };
      var str = "slider?val=";
      var res = str.concat(this.value);
      xhttp.open("GET", res, true);
      xhttp.send();
    }

  </script>

</html>
)===";
