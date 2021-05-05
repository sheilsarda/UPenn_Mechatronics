const char body[] PROGMEM = R"===( 
<!DOCTYPE html>
<html>
<head>  <title>Ranging Display</title>
<style> 
  article, footer { float: left; border-radius: 15px;  padding: 10px;  width:100%; background-color: #f1f1f1;}
  footer { background-color: #ddd; }
  section { width: 510px;}
</style> 
</head>

<body>
<section>
<article>
  <canvas id="myCanvas" width="500" height="400" style="border:3px solid #d3d3d3;">
  Your browser does not support the HTML5 canvas tag.
  </canvas> <br>
  <span style="font-size: 25px;"> Ranging </span>  &nbsp;
  
  <button type="button" onclick="zoomf()"> &nbsp; toggle zoom &nbsp; </button> <br>
</article>
</section>

<script>

// Display Elements
var c = document.getElementById("myCanvas");
var ctx = c.getContext("2d");
ctx.font="14px Arial";

var ch = [];
var scansize=45;
var zoom = 8;

setInterval(getData, 1000);

function zoomf() {
  if (zoom == 8) zoom = 2;
  else zoom = 8;
  getData();
}

function drawScreen() {
  ctx.clearRect(0, 0, c.width, c.height);
  ctx.strokeStyle = "#008800";
  ctx.beginPath();
  ctx.setLineDash([1, 5]);

  // draw distance lines
  for (let i=1; i < 8; i++) {
    ctx.moveTo(0, i * 50);
    ctx.lineTo(c.width, i * 50);  
    ctx.fillText(String((8-i)*50*zoom)+" mm",20,i*50);
  }
  ctx.stroke();
  
  // draw origin (robot as circle)
  ctx.beginPath();
  ctx.arc(c.width/2, c.height-2, 30/zoom, 0, 2 * Math.PI, false);
  ctx.fill();
  ctx.stroke();
}

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200 ) { 
      updateGraph(this);
    }
  };
  xhttp.open("GET", "up", true);
  xhttp.send();
}

function radius(i){return ch[i+1]; }

function theta(i){ return ch[i+1+scansize]*Math.PI/180; }

function x(i){     return radius(i)*Math.cos(theta(i))/zoom; }

function y(i){     return radius(i)*Math.sin(theta(i))/zoom; }

function drawDataCircles() {
  ctx.setLineDash([]);
  for (let i=0; i < scansize; i++) {
    ctx.beginPath();
    ctx.arc(c.width/2 + y(i), c.height - x(i),
            radius(i)/80, 0, 2*Math.PI);
    ctx.stroke();
  }
}

function updateGraph(xhttp) {
      scansize = parseInt(xhttp.responseText); // get 1st value
      drawScreen(); 
      
      // draw old data in light green
      ctx.strokeStyle = "#88FF88"; 
      drawDataCircles();
      
      // draw new data in dark green
      ch = xhttp.responseText.split(","); // get ranging data
      ctx.strokeStyle = "#008800";
      drawDataCircles();
//  debug.innerHTML = ch; // debugging print
}
</script>
</body>
</html>

)===";
