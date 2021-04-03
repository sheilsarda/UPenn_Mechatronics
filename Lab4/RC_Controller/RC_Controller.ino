/*
 * Sheil Sarda
 * University of Pennsylvania
 * copyright (c) 2021 All Rights Reserved
 */
#include <WiFi.h>
#include "html510.h"
#include "joyJS.h"
#include "tankJS.h"

WiFiServer server(80);
const char *body;

/********************/
/* HTML510  web   */
void handleFavicon(){
  sendplain(""); // acknowledge
}

void handleRoot() {
  sendhtml(body);
}

void handleSwitch() { // Switch between JOYSTICK and TANK mode
  String s="";
  static int toggle=0;
  if (toggle) body = joybody;
  else body = tankbody;
  toggle = !toggle;
  sendplain(s); //acknowledge
}

#define RIGHT_CHANNEL0      0 // use first channel of 16  
#define LEFT_CHANNEL1       1
#define SERVOPIN1   32
#define SERVODIR1   21

#define SERVOPIN2   33
#define SERVODIR2   22

#define SERVOFREQ   50
#define LEDC_RESOLUTION_BITS  12
#define LEDC_RESOLUTION  ((1<<LEDC_RESOLUTION_BITS)-1) 
#define FULLBACK LEDC_RESOLUTION*1          
#define SERVOOFF  LEDC_RESOLUTION*0  
#define FULLFRONT  LEDC_RESOLUTION*1  

int leftservo, rightservo, leftdir, rightdir;

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {            
  uint32_t duty =  LEDC_RESOLUTION * min(value, valueMax) / valueMax;   
  ledcWrite(channel, duty);  // write duty to LEDC 
}

void updateServos() {
  ledcAnalogWrite(LEFT_CHANNEL1, leftservo, LEDC_RESOLUTION);
  digitalWrite(SERVODIR1, leftdir);

  ledcAnalogWrite(RIGHT_CHANNEL0, rightservo, LEDC_RESOLUTION); 
  digitalWrite(SERVODIR2, rightdir);
}

/************************/
/* joystick mode  code  */

int leftarm, rightarm;
int x,y;

void handleJoy() {
  int left, right;
  x = getVal(); // from -50 to +50
  y = getVal();
  String s = String(x) + "," + String(y);

  left = x - y;
  right = x + y;

  leftservo = map(left, -50, 50, FULLBACK, FULLFRONT);
  rightservo = map(right, -50, 50, FULLBACK, FULLFRONT); 

  sendplain(s);
  Serial.printf("received X,Y:=%d,%d\n",x,y); 
}

void handleArmdown() {
  // do something? 
  Serial.println("armdown");
  sendplain(""); //acknowledge
}

void handleArmup() {
  // do something? 
  Serial.println("armup");
  sendplain(""); //acknowledge
}

/*********************/
/* tank mode  code  */
int leftstate, rightstate;
long lastLeverMs;

void handleLever() {
  leftarm = getVal();
  rightarm = getVal();
  leftstate = getVal();
  rightstate = getVal();
  String s = String(leftarm) + "," + String(rightarm) + "," +
             String(leftstate) + "," + String(rightstate);

 //  if (leftarm) do something?
 //  if (rightarm) do 192.168.43.100something?

  if (leftstate > 0)    leftdir = HIGH;
  else                  leftdir = LOW;

  if (rightstate < 0)   rightdir = HIGH;
  else                  rightdir = LOW;
 
  if (leftstate>0)      leftservo =  FULLBACK;
  else if (leftstate<0) leftservo =  FULLFRONT; 
  else                  leftservo =  SERVOOFF; 
  
  if (rightstate>0)      rightservo =  FULLFRONT; 
  else if (rightstate<0) rightservo =  FULLBACK; 
  else                   rightservo =  SERVOOFF; 
  
  lastLeverMs = millis(); //timestamp command
  sendplain(s);
  
  // move bot  or something
  Serial.printf("received %d %d %d %d \n",leftarm, rightarm, leftstate, rightstate); 

  updateServos();
}

void setup() 
{
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);

  /**
   * change the last number to your assigned number
  WiFi.config(IPAddress(192, 168, 1, 128),
              IPAddress(192, 168, 1, 1),
              IPAddress(255, 255, 255, 0));
  */
  WiFi.config(IPAddress(192, 168, 43, 128),
              IPAddress(192, 168, 43, 1),
              IPAddress(255, 255, 255, 0));
  while(WiFi.status()!= WL_CONNECTED ) { 
    delay(500); Serial.print("."); 
  }
  Serial.println("WiFi connected"); 
  Serial.printf("Use this URL http://%s/\n",WiFi.localIP().toString().c_str());
  server.begin();                  //Start server

 // Servo initialization
  ledcSetup(RIGHT_CHANNEL0, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
  ledcAttachPin(SERVOPIN1, RIGHT_CHANNEL0);
  ledcSetup(LEFT_CHANNEL1, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
  ledcAttachPin(SERVOPIN2, LEFT_CHANNEL1);

 // HTML510 initialization
  attachHandler("/joy?val=",handleJoy);
  attachHandler("/armup",handleArmup);
  attachHandler("/armdown",handleArmdown);
  attachHandler("/switchmode",handleSwitch);
  attachHandler("/lever?val=",handleLever);
  body = joybody;
  
  attachHandler("/favicon.ico",handleFavicon);
  attachHandler("/ ",handleRoot);
  
  pinMode(SERVODIR1, OUTPUT);
  pinMode(SERVODIR2, OUTPUT);
}

void loop()
{ 
  static long lastWebCheck = millis();
  static long lastServoUpdate = millis();
  uint32_t ms;

  ms = millis();
  if (ms-lastWebCheck > 2){ 
    serve(server,body);    
    lastWebCheck = ms;
  }
  if (ms-lastServoUpdate > 1000/SERVOFREQ) {
    updateServos();
    lastServoUpdate = ms;
  }
}
