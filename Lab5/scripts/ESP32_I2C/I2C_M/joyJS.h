/*
 * Joystick HTML and javascript 
 * uses mouse input
 */
 
const char joybody[] PROGMEM = R"===( 
<!DOCTYPE html>
<html>
<head> <title> Joystick </title>
    <meta name="viewport" content="user-scalable=no">
    <link rel="shortcut icon" type="image/png"
 href="https://icons.iconarchive.com/icons/google/noto-emoji-activities/16/52763-joystick-icon.png">

</head>
<body style="text-align: center;" >

<canvas id="canvas" name="game"></canvas> 
<br>
<button type="button" onclick="backward(3)"> &nbsp; &#8681; &#8681; &nbsp;  </button>
<button type="button" onclick="backward(1.5)"> &nbsp; &#8681; &nbsp;  </button>
<button type="button" onclick="forward(1.5)"> &nbsp; &#8679; &nbsp;  </button> 
<button type="button" onclick="forward(3)"> &nbsp; &#8679; &#8679; &nbsp;  </button> 
<br>
<br>

<button type="button" onclick="armup()"> &nbsp; Wall Follow &nbsp;  </button> 
<button type="button" onclick="armdown()"> &nbsp; Stop Wall Follow &nbsp;  </button> 
<br> 
<br>

<span id="Arm"> Control the Gripper </span> <br>
<button type="button" onclick="openArm()"> &nbsp; Open Arm &nbsp;  </button> 
<button type="button" onclick="closeArm()"> &nbsp; Close Arm &nbsp;  </button> 
<br> 
<br>

<span id="Rotate"> Rotate</span> <br>

<button type="button" onclick="anticlockwise(-1)"> &nbsp; rotate ccw &nbsp;  </button> 
<button type="button" onclick="clockwise(-1)"> &nbsp; rotate cw &nbsp;  </button> <br>

<button type="button" onclick="anticlockwise(90)">90</button>
<button type="button" onclick="anticlockwise(45)">45</button> 
<button type="button" onclick="anticlockwise(15)">15</button> 
---
<button type="button" onclick="clockwise(15)">15</button> 
<button type="button" onclick="clockwise(45)">45</button> 
<button type="button" onclick="clockwise(90)">90</button> 

<br>
<br>
<span id="armstate"> armstate </span> <br>
Sent: X: <span id="x_coord"> </span> Y: <span id="y_coord"> </span> <br>
Received X,Y: <span id="joystate"> joystate</span> <br>

<br>
<br>
<canvas id="rangeView" width="360" height="300" style="border:3px solid #d3d3d3;">
Your browser does not support the HTML5 canvas tag.
</canvas> <br>
<span style="font-size: 25px;"> Ranging </span>  &nbsp;
<span id="sensordata" style="font-size: 15px;"> DATA HERE </span>  &nbsp;
<span id="dataView" style="font-size: 15px;"> DATA HERE </span>  &nbsp;

<button type="button" onclick="zoomf()"> &nbsp; toggle zoom &nbsp; </button> <br>


<script>
  var canvas, ctx;
  var armstate ="up";
  var x=0, y=0;
  var x_relative=0, y_relative=0;
  var resendflag=0;
  var joystate = [];

// limit transfer rate, 200 is 5Hz
  setInterval(myTimer, 200); 
  var cts = 1; // clear to send
  function myTimer() {  
    cts = 1; 
    if (x_relative != joystate[0] || y_relative != joystate[1]) {
      joystick();
    }
  }

// joystick UI
  window.addEventListener('load', () => {
      canvas = document.getElementById('canvas');
      ctx = canvas.getContext('2d');          
      resize(); 
  
      document.addEventListener('mousedown', startDrawing);
      document.addEventListener('mouseup', stopDrawing);
      document.addEventListener('mousemove', Draw);
  
      document.addEventListener('touchstart', startDrawing);
      document.addEventListener('touchend', stopDrawing);
      document.addEventListener('touchcancel', stopDrawing);
      document.addEventListener('touchmove', Draw);
      window.addEventListener('resize', resize);
  
      document.getElementById("x_coord").innerText = 0;
      document.getElementById("y_coord").innerText = 0;
  });

  var width, height, radius=50, x_orig, y_orig;
  function resize() {
      width = window.innerWidth;
      height = radius * 4;
      ctx.canvas.width = width;
      ctx.canvas.height = height;
      background();
      x = width/2; y= height/2;
      joystick();
  }

  function background() {
      x_orig = width / 2;
      y_orig = height / 2;
      ctx.beginPath();
      ctx.arc(x_orig, y_orig, radius + 20, 0, Math.PI * 2, true);
      ctx.fillStyle = '#000000';
      ctx.fill();
  }

  function joystick() { 
      var grd = ctx.createRadialGradient(x+10,y-10,3,x+10,y-10,50);
      ctx.beginPath();
      grd.addColorStop(0,"yellow");
      grd.addColorStop(1,"green");
      ctx.fillStyle = grd;
      ctx.arc(x, y, radius, 0, Math.PI * 2, true);
      ctx.fillStyle = grd;
      ctx.fill();
      sendJoy(x,y);
  }

  let coord = { x: 0, y: 0 };
  let paint = false;

  function getPosition(event) {
    e = window.event || e;
    var mouse_x = e.clientX || e.touches[0].clientX;
    var mouse_y = e.clientY || e.touches[0].clientY;
    coord.x = mouse_x - canvas.offsetLeft;
    coord.y = mouse_y - canvas.offsetTop;
  }

  function is_it_in_the_circle() {
      var current_radius = Math.sqrt(Math.pow(coord.x - x_orig, 2) + Math.pow(coord.y - y_orig, 2));
      if (radius >= current_radius) return true
      else return false
  }

  function startDrawing(event) {
      getPosition(event);
      if (is_it_in_the_circle()) {
          paint = true;
          ctx.clearRect(0, 0, canvas.width, canvas.height);
          background();
          cts = 1;      // hack to ensure message goes through
          x = coord.x; y = coord.y;
          joystick();         
          Draw();
      }
  }

  function stopDrawing() {
      if (paint) {
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        background();
        cts = 1;      // hack to ensure message goes through    
        x = width/2; y= height/2;
        joystick();
      }
      paint = false;
      x_relative=0;
      y_relative=0;
      document.getElementById("x_coord").innerText = x_relative;
      document.getElementById("y_coord").innerText = y_relative;
  }

  function Draw(event) {
      if (paint) {
          ctx.clearRect(0, 0, canvas.width, canvas.height);
          background();
          var angle_in_degrees,cx, cy;
          var angle = Math.atan2((coord.y - y_orig), (coord.x - x_orig));

          if (Math.sign(angle) == -1) angle_in_degrees = Math.round(-angle * 180 / Math.PI);
          else angle_in_degrees = Math.round( 360 - angle * 180 / Math.PI);

          if (is_it_in_the_circle()) {
              x = coord.x;
              y = coord.y;
          }
          else {
              x = radius * Math.cos(angle) + x_orig;
              y = radius * Math.sin(angle) + y_orig;
          }
          joystick();

          getPosition(event);
      }
  } 
  
// UI functions via AJAX to esp32
  function armup() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        armstate = this.responseText;
        document.getElementById("armstate").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "armup", true);
    xhttp.send();
  }
  
  function armdown() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        armstate = this.responseText;
        document.getElementById("armstate").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "armdown", true);
    xhttp.send();
  }

  
    function clockwise(deg) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        Rotate = this.responseText;
        document.getElementById("Rotate").innerHTML = this.responseText;
      }
    };
  var str = "clockwise?val=";
  var res = str.concat(deg);
    xhttp.open("GET", res, true);
    xhttp.send();
  }

    function anticlockwise(deg) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        Rotate = this.responseText;
        document.getElementById("Rotate").innerHTML = this.responseText;
      }
    };
  var str = "anticlockwise?val=";
  var res = str.concat(deg);
    xhttp.open("GET", res, true);
    xhttp.send();
  }

    function openArm() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        Rotate = this.responseText;
        document.getElementById("Arm").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "openArm", true);
    xhttp.send();
  }

  function switchmode() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        location.reload();
      }
    };
    xhttp.open("GET", "switchmode", true);
    xhttp.send();
  }

  function sendJoy(x,y) {
    x_relative = Math.round(x - x_orig);
    y_relative = Math.round(y - y_orig);
    
    document.getElementById("x_coord").innerText = x_relative;
    document.getElementById("y_coord").innerText = y_relative ;

    if (cts) {
      var xhttp = new XMLHttpRequest();
      var str = "joy?val=";
      var res = str.concat(x_relative,",",y_relative);
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4) {
          if (this.status == 200) {
            resendflag = 0;
            joystate = this.responseText.split(",");
            document.getElementById("joystate").innerHTML = this.responseText;
          }
          else  resendflag = 1; // may not need this
        }
      };
      xhttp.open("GET", res, true);
      xhttp.send();
      cts = 0;
    }
  }

// Display Elements
var c = document.getElementById("rangeView");
var vctx = c.getContext("2d");
vctx.font="14px Arial";

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
  vctx.clearRect(0, 0, c.width, c.height);
  vctx.strokeStyle = "#008800";
  vctx.beginPath();
  vctx.setLineDash([1, 5]);

  // draw distance lines
  for (let i=1; i < 8; i++) {
    vctx.moveTo(0, i * 50);
    vctx.lineTo(c.width, i * 50);  
    vctx.fillText(String((8-i)*50*zoom)+" mm",20,i*50);
  }
  vctx.stroke();
  
  // draw origin (robot as circle)
  vctx.beginPath();
  vctx.arc(c.width/2, c.height-2, 30/zoom, 0, 2 * Math.PI, false);
  vctx.fill();
  vctx.stroke();
}

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200 ) { 
      document.getElementById("sensordata").innerHTML = this.responseText;
      updateGraph(this);
    }
  };
  xhttp.open("GET", "update", true);
  xhttp.send();
}

function get_radius(i){return ch[i+1]; }
function get_theta(i){ return ch[i]; }

function get_x(i){     
    return radius(i)*Math.cos(get_theta(i))/zoom; 
}

function get_y(i){     
    return radius(i)*Math.sin(get_theta(i))/zoom; 
}

function drawDataCircles() {
  ctx.setLineDash([]);
  for (let i=0; i < scansize; i++) {
    ctx.beginPath();
    ctx.arc(c.width/2 +  get_y(i), c.height - get_x(i),
            get_radius(i)/80, 0, 2*Math.PI);
    ctx.stroke();
  }
}

var range_sum;

function updateGraph(xhttp) {
      scansize = parseInt(xhttp.responseText); // get 1st value

      drawScreen(); 
      
      // draw old data in light green
      ctx.strokeStyle = "#88FF88"; 
      drawDataCircles();
      
      range_sum = ch.reduce((a, b) => a + b, 0);
      
      // draw new data in dark green
      ch = xhttp.responseText.split(","); // get ranging data
      ctx.strokeStyle = "#008800";
      drawDataCircles();
      
      //  debug.innerHTML = ch; // debugging print
      if(ch.reduce((a, b) => a + b, 0)  > range_sum + 100)
          document.getElementById("dataView").innerHTML = "Obstacle distance INCREASED";
      else if(ch.reduce((a, b) => a + b, 0)  < range_sum + 100)   
          document.getElementById("dataView").innerHTML = "Obstacle distance DECREASED";
      else document.getElementById("dataView").innerHTML = "Obstacle distance UNCHANGED"; 
}

</script>
</body>
</html>
)===";
