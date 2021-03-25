/*
 * MEAM510 HTML510 example w/LED
 * March 2021
 * 
 * Mark Yim
 * University of Pennsylvania
 * copyright (c) 2021 All Rights Reserved
 */
 
#include <WiFi.h> 
#include "html510.h"

#define LEDPIN 21
                         
const char* ssid     = "Fios-rTWv7";
const char* password = "Govinda1!";


// the number of the LED pin
const int ledPin1 = 21;  
const int ledPin2 = 10;

// setting PWM properties
const int freq = 4000;
const int ledChannel = 0;
const int resolution = 8;

int val = 0;  // variable to store the value read
int dutyCycle = 255;
 

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

setInterval(updateLabel,2000);

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
)==="

/*****************/
/* web handler   */
void handleRoot(){
  sendhtml(body);
}

void handleH(){
  digitalWrite(LEDPIN, HIGH);  // LED ON  
  sendhtml(body);
}                    
                     
void handleL(){
  digitalWrite(LEDPIN, LOW);  // LED ON  
  sendhtml(body);
}

void handleHit(){
  static int toggle;
  if (++toggle%2)  digitalWrite(LEDPIN,HIGH);
  else digitalWrite(LEDPIN,LOW);
  sendplain(""); // acknowledge         
}

void handleLEDstate(){
  String    s = "Frequency is " + String(freq) + "<br>";
            s += "Duty Cycle is ";
            s += String((float) dutyCycle/255 * 100.0);
            s += "<br>"
  sendplain(s);
}


void setup() {
  Serial.begin(115200);                                             
  pinMode(LEDPIN, OUTPUT);
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
  pinMode(4, INPUT);

  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin1, ledChannel);
  ledcAttachPin(ledPin2, ledChannel);
}

void loop(){

  val       = analogRead(4);
  dutyCycle = 255*((float) val/1024.0);
  ledcWrite(ledChannel, dutyCycle);
  
  serve(server, body);
}
