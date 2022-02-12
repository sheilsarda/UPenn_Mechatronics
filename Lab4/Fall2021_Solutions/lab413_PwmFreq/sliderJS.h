/*
 * HTML and Javascript code
 */
const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html>
  <body>
   <div style="background-color:black;color:white;padding:20px;">
    <!--PWM Slider-->
    <label for="freq_slider">Duty Cycle (0% - 100%):</label>
    <input type="range" min="0" max="100" value="50"  id="duty">
    <span id="outputlabel1">  </span> <br>
    <!--Frequency Slider-->
    <label for="freq_slider">Frequency (0.1 Hz - 10 Hz):</label>
    <input type="range" min="1" max="100" value="5"  id="freq">
    <span id="outputlabel2">  </span> <br>
   </div>
  
  <script>

  duty.onchange = function dutyFunction () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("outputlabel1").innerHTML = this.responseText;
      }
    };
    var str = "duty?val=";
    var res = str.concat(this.value);
    xhttp.open("GET", res, true);
    xhttp.send();
  }

    
  freq.onchange = function freqFunction() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("outputlabel2").innerHTML = this.responseText;
      }
    };
    var str = "freq?val=";
    var res = str.concat(this.value);
    xhttp.open("GET", res, true);
    xhttp.send();
  }


  </script>
  </body>

</html>
)===";
