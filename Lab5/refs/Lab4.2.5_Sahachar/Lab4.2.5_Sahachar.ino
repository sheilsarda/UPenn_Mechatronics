
/*
 * Lab 4.2.5 (modified - MEAM510 Lab 4 demo)
 * March 2021
 * Sahachar Reddy Tippana
 */

#include <WiFi.h>
#include "html510.h"
#include "joyJS.h"
#include "tankJS.h"

WiFiServer server(80);
const char* ssid     = "#TunnelShade";
const char* password = "reddy4you";
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
#define LEFT_CHANNEL1       1 // use 2nd channel of 16
#define SERVOPIN1    33       // PWM generating pin (33) for Right Motor
#define SERVOPIN2    32       // PWM generating pin (32) for left Motor
#define SERVOFREQ    60       // Frequency of the PWM
#define LEDC_RESOLUTION_BITS  16  //LEDC resolution in bits
#define LEDC_RESOLUTION  ((1<<LEDC_RESOLUTION_BITS)-1)  //LEDC resolution
//#define FULLBACK  //LEDC_RESOLUTION*10*60/10000
//#define SERVOOFF  LEDC_RESOLUTION*15*60/10000
//#define FULLFRONT  0 //LEDC_RESOLUTION*20*60/10000
#define leftpin 15            //Direction pin for left motor
#define rightpin 13           //Direction for right motor
#define NEUTRAL 0             //Variable storing the no spin condition of motor
#define MAX 50*100            //Variable storing the full forward spin condition of motor
#define REVERSE -50*100       //Variable storing the full backward spin condition of motor

uint32_t LMduty;              //Duty Cycle variable for the left motor
uint32_t RMduty;              //Duty Cycle variable for the right motor
int leftmotor, rightmotor;    //Variables determining the spin condition of motors



void updateServos() {
  if(leftmotor < 0)
  {
    digitalWrite(leftpin, LOW);
  }
  else
  {
    digitalWrite(leftpin, HIGH);
  }

  if(rightmotor < 0)
  {    
    digitalWrite(rightpin, LOW);
  }
  else
  {
    digitalWrite(rightpin, HIGH);
  }

  
  LMduty = map(abs(leftmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  RMduty = map(abs(rightmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
//  Serial.println("LMduty = %d", LMduty)
//  Serial.println("RMduty = %d", RMduty)
  ledcWrite(LEFT_CHANNEL1, LMduty);
  ledcWrite(RIGHT_CHANNEL0, RMduty); 
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

 float Left;     //Left motor movement variable
 float Right;    //Right motor movement variable

 float z = sqrt(x * x + y * y);   //hypotenuse
// Serial.println("z = %d", z);


 float rad = acos(abs(x) / z);    //Computing the unsigned angle in radians


 if (isnan(rad) == true)          //Cater for NaN values
 {
 rad = 0;
 }


 float angle = rad * 180 / PI;    //Computing the unsigned angle (0-90) in degrees

// Measuring turns with the angle of turn
 //For angles between 0-90, with angle 0 the turn co-efficient is -1, with angle 45 the co-efficient is 0, and with angle 90 it is 1
 //For a straight line path the turn coefficient will remain the same

float tcoeff = -1 + (angle / 90) * 2;     //turn co-efficient
 float turn = tcoeff * abs(abs(y) - abs(x));    //Variable determining the turn movement
 turn = round(turn * 100) / 100;


 float mov = max(abs(y), abs(x));      // Max of y or x is the movement

// First and third quadrant
 if ((x >= 0 && y <= 0) || (x < 0 && y > 0))
 {
 Left = mov; Right = turn;
 }
 else
 {
 Right = mov; Left = turn;
 }

// reverse spin direction condition
 if (y > 0) {
 Left = 0 - Left;
 Right = 0 - Right;
 }

 leftmotor = 100*Left;
 rightmotor = 100*Right;


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
 //  if (rightarm) do something?
 
  if (leftstate>0)      leftmotor =  REVERSE;
  else if (leftstate<0) leftmotor =  MAX; 
  else                  leftmotor =  NEUTRAL; 
  
  if (rightstate>0)      rightmotor =  REVERSE; 
  else if (rightstate<0) rightmotor =  MAX;
  else                   rightmotor =  NEUTRAL; 
  
  lastLeverMs = millis(); //timestamp command
  sendplain(s);
  Serial.printf("received %d %d %d %d \n",leftarm, rightarm, leftstate, rightstate); // move bot  or something
}

void setup() 
{
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192, 168, 43, 133), // change the last number to your assigned number
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
  pinMode(leftpin, OUTPUT); 
  pinMode(rightpin, OUTPUT); 

 // HTML510 initialization
  attachHandler("/joy?val=",handleJoy);
  attachHandler("/armup",handleArmup);
  attachHandler("/armdown",handleArmdown);
  attachHandler("/switchmode",handleSwitch);
  attachHandler("/lever?val=",handleLever);
  body = joybody;
  
  attachHandler("/favicon.ico",handleFavicon);
  attachHandler("/ ",handleRoot);
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
