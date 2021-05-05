/****************************************************** 
 * RANGER DEMO scans range sensor mounted on servo and displays on web
 * 
 * ToF ranger VL53L0X uses I2C on ESP32 SCL = GPIO21 and SDA = GPIO22
 * Ultrasonic ranger  uses TriggerPin GPIO19 and EchoPin GPIO18
 *
 * Mark Yim
 * University of Pennsylvania
 * copyright (c) 2021 All Rights Reserved
 ******************************************************/

//****************************
//********* ToF sensor stuff:
//****************************
#include "Adafruit_VL53L0X.h"
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

int rangeToF (){
  VL53L0X_RangingMeasurementData_t measure;
  
  lox.rangingTest(&measure, false); // pass 'true' for debugging info 
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
  //Serial.printf("Distance (mm): %d\n",measure.RangeMilliMeter);
    return measure.RangeMilliMeter;
  }
//Serial.println(" out of range ");
  return 0;  // error, real values are never 0
}

//****************************
//********* Ultrasonic sensor stuff:
//****************************
#define trigPin 18
#define echoPin 19

int rangeSonar() {    // return range distance in mm
  int mm;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  mm = 10*pulseIn(echoPin,HIGH) / 58;
  //Serial.printf("sonar range %d\n",mm);
  return mm;
}

//****************************
//********* Servo stuff:
//****************************
#define LEDC_CHANNEL       0 // use first channel of 16  
#define LEDC_RESOLUTION_BITS  14
#define LEDC_RESOLUTION  ((1<<LEDC_RESOLUTION_BITS)-1) 
#define LEDC_FREQ_HZ     60
#define SERVO_PIN        23
#define SERVO_SCAN       6   // can increase this to have wider sweep
#define FULLBACK  LEDC_RESOLUTION*(15-SERVO_SCAN)*60/10000 
#define SERVOOFF  LEDC_RESOLUTION*15*60/10000   // center servo (1.5ms pulse)
#define FULLFRONT LEDC_RESOLUTION*(15+SERVO_SCAN)*60/10000
#define SERVOTODEG(x) ((x)*(int8_t)90*10000)/(12*LEDC_RESOLUTION*60)

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
int scanA[ARRAYMAX];
int scanoffset = SCANSIZE; // start at SCANSIZE so we don't endup negative mod

void printScan(){
  for (int i=0; i< 100; i++) {
    if (scanR[i]) {
      Serial.print(i);
      for (int j=0; j<scanA[i]; j+=10) {
        Serial.print(".");
      }
      Serial.println("#");
    }
  }
}

void scanStep(int range) {
  static int angle;
  static int dir=SCANSPEED;
  
  if (angle+SERVOOFF > FULLFRONT) {
    dir = -SCANSPEED;
  }
  if (angle+SERVOOFF < FULLBACK) {
    dir = SCANSPEED;    
//    printScan();
  }
  scanR[scanoffset % ARRAYMAX] = range;
  scanA[scanoffset % ARRAYMAX] = -SERVOTODEG(angle);
  scanoffset++;
  angle += dir;
  ledcAnalogWrite(LEDC_CHANNEL, SERVOOFF+angle, LEDC_RESOLUTION);
}

//****************************
//********* WEB STUFF:
//****************************
#include <WiFi.h>
#include <WiFiClient.h>
#include "index.h"  //Web page file
#include "html510.h"
WiFiServer server(80);

//const char* ssid     = "YOURSSID";
//const char* password = "YOURPASSWORD";

void handleUpdate() {
  String s = "";

  s = s+SCANSIZE;                  // first number is number of data pairs
  for (int i=0; i<SCANSIZE; i++) { // add range values
    s = s+","+ scanR[(scanoffset-i) % ARRAYMAX]; // range sensor lags angle by 1 step
  }
  for (int i=0; i<SCANSIZE; i++) { // add angle value
    s = s+","+ scanA[(scanoffset-i-1) % ARRAYMAX];
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
  ledcSetup(LEDC_CHANNEL, 60, LEDC_RESOLUTION_BITS); // channel, freq, bits
  ledcAttachPin(SERVO_PIN, LEDC_CHANNEL);
  ledcAnalogWrite(LEDC_CHANNEL, SERVOOFF, LEDC_RESOLUTION); 
  Serial.println("starting");
  
  if (!lox.begin(0x53)) { // for debugging use lox.begin(0x29,true), if device
    Serial.println(F("Failed to boot VL53L0X"));
    delay(2000);
  }
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
  static uint32_t lastServo = micros();
  static uint32_t lastmicros = micros();
  static uint32_t us = micros();
  int range;
  
  us = micros();
  if (us-lastmicros > 5000){   // check for webpage request
    lastmicros = us;
    serve(server,body);    
  }
  if (us-lastServo > 1000000/SAMPLEFREQ) { // update the servo position
    //range = rangeToF();     // uncomment if using ToF sensor
    range = rangeSonar();   // uncomment if using Sonar
    //range = 100;              // uncomment if testing display send dummy data
    scanStep(range);
    lastServo = us;
  }
}
