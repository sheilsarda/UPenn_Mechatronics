/*
   MEAM510 HTML510 example w/LED
   March 2021

   Sheil Sarda
   University of Pennsylvania
   copyright (c) 2021 All Rights Reserved
*/

#include <WiFi.h>
#include "html510.h"

#define MOTOR       33  // variable duty
#define POT_PIN     35  // potentiometer
#define DIR_PIN     32  // forward / backward

// setting PWM properties
const int freq = 50;
const int ledChannel = 1;
const int resolution = 8;

int val = 0;            // read in pot
int dutyCycle = 0;    // no movement

WiFiServer server(80);

const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html><body>
<H1>
<a href="/H">Turn ON</a> LED.<br>
<a href="/L">Turn OFF</a> LED.<br>
<span id="freqlabel"> LED State HERE </span> <br>

</H1>
<button type="button" onclick="hit()"> Turn ON then OFF </button>
</body>
<script>
function hit() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "hit", true);
  xhttp.send();
}  

setInterval(updateLabel,400);

function updateLabel() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("freqlabel").innerHTML =   
             this.responseText;
    }
  };
  xhttp.open("GET", "LEDstate", true);
  xhttp.send();
}

</script>

</html>
)===";

/*****************/
/* web handler   */
void handleRoot(){
  sendhtml(body);
}

void handleH(){
  sendhtml(body);
}                    
                     
void handleL(){
  sendhtml(body);
}

void handleHit(){
  sendplain(""); // acknowledge         
}

void handleLEDstate(){
  String    s = "Frequency is " + String(freq) + "<br>";
            s += "Duty Cycle is ";
            s += String(dutyCycle*100/254);
            s += "% <br>";
  sendplain(s);
}


void setup() {
  Serial.begin(115200);                                             
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192, 168, 1, 6),
              IPAddress(192, 168, 1, 1),
              IPAddress(255, 255, 255, 0));
              
  while(WiFi.status()!= WL_CONNECTED ) { 
    delay(500);
    Serial.print("."); 
  }
  
  Serial.println("WiFi connected"); server.begin();

  attachHandler("/H",handleH);
  attachHandler("/L",handleL);
  attachHandler("/ ",handleRoot);
  attachHandler("/hit",handleHit);
  attachHandler("/LEDstate",handleLEDstate);

  analogReadResolution(10);
  pinMode(POT_PIN, INPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(MOTOR, OUTPUT);

  ledcAttachPin(MOTOR, ledChannel);
  ledcSetup(ledChannel, freq, resolution);
}


void loop(){
  val       = analogRead(POT_PIN);   

  if(val > 512) {
    digitalWrite(DIR_PIN, HIGH);
    dutyCycle   = 255*((float) (val - 512)/512.0);
  }
  else {
    digitalWrite(DIR_PIN, LOW);
    dutyCycle   = 255*((float) (val)/512.0);
  }

  ledcWrite(ledChannel, dutyCycle);

  serve(server, body);
}
