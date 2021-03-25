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

WiFiServer server(80);              

const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html><body>
<H1>
<a href="/H">Turn ON</a> LED.<br>
<a href="/L">Turn OFF</a> LED.<br>
<span id="somelabel">  </span> <br>

</H1>
<button type="button" onclick="hit()"> mybutton </button>
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
      document.getElementById("somelabel").innerHTML =   
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
  String s = "LED state is "+String(digitalRead(LEDPIN));
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

}

void loop(){
  serve(server, body);
  delay(1);
}
