/****************************************************** 
 * Scans range sensor mounted on servo and displays on web
 * Ultrasonic ranger  uses TriggerPin GPIO19 and EchoPin GPIO18
 ******************************************************/

//****************************
//********* Ultrasonic sensor stuff:
//****************************
#define trigPin 18
#define echoPin 19

long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

int rangeSonar() {    // return range distance in mm
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  return distance * 10;
}
//****************************
//********* Servo stuff:
//****************************
#define LEDC_CHANNEL       0 // use first channel of 16  
#define LEDC_RESOLUTION_BITS  14
#define LEDC_RESOLUTION  ((1<<LEDC_RESOLUTION_BITS)-1) 
#define LEDC_FREQ_HZ     60
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {            
  uint32_t duty =  LEDC_RESOLUTION * min(value, valueMax) / valueMax;   
  ledcWrite(channel, duty);  // write duty to LEDC 
}

//****************************
//********* Scanning stuff:
//****************************
#define SAMPLEFREQ 15   // TOF can use 30, Ultrasonic maybe 15
#define SCANSPEED 40
#define SCANSIZE 45
#define ARRAYMAX SCANSIZE*2 // needs to be bigger than scansize
int scanR[ARRAYMAX];
int scanoffset = SCANSIZE; // start at SCANSIZE so we don't endup negative mod
int i;

void printScan(){
    int offset = 10;
    i = (scanoffset - 1) % ARRAYMAX;  
    Serial.print(scanR[i]);
    for (int j=0; j<scanR[i]; j+=offset){
        Serial.print(".");
        offset *= 1.5;
    }
    Serial.println("#");
}


void scanStep(int range) {
  printScan();
    
  scanR[scanoffset % ARRAYMAX] = range;
  scanoffset++;
}

//****************************
//********* WEB STUFF:
//****************************
#include <WiFi.h>
#include <WiFiClient.h>
#include "index.h"  //Web page file
#include "html510.h"
WiFiServer server(80);


void handleUpdate() {
  String s = "";

  s = s+SCANSIZE;                  // first number is number of data pairs
  for (int i=0; i<SCANSIZE; i++) { // add range values
    s = s+","+ scanR[(scanoffset-i) % ARRAYMAX]; // range sensor lags angle by 1 step
  }
  
  sendplain(s);
}

void handleFavicon(){
  sendplain(""); // acknowledge
}

void handleRoot() {
  sendhtml(body);
}

//****************************
// setup and loop
//****************************
void setup()
{
  Serial.begin(115200);
  Serial.println("starting");
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192, 168, 1, 6),
              IPAddress(192, 168, 1, 1),
              IPAddress(255, 255, 255, 0));
  while(WiFi.status()!= WL_CONNECTED ) { 
    delay(500);
    Serial.print("."); 
  }  
  Serial.print("Use this URL to connect: http://");  
  Serial.print(WiFi.localIP()); Serial.println("/");
  server.begin();                  //Start server
  
  // webpage handlers
  attachHandler("/up",handleUpdate);
  attachHandler("/favicon.ico",handleFavicon);
  attachHandler("/ ",handleRoot);
}

void loop()
{
  static uint32_t lastUpdate = micros();
  static uint32_t lastmicros = micros();
  static uint32_t us = micros();
  int range;
  
  us = micros();
  if (us-lastmicros > 5000){   // check for webpage request
    lastmicros = us;
    serve(server,body);    
  }
  if (us-lastUpdate > 1000000/SAMPLEFREQ) { // update the servo position
    range = rangeSonar();   // uncomment if using Sonar
    scanStep(range);
    lastUpdate = us;
  }
}
