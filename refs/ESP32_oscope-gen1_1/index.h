const char body[] PROGMEM = R"===( <!DOCTYPE html>
<html>
<head>
<title> OscilloSorta </title>
<style> 

.button {  cursor: pointer;  background-color: #fff; border: none;  border-radius: 15px;  box-shadow: 4px 2px #999;}
.button:hover { background-color: #ede}
.button:active {background-color: #a9a;  box-shadow: 1px 0px #666;  transform: translatex(2px);}
  h3 { margin: 0px;}
  hr { border: none; height: 2px; background-color: #111; }
  th, td {  border-radius: 9px; text-align: center;}
  nav1 { float: left; border-radius: 15px; width: 20%; border: 1px solid gray; background: #ddd; padding: 5px; text-align: center;}
  nav2 { float: left; border-radius: 15px; width: 20%; border: 1px solid gray; padding: 5px; text-align: center;}
  nav3 { float: left; border-radius: 15px; width: 20%; border: 1px solid gray; background: #ddd; padding: 5px; text-align: left;}
  article { float: left; border-radius: 15px;  padding: 10px;  width: 70%; background-color: #f1f1f1;}
  footer { float: left; border-radius: 25px; padding: 10px;  width: auto;  background: #73AD21;  text-align: center;}
  section {  width: 730px;}
  @media (max-width: 700px) {
    nav, article {   width: 100%;  height: auto;
    }
  }
</style> 
</head>

<body>
<section>
<article>
  <canvas id="myCanvas" width="500" height="400" style="border:3px solid #d3d3d3; background-color: #fff">
  Your browser does not support the HTML5 canvas tag.
  </canvas> <br>
  <p.a style="font-size: 25px;"> OscilloSorta</p.a> v1.1 MEAM510 UPenn &emsp;
     <span id='frame'> </span> <br>
<table style= "width: 100%;"> <tr> 
 <th> Resolution = <span id='resolution'> 250 </span> </th> 
  <th> PGA Gain = <span id='MCPgain'> 1 </span> (assuming MCP6S21) </th></tr>
<tr>
  <th> <input type="range" min="1" max="20" value="5" id="resslider"> &nbsp; </th><th>
  <input type="range" min="0" max="7" value="0" id="MCPslider"></th>
    <th>  <!-- <button type="button" class="button" onclick="autoset()"> &nbsp; AUTOSET-hack </button> -->
</th></tr>
</table>
</article>

<nav1>
  <button type="button" class="button" style="background-color:lime;" id='runstop' onclick="runStop()"> Run/Stop </button> &emsp;
  <button type="button" class="button" style="background-color:pink;" id='trigB' onclick="trigmode()"> Single </button> <br>
<h3 > Horizontal </h3> 
<table >
  <tr>
    <th style="border-radius: 5px;   background-color: #555; color: lightgreen ; ">  <span id='speed'> 5000 </span><br> uS/div </th>
    <td>
    <button type="button" class="button" onclick="speedup()"> &#10133;   </button> <br>
    <button type="button" class="button" onclick="speeddown()"> &#10134;   </button> <br>
    </td>
  </tr>
  <tr> <th colspan="2"> 
    <input type="range" min="0" max="500" value="125"  id="hoffslider"> </th>
  </tr>
</table>
</nav1>

<nav2 id="nav2">
<h3 id="vert"> Vertical </h3>
<table >
  <tr>
    <th style="border-radius: 5px;   background-color: #555; color: lightgreen ; "> <span id='volt'> 1000 </span><br>mV/div </th>
    <td><button type="button" class="button" onclick="voltup()">  &#10133; </button><br>
     <button type="button" class="button" onclick="voltdown()"> &#10134; </button><br>
    </td>
  </tr><tr>
    <th colspan="2"> <input type="range" min="1" max="400" value="200"  id="gndslider"></th> 
  </tr><tr>
    <th colspan="2"> <span id='acc'> &#127996; </span> <button type="button" class="button" onclick="accb()"> AC Coupling </button></th>
  </tr>
</table>
<hr>
<h3 id = "trig"> Trigger </h3> 
<table >
  <tr>
    <th colspan="2"> <input type="range" min="1" max="400" value="200"  id="triggerslider"></th>
  </tr><tr>
    <th > <span id='edge'> &#10548; </span> <button type="button" class="button" onclick="edgeCB()">  Edge </button> </th>
    <th > <span id='tedge'>  1/5 </span> <button type="button" class="button" onclick="tedgeCB()">  &#10133;  </button></th>
  </tr>
</table> 
<hr>
  <button type="button" class="button" style="background-color:pink;" id='chOn' onclick="toggleChOn()"> On/Off </button>  &emsp;
  <button type="button" class="button" style="background-color:pink;" id='draw0B' onclick="toggleDrawOs()"> Data Os </button> <br>
</nav2>

<nav3>
<H3 align="center"> Select Channel </H3> 
<span id='ch1'> &#x2705; </span> <button type="button" class="button" style="color:chocolate;" onclick="ch1RB()"> Ch 1 </button>  P36 Analog <br>
<span id='ch2'> &#x2705; </span> <button type="button" class="button" style="color:blue;" onclick="ch2RB()"> Ch 2 </button>  P39 Logic 
</nav3>

<footer>
<h1 style="margin:1px;"> Function Generator</h1>
output on GPIO25
<table style="border-radius: 9px; background-color: #eee;">
  <tr>
    <td style="font-size: 35px; width:150px; border-radius: 10px;   background-color: #555; color: lightgreen ; "> 
     <span id='freq' > 0 </span>Hz</td>
    <td><label> <input id="x1RB" type="radio" name="xFreq"><br>  x1 </label> </td>
    <td><label> <input id="x10RB" type="radio" name="xFreq"><br>  x10 </label></td>
    <td><label> <input id="x100RB" type="radio"  name="xFreq"><br> x100 </label></td>
    <td><label> <input id="x1kRB" type="radio"  name="xFreq"><br>  x1k </label></td>
    <td><label> <input id="x10kRB" type="radio"  name="xFreq"><br>  x10k </label></td>
    <td style="border: none; background-color: #eee;"> &nbsp; </td>
    <td style="float: right;"><label> <input id="offRB" type="radio"  name="waveform">&nbsp; &nbsp;<br>  off &nbsp; </label></td>
    <td><label> <input id="squareRB" type="radio"  name="waveform"><br>  square </label><br></td>
    <td><label> <input id="sineRB" type="radio"  name="waveform"><br>  sine </label><br></td>
 </tr>
 <tr><td style="border: none; background-color: #eee;">&nbsp;<br></td> </tr>
 <tr><td > Frequency <br><input type="range" min="2" max="100" value="50"  id="FGfreqslider"></td>
   <td style="border: none;" colspan="3">  <span id="duty"> Duty</span> <br>  <input type="range" min="0" max="100" value="50" id="dutyslider"></td>  
   <td  colspan="3">  <span id="FGoffset"> Offset </span> <br>  <input type="range" min="-127" max="127" value="0"  id="FGoffsetslider"></td>
   <td  colspan="1">  <span id="FGscale"> Amplitude </span> <br>  <input type="range" min="0" max="3" value="1" style="width: 100px;" id="FGscaleslider"> </td>
   <td  colspan="3">  <span id="FGprecision"> Sine Precision </span> <br>  <input type="range" min="1" max="8" value="8" style="width: 100px;" id="FGprecisionslider"> </td>
 </tr>
</table> 
</footer>

<button onclick="localWiFi()">Switch to local WiFi router</button>

</section>

<script>
// OScope Elements
var c = document.getElementById("myCanvas");
var ctx = c.getContext("2d");
var gndslider = document.getElementById("gndslider");
var frame = document.getElementById("frame");
var trig = document.getElementById("trig");
// Function Generator Elements
var FGfreqslider = document.getElementById("FGfreqslider");
var duty = document.getElementById("duty");
var FGoffset = document.getElementById("FGoffset");
var FGscale = document.getElementById("FGscale");
var FGprecisionslider = document.getElementById("FGprecisionslider");

// Oscope interface states
var trigger = [240, 240];
var minADC = 142;
var maxADC = 3170
var drawOs = [0, 0];
var mvPerDiv = [1000, 1000];
var omvPerDiv = [1000, 1000];
var usPerDiv = 1000;
var ground = [200, 20];
var risingEdge = [1,1];
var x10sq = 1;
var setandtriggered = 0;
var triggered;
var chOn = [0,0];
var chSelected = 1;
var trigCh = 1;
var trigEdges = [0,0];
var triggerMode = 0;
var avgV = [0, 0];
var acCoupling = [false,false];
var mcpgains = [1,2,4,5,8,10,16,32];
var mcpgain = 1;

const chColor = ["chocolate","blue"];

const aSize = 1024
var z = new Array(3 * aSize); // input buffer
var v = new Array(aSize);
voffset = 0; // chan 1
var u = new Array(aSize);
uoffset = 0; // chan 2
var w = new Array(aSize);
woffset = 0; // time
var yoff = 125; // starting horizontal position (hoffset)
var wend = 0;
var i;
var risingedges;
// Function generator state
var FGmode = "square";
var sineoffset = 0;

// INITIAL SETUP 
ch1RB();
toggleChOn();
speeddown();
document.getElementById("ch1").checked = true;
document.getElementById("offRB").checked = true;
checkData();
getADCchars();

ctx.font = "14px Arial";
ctx.translate(-0.5, -0.5);
freq.innerHTML = FGfreqslider.value;
var autosetcount = 0; // static autoset vars
var maxV = 0,  minV = 400;

function localWiFi() {
  let text;
  let ssid = prompt("Please enter your local router ssid", "type SSID here");
  let pass = prompt("Please enter SSID passwd (Transmittal is not secure). After hitting okay, switch to local router and reload.", "type password here");
  let ssidpass = ssid + "+" + pass;
  if (ssidpass != null && ssidpass != "") {
    var str = "localWiFi?val=";
    var res = str.concat(ssidpass);
    sendXhttp(res);
  }
}

function measureMinMax() {
  var x;
  maxV = 0;
  minV = 400;
  for (i = 1; i < wend; i++) {
    x = v[i];
    if (x > maxV) maxV = x;
    if (x < minV) minV = x;
  }
}

function autoset() {
  var mytimeout;
  if (autosetcount == 0) {
    ch1RB();
    if (usPerDiv > 5000) {
      speeddown(); // hack to reduce antialiasing 
      speeddown();
      speeddown();
    }
    ground[chSelected-1] = 200;
    updateGround();
  } else if (autosetcount < 6 && (autosetcount % 2 == 1)) {
    measureMinMax();
    ground[chSelected-1] += 200 - minV;
    updateGround();
  } else if (autosetcount < 6) {
    measureMinMax();
    if (maxV > 400) { voltup(); }
    if (maxV < 250) { voltdown(); }
  } else {
    trigger[0] = 240;
    measureFreq();
    if (risingedges > 8) speeddown();
    if (risingedges < 4) speedup();
  }
  if (maxV < 400 && risingedges > 3 && risingedges < 9) setandtriggered = 1;
  else setandtriggered = 0;
  mytimeout = setTimeout(autoset, 600);
  if ((setandtriggered && autosetcount > 3) || autosetcount > 8) {
    clearTimeout(mytimeout);
    autosetcount = 0;
  }
  autosetcount++;
}

function vPush(item) {
  v[voffset++] = item;
  voffset %= aSize;
}

function uPush(item) {
  u[uoffset++] = item;
  uoffset %= aSize;
}

function wPush(item) {
  w[woffset++] = item;
  woffset %= aSize;
}

function measureFreq() {
  var oldx = 0,  firstw = 0,  lastw = 0; risingedges = 0; firstw = 0;
  if (chSelected == 1) {
    let trigV = Math.floor(trigger[0]);
    oldx = v[0];
    for (i = 1; i < wend; i++) {
      if (oldx > trigV && v[i] <= trigV) {
        risingedges++;
        if (firstw < 1) firstw = w[i];
        else lastw = w[i];
      }
      oldx = v[i];
    }
    return 1000000 / (usPerDiv * (lastw - firstw) / (risingedges - 1) / 50);
  }
  if (chSelected == 2) {
    let trigV = Math.floor(trigger[1]);
    oldx = u[0];
    for (i = 1; i < wend; i++) {
      if (oldx > trigV && u[i] <= trigV) {
        risingedges++;
        if (firstw == 0) firstw = w[i];
        else lastw = w[i];
      }
      oldx = u[i];
    }
    return 1000000 / (usPerDiv * (lastw - firstw) / (risingedges-1) / 50);
  }
  return 0;
}

function sendXhttp(res){
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", res, true);
  xhttp.send();
}

///////////// Function Generator ///////////////
function waveOff() {
  sendXhttp("waveOff");
  FGmode = "off";
  duty.style.color = "lightgrey";
  FGoffset.style.color = "lightgrey";
  FGscale.style.color = "lightgrey";
  document.getElementById("FGprecision").style.color =  "lightgrey";
  duty.style.color = "lightgrey";
}

function setsine() {
  sendXhttp("sineOn");
  FGmode = "sine";
  duty.style.color = "lightgrey";
  FGoffset.style.color = "black";
  FGscale.style.color = "black";
  document.getElementById("FGprecision").style.color =  "black";
}

function setsquare() {
  sendXhttp( "squareOn");
  FGmode = "square";
  FGoffset.style.color = "lightgrey";
  FGscale.style.color = "lightgrey";
  document.getElementById("FGprecision").style.color =  "lightgrey";
  duty.style.color = "black";
}

document.getElementById("dutyslider").oninput = function() {
  if (FGmode == "square") {
    duty.innerHTML = this.value;
  }
}
document.getElementById("dutyslider").onchange = function() {
  if (FGmode == "square") {
    var str = "duty?val=";
    var res = str.concat(this.value);
    sendXhttp(res);
  }
}

function findSineFreq( f_target) {
  var step, divi,f, delta, delta_min=10000, step_target, divi_target=0;
    for ( step=1;step<2000; step++) {
      for ( divi=0; divi<document.getElementById("FGprecisionslider").value; divi++) {
        f=125.6*step/(divi+1);
        delta= Math.abs((f_target-f)); 
        if (delta  < delta_min) {delta_min=delta; step_target=step; divi_target=divi;}
      }
    }
    return 8500000 * step_target  / (1 + divi_target) / 65536;
}

function sendFGfreq(val) {
  var str = "testfq?val=";
  var nval = val*x10sq;
  let res = str.concat(nval);
  sendXhttp(res);

  if (FGmode=="sine") 
    nval = findSineFreq(nval);
  if (nval>=1000) {
    var sval = nval/1000;
    freq.innerHTML = String(sval.toPrecision(3))+"k";
  }  else
    freq.innerHTML = nval.toPrecision(3);
}

function sendFGoffset(val) {
  var str = "sineoffset?val=";
  var res = str.concat(val);
  sendXhttp(res);
  FGoffset.innerHTML = "Offset " + String(val);
}

function sendFGprecision(val) {
  var str = "sineprecision?val=";
  var res = str.concat(val);
  sendXhttp(res);
  sendFGfreq(FGfreqslider.value);
}

function sendFGscale(val) {
  var str = "sinescale?val=";
  var res = str.concat(val);
  sendXhttp(res);
  FGscale.innerHTML = "Ampl. x" + String(1 / (1 << val));
}

FGfreqslider.oninput = function() {
  var nval = this.value*x10sq;
  if (FGmode=="sine") 
    nval = findSineFreq(nval);
  if (nval>1000) {
    var val = nval/1000;
    freq.innerHTML = String(val.toPrecision(3))+"k";
  }  else
    freq.innerHTML = nval.toPrecision(3);
}
FGfreqslider.onchange = function() {
  sendFGfreq(this.value);
}
document.getElementById("FGoffsetslider").oninput = function() {
  FGoffset.innerHTML = this.value;
  sineoffset = this.value;
}
document.getElementById("FGoffsetslider").onchange = function() {
  sendFGoffset(sineoffset);
}
document.getElementById("FGscaleslider").oninput = function() {
  FGscale.innerHTML = "Ampl. x" + String(1 / (1 << this.value));
}
document.getElementById("FGscaleslider").onchange = function() {
  sendFGscale(this.value);
}
document.getElementById("FGprecisionslider").onchange = function() {
  sendFGprecision(this.value);
}

document.getElementById("x1RB").addEventListener('change', () => {
  x10sq = 1;
  sendFGfreq(FGfreqslider.value);
});
document.getElementById("x10RB").addEventListener('change', () => {
  x10sq = 10;
  sendFGfreq(FGfreqslider.value);
});
document.getElementById("x100RB").addEventListener('change', () => {
  x10sq = 100;
  sendFGfreq(FGfreqslider.value);
});
document.getElementById("x1kRB").addEventListener('change', () => {
  x10sq = 1000;
  sendFGfreq(FGfreqslider.value);
});
document.getElementById("x10kRB").addEventListener('change', () => {
  x10sq = 10000;
  sendFGfreq(FGfreqslider.value);
});
document.getElementById("offRB").addEventListener('change', () => {
  waveOff();
});
document.getElementById("sineRB").addEventListener('change', () => {
  setsine();
});
document.getElementById("squareRB").addEventListener('change', () => {
  setsquare();
});


////////////// OSCOPE FUNCTIONS //////////

function getADCchars() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      let val =  this.responseText.split(",");
      minADC = val[0];
      maxADC = val[1];
    }
  }
  xhttp.open("GET", "minmaxADC", true);
  xhttp.send();
}

function runStop() {
  if (triggerMode==2) { // stop
    document.getElementById("runstop").style = "background-color:lime";
    triggerMode=0; // run
  } else {
    triggerMode = 2;
    document.getElementById("hoffslider").min = w[0];
    document.getElementById("runstop").style = "background-color:pink";
  }
  document.getElementById("trigB").style = "background-color:pink";
}

function ch1RB() {
  chSelected = 1;
  if (trigCh == 1) {
    trig.innerHTML = "Trigger on Ch1";
    trig.style.color = "black";
  } else {
    trig.innerHTML = "Set Trigger Ch1";
    trig.style.color = "grey";
  }
  document.getElementById("vert").innerHTML = "Vertical Ch1";
  document.getElementById("vert").style.color = "sienna";
  document.getElementById("nav2").style.backgroundColor = "BlanchedAlmond";
  updateChOn();
  updateACC();
  updateEdge();
  updateVolt();
  sendXhttp("ch?val=0");;
}

function ch2RB() {
  chSelected = 2;    
  if (trigCh == 2) {
    trig.innerHTML = "Trigger on Ch2";
    trig.style.color = "black";
  } else {
    trig.innerHTML = "Set Trigger Ch2";
    trig.style.color = "grey";
  }
  document.getElementById("vert").innerHTML = "Vertical Ch2";
  document.getElementById("vert").style.color = "darkblue";
  document.getElementById("nav2").style.backgroundColor = "lightblue";
  updateChOn();
  updateACC();
  updateEdge();
  updateVolt();
  sendXhttp("ch?val=1");;
}

function trigmode() {
  if (triggerMode==0) {
    triggerMode = 1;   // run once
   // document.getElementById("trigB").innerHTML = "single";
    document.getElementById("trigB").style = "background-color:lime";
    document.getElementById("runstop").style = "background-color:lime";
  } else {
    triggerMode = 0;
   // document.getElementById("trigB").innerHTML = "auto";
    document.getElementById("trigB").style = "background-color:pink";
    document.getElementById("runstop").style = "background-color:lime";
  }
}

function sendChOn() {
  var str = "chOn?val=";
  var res = str.concat((chOn[0]>0) + 2 * (chOn[1]>0)); // 0 of off, 1 on off, 2, off on, 3 on on 
  sendXhttp(res);
}

function toggleDrawOs() {
  drawOs[chSelected - 1] = !drawOs[chSelected - 1];
  updateDraw0B();
}

function updateACC() {
    if (acCoupling[chSelected-1])
      document.getElementById("acc").innerHTML = "&#x2705;";
    else
      document.getElementById("acc").innerHTML = "&#127996;";
}

function updateEdge() {
    if (risingEdge[chSelected-1]==1)
      document.getElementById("edge").innerHTML = "&#10549;";
    else
      document.getElementById("edge").innerHTML = "&#10548;";
}

function updateGround() {
  gndslider.value = ground[chSelected-1];
}

function updateDraw0B() {
  if ((chSelected == 1 && drawOs[0]) || (chSelected == 2 && drawOs[1])) {
    document.getElementById("draw0B").style = "background-color:lime";
  } else {
    document.getElementById("draw0B").style = "background-color:pink";
  }
}

function updateVolt() {
  if (chSelected == 1)
      document.getElementById("volt").innerHTML = mvPerDiv[0]/mcpgain;
  else
      document.getElementById("volt").innerHTML = mvPerDiv[1];
}

function updateChOn() {
  if ((chSelected == 1 && chOn[0]) || (chSelected == 2 && chOn[1])) {
    document.getElementById("chOn").style = "background-color:lime";
  } else {
    document.getElementById("chOn").style = "background-color:pink";
  }
}

function updateChRB() {
  if (chOn[0]) document.getElementById("ch1").innerHTML = "&#9989;";
  else document.getElementById("ch1").innerHTML = "&#127996;";
  if (chOn[1]) document.getElementById("ch2").innerHTML = "&#9989;";
  else document.getElementById("ch2").innerHTML = "&#127996;";
}

function toggleChOn() {
  chOn[chSelected-1] = chOn[chSelected-1]+1;
  if (chOn[chSelected-1] == 2) {
    chOn[chSelected-1] = 0;
  } 
  sendChOn();
  updateChOn();
  updateChRB();
}

document.getElementById("resslider").oninput = function() {
  document.getElementById("resolution").innerHTML = 50*this.value;
}
document.getElementById("resslider").onchange = function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("resolution").value = parseInt(this.responseText);
    }
  }
  var str = "resolution?val=";
  var res = str.concat(50*this.value);
  xhttp.open("GET", res, true);
  xhttp.send();
}

document.getElementById("MCPslider").oninput = function() {
  mcpgain = mcpgains[this.value];
  document.getElementById("MCPgain").innerHTML = mcpgain;
  updateVolt();
}

document.getElementById("MCPslider").onchange = function() {
  var str = "mcpgain?val=";
  var res = str.concat(this.value);
  sendXhttp(res);
}

document.getElementById("triggerslider").oninput = function() {
  trigCh = chSelected;
  trigger[chSelected - 1] = this.value;
  trig.innerHTML = "Trigger on Ch" + String(trigCh);
  trig.style.color = "black";
}
document.getElementById("triggerslider").onchange = function() {
  var str = "trigV?val=";
  var res = str.concat(trigger[chSelected-1]-ground[chSelected-1]);
  sendXhttp(res);
  var str = "trigCh?val=";
  var res = str.concat(trigCh);
  sendXhttp(res);
  var str = "risingEdge?val=";
  var res = str.concat(risingEdge[chSelected-1]);
  sendXhttp(res);
}
document.getElementById("hoffslider").oninput = function() {
  yoff = Math.floor(this.value);
}
gndslider.oninput = function() {
  ground[chSelected-1] = this.value;
}
gndslider.onchange = function() {
  updateGround();
}

function speedup() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      usPerDiv = parseInt(this.responseText);
      document.getElementById("speed").innerHTML = usPerDiv;
    }
  };
  xhttp.open("GET", "speedup", true);
  xhttp.send();
}

function speeddown() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      usPerDiv = parseInt(this.responseText);
      document.getElementById("speed").innerHTML = usPerDiv;
    }
  };
  xhttp.open("GET", "speeddown", true);
  xhttp.send();
}

function voltup() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      mvPerDiv[chSelected-1] = parseInt(this.responseText);
      updateVolt();
    }
  };
  xhttp.open("GET", "voltup", true);
  xhttp.send();
}

function voltdown() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      mvPerDiv[chSelected-1] = parseInt(this.responseText);
      updateVolt();
    }
  };
  xhttp.open("GET", "voltdown", true);
  xhttp.send();
}

function accb() {
   acCoupling[chSelected-1] = !acCoupling[chSelected-1];
   updateACC();
}

function edgeCB() {
  risingEdge[chSelected-1] = (risingEdge[chSelected-1]+1)%2;
  updateEdge();
}


function tedgeCB() {
  trigEdges[chSelected-1] = (trigEdges[chSelected-1]+1)%5;
  document.getElementById("tedge").innerHTML = String(trigEdges[chSelected-1]+1)+"/5";
}

  // OSCOPE DISPLAY
function LEDon() {
  ctx.fillStyle = '#00ff00';
  ctx.strokeStyle = '#00cc00';
}

function LEDoff() {
  ctx.fillStyle = '#008800';
  ctx.strokeStyle = '#007700';
}

var togglevar; 
function toggleLED (){
  togglevar = !togglevar;
  ctx.setLineDash([]);
  ctx.lineWidth = 9;
  ctx.beginPath();
  ctx.arc(492, 9, 4, 0, 2 * Math.PI);
  if (togglevar) LEDon();
  else LEDoff();
  ctx.stroke();
  ctx.fill();
  ctx.lineWidth = 1; //reset line and text color
  ctx.fillStyle = '#000000';
}

function drawScope() {
  ctx.clearRect(0, 0, 501, 401);
  if (triggerMode==1) {
    ctx.fillStyle = "lightgrey";
    ctx.fillRect(0, 0, 501, 401); 
    ctx.fillStyle = "white";
    ctx.font = "30px Arial";
    ctx.fillText("Waiting on trigger CH"+String(trigCh), 70, 50);
    ctx.font = "14px Arial";
  }
  toggleLED();
  ctx.setLineDash([]);
  ctx.strokeStyle = "#008800";
  ctx.beginPath();
  ctx.moveTo(250, 0);  ctx.lineTo(250, 400);
  ctx.moveTo(0, 200);  ctx.lineTo(500, 200);
  ctx.stroke();
  ctx.beginPath();
  ctx.setLineDash([1, 5]);
  for (i = 1; i < 8; i++) {
    ctx.moveTo(0, i * 50);    ctx.lineTo(500, i * 50);
  }
  for (i = 1; i < 10; i++) {
    ctx.moveTo(i * 50, 0);
    ctx.lineTo(i * 50, 400);
  }
  ctx.stroke();
  //trigger pointer

  var tmpyoff = yoff*ousPerDiv/usPerDiv;
  ctx.setLineDash([]);
  if (triggered) ctx.strokeStyle = "#00FF00"; // green pointer
  else ctx.strokeStyle = "#FF0000"; // red pointer
  ctx.beginPath();
  ctx.moveTo(tmpyoff, 10);  ctx.lineTo(tmpyoff + 10, 1);  ctx.lineTo(tmpyoff - 10, 1);  ctx.lineTo(tmpyoff, 10);
  ctx.stroke();
  ctx.fillStyle = chColor[trigCh-1];
  ctx.fillText(String(trigCh), tmpyoff - 3, 20);
  ctx.fillStyle = "black";
  if (tmpyoff > 450) tmpyoff = 450
  ctx.fillText(String(yoff*ousPerDiv/50 - 250*usPerDiv/50)+" us" , tmpyoff, 40);
  //trigger voltage
  let xt = 400 - trigger[chSelected - 1];
  ctx.strokeStyle = "#880000";
  ctx.setLineDash([5, 5]);
  ctx.fillText(String((trigger[chSelected - 1] - ground[chSelected-1]) * mvPerDiv[chSelected-1] / 50 / mcpgain) + " mV", 440, xt);
  ctx.fillText(String(Math.round(measureFreq())) + " Hz", 440, xt + 20);
  ctx.beginPath();
  ctx.moveTo(0, xt);  ctx.lineTo(500, xt);
  ctx.stroke();
  // ground line
  ctx.strokeStyle = "#000088";
  ctx.setLineDash([5, 5]);
  ctx.fillText("0 mV", 10, 408 - ground[chSelected-1]);
  ctx.beginPath();
  ctx.moveTo(0, 400 - ground[chSelected-1]);  ctx.lineTo(500, 400 - ground[chSelected-1]);
  ctx.stroke();
  
  if (chSelected == 1) {
    ctx.setLineDash([2, 15]);
    // min V line ADC calibration (~142mV/mcpgain) 
    let minVline = 400 - (400*minADC/mvPerDiv[0]/8)*omvPerDiv[0]/mvPerDiv[0]-ground[chSelected-1];
    ctx.fillText("Min ADC", 10, minVline);
    ctx.moveTo(0, minVline);  ctx.lineTo(500, minVline);
    ctx.stroke();
    // max V line adc calibration (~3.3V/mcpgain) 
    let maxVline = 400 - (400*maxADC/mvPerDiv[0]/8)*omvPerDiv[0]/mvPerDiv[0]-ground[chSelected-1];
    ctx.fillText("Max ADC", 10, maxVline);
    ctx.beginPath();
    ctx.moveTo(0, maxVline);  ctx.lineTo(500, maxVline);
    ctx.stroke();
  }
}

function drawCircle(whichchannel) {
  var  yshift = 0, edge;
  ctx.setLineDash([]);
  ctx.beginPath();
  edge = findTrigger();
  yshift = w[edge] - 250 - yoff*ousPerDiv/usPerDiv + 250;
  if (whichchannel == 1) x = v[0];
  if (whichchannel == 2) x = u[0];
  ctx.moveTo(w[0] - yshift, 400 - x);
  for (i = 1; i < wend; i++) {
    if (whichchannel == 1) x = v[i];
    if (whichchannel == 2) x = u[i];
    
    ctx.beginPath();
    ctx.arc(w[i] - yshift, 400 - x,
            5, 0, 2*Math.PI);
    ctx.stroke();
  }
}

function drawLine(whichchannel) {
  var yshift = 0, edge;
  ctx.setLineDash([]);
  ctx.lineWidth = 2;
  ctx.beginPath();
  edge = findTrigger();
  yshift = w[edge] - 250 - yoff*ousPerDiv/usPerDiv + 250;
  if (whichchannel == 1) x = v[0];
  if (whichchannel == 2) x = u[0];
  ctx.moveTo(w[0] - yshift, 400 - x);
  for (i = 1; i < wend; i++) {
    if (whichchannel == 1) x = v[i];
    if (whichchannel == 2) x = u[i];
    if (w[i] - w[i - 1] > 50) //  if large jump or return stroke ignore
      ctx.moveTo(w[i] - yshift, 400 - x);
    else
      ctx.lineTo(w[i] - yshift, 400 - x);
  }
  ctx.stroke();
  ctx.lineWidth = 1;
}

var oldTime;
function displayFramerate(){
  var d = new Date();
  let fr = 1000/(d.getTime() - oldTime);
  oldTime = d.getTime();
  let eff = (10*usPerDiv)/1000000*fr;
  frame.innerHTML = " " + " #data = "+String(z.length) + 
    ": FPS = " + String(fr.toPrecision(3)) +" " + String(omvPerDiv[1]/mvPerDiv[1]) ;
}

 // OSCOPE DATA HANDLING
function getData() {
    if (chOn[0] || chOn[1]) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4) { // && this.status == 200 ) { //ignore errors
          z = this.responseText.split(",");
          updateloop();
        }
      };
      xhttp.open("GET", "up", true);
      xhttp.send();
    } 
    else updateloop();
    omvPerDiv[0] = mvPerDiv[0];
    omvPerDiv[1] = mvPerDiv[1];  
    ousPerDiv = usPerDiv;
}

function checkData() {
  if (triggerMode==0) { // run normal
    getData();
  } 
  else if (triggerMode == 1) { // run once
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4  && this.status == 200 ) { 
        z = this.responseText.split(",");
        if (z.length > 3) {
          triggerMode = 2;
          document.getElementById("runstop").style = "background-color:pink";
        } 
        updateloop();
      }
    };
    xhttp.open("GET", "single", true);
    xhttp.send();
  }  
  else updateloop();
}

var t = 0;
var chOnSent = [0,0];
function processData() {
  var tstart;
  i = 0;
  chOnSent[0] = parseInt(z[i++]);
  chOnSent[1] = parseInt(z[i++]);
  woffset = 0;
  voffset = 0;
  uoffset = 0
  tstart = z[i];
  while (i < z.length) {
    t = z[i++];
    
    wPush(Math.floor((500 * (t - tstart)) / (10 * usPerDiv)));
    if (chOnSent[0]) {    
      avgV[0] = (avgV[0]*800 + parseInt(z[i])) / (800+1);  // running avg filter
      if (acCoupling[0])  vPush((z[i++] - avgV[0])*omvPerDiv[0]/mvPerDiv[0] + parseInt(ground[0]));
      else                vPush((z[i++]          )*omvPerDiv[0]/mvPerDiv[0] + parseInt(ground[0]));
    }
    if (chOnSent[1])   {    
      avgV[1] = (avgV[1]*800 + parseInt(z[i])) / (800+1);  // running avg filter
      if (acCoupling[1])  uPush((z[i++] - avgV[1])*omvPerDiv[1]/mvPerDiv[1] + parseInt(ground[1]));
      else                uPush((z[i++]          )*omvPerDiv[1]/mvPerDiv[1] + parseInt(ground[1]));
    }
  } 
  wend = woffset - 1;
}

function findTrigger() {
  var i, oldx = 0, x = 0, edgeNo =0;
  var istart = 0;// Math.floor(wend / 3); // choose left of middle
  var trigV = Math.floor(trigger[trigCh - 1]);
  triggered = 1;
  if (trigCh == 1) oldx = v[istart - 1];
  if (trigCh == 2) oldx = u[istart - 1];
  for (i = istart; i < wend; i++) {
    if (trigCh == 1) x = v[i];
    if (trigCh == 2) x = u[i];
    if (risingEdge[trigCh-1] && oldx > trigV && x <= trigV) {
      if (++edgeNo > trigEdges[trigCh-1]) return i; 
    } 
    else if (!risingEdge[trigCh-1] && oldx < trigV && x >= trigV) {
      if (++edgeNo > trigEdges[trigCh-1]) return i; 
    }
    oldx = x; 
  }

  triggered = 0;
  return istart;
}

function updateloop() {
  var i;
  processData();
  drawScope();
  for (i=0; i<2; i++) { 
    if (chOnSent[i]) {
      ctx.strokeStyle = chColor[i];
      drawLine(i+1);
      if (drawOs[i]) drawCircle(i+1);
    }
  }
  
  if (triggerMode==1 && triggered) {
    triggerMode = 2;
    document.getElementById("hoffslider").min = w[0];
    document.getElementById("runstop").style = "background-color:pink";
  }

  setTimeout(checkData, 20); // RUN mode repeat call up to 50Hz
  displayFramerate();
}

</script>
</body>
</html>
)===";
