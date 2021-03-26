/*
 * Joystick HTML and javascript 
 * uses mouse input
 * 
 * code based on iamrachelle
 * https://www.instructables.com/Making-a-Joystick-With-HTML-pure-JavaScript/
 * added esp32 stuff and extra buttons and gradient graphics
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
  <button type="button" onclick="armup()"> &nbsp; armup &nbsp;  </button> <br>
  <button type="button" onclick="armdown()"> &nbsp; armdown &nbsp;  </button> <br>
  <button type="button" onclick="switchmode()"> &nbsp; switch to tankmode &nbsp;  </button> <br>
  <span id="armstate"> armstate </span> <br>
   Sent: X: <span id="x_coord"> </span> Y: <span id="y_coord"> </span> <br>
  Received X,Y: <span id="joystate"> joystate</span>
      
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
      upDate();
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
      upDate();
  }

  function background() {
      x_orig = width / 2;
      y_orig = height / 2;
      ctx.beginPath();
      ctx.arc(x_orig, y_orig, radius + 20, 0, Math.PI * 2, true);
      ctx.fillStyle = '#c5c5c5';
      ctx.fill();
  }

  function upDate(){
    joystick();
    sendJoy(x,y);
  }
  
  function joystick() { 
      var grd = ctx.createRadialGradient(x+10,y-10,3,x+10,y-10,50);
      ctx.beginPath();
      grd.addColorStop(0,"white");
      grd.addColorStop(1,"red");
      ctx.arc(x, y, radius, 0, Math.PI * 2, true);
      ctx.fillStyle = grd;
      ctx.fill();
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

  function inCircle() {
      var current_radius = Math.sqrt(Math.pow(coord.x - x_orig, 2) + Math.pow(coord.y - y_orig, 2));
      if (radius >= current_radius) return true
      else return false
  }

  function startDrawing(event) {
      getPosition(event);
      if (inCircle()) {
          paint = true;
          ctx.clearRect(0, 0, canvas.width, canvas.height);
          background();
          cts = 1;      // hack to ensure message goes through
          x = coord.x; y = coord.y;
          upDate();         
          Draw();
      }
  }

  function stopDrawing() {
      if (paint) {
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        background();
        cts = 1;      // hack to ensure message goes through    
        x = width/2; y= height/2;
        upDate();
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

          if (inCircle()) {
              x = coord.x;
              y = coord.y;
          }
          else {
              x = radius * Math.cos(angle) + x_orig;
              y = radius * Math.sin(angle) + y_orig;
          }
          upDate();

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

</script>
</body>
</html>
)===";
