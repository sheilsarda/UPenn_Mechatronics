/*
 * MEAM510 Lab 4 demo
 * March 2021
 * 
 * Mark Yim
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
void handleFavicon()
{
  sendplain(""); // acknowledge
}

void handleRoot()
{
  sendhtml(body);
}

void handleSwitch()
{ // Switch between JOYSTICK and TANK mode
  String s = "";
  static int toggle = 0;
  if (toggle)
    body = joybody;
  else
    body = tankbody;
  toggle = !toggle;
  sendplain(s); //acknowledge
}

#define RIGHT_CHANNEL0      0 // use first channel of 16  
#define LEFT_CHANNEL1       1
#define SERVOPIN1    32
#define DIR_PIN1     21
#define SERVOPIN2    33
#define DIR_PIN2     22
#define SERVOFREQ    60
#define LEDC_RESOLUTION_BITS  12
#define LEDC_RESOLUTION  ((1<<LEDC_RESOLUTION_BITS)-1) 
#define FULLBACK LEDC_RESOLUTION*1
#define SERVOOFF  LEDC_RESOLUTION*0
#define FULLFRONT  LEDC_RESOLUTION*1

double  leftservo, rightservo; 
int left_dir, right_dir;

int oldleftservo, oldrightservo;
int auto_state = 0;
int follow_state = 0;


void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 4095) {            
  uint32_t duty =  LEDC_RESOLUTION * min(value, valueMax) / valueMax;   
  ledcWrite(channel, duty);  // write duty to LEDC 
}

void updateServos() {
  ledcAnalogWrite(LEFT_CHANNEL1, leftservo, LEDC_RESOLUTION);
  digitalWrite(DIR_PIN1,left_dir);
  
  ledcAnalogWrite(RIGHT_CHANNEL0, rightservo, LEDC_RESOLUTION); 
  digitalWrite(DIR_PIN2,right_dir);
}

int trigpin_right=34;
int echopin_right=35;

int trigpin_front=9;
int echopin_front=10;


int movingavg(int newvalue,int WEIGHT) {
int update;
static int lastvalue;
update = (WEIGHT*lastvalue + newvalue)/(WEIGHT+1);
lastvalue = update;
return update;
}

long right_range(int trigpin_right, int echopin_right) { 
  digitalWrite(trigpin_right,LOW);
  delayMicroseconds(2);

  digitalWrite(trigpin_right,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin_right,LOW);

  long duration, range_cm, range_cm_filtered;
  duration=pulseIn(echopin_right,HIGH);
  range_cm=duration*0.017;
  range_cm_filtered=movingavg(range_cm,1);
  return range_cm_filtered; 
}

long front_range(int trigpin_front, int echopin_front) { 
  digitalWrite(trigpin_front,LOW);
  delayMicroseconds(2);

  digitalWrite(trigpin_front,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin_front,LOW);

  long duration, range_cm, range_cm_filtered;
  duration=pulseIn(echopin_front,HIGH);
  range_cm=duration*0.017;
  range_cm_filtered=movingavg(range_cm,1);
  return range_cm_filtered; 
}

void control(int desired_front, int measured_front, int desired_right, int measured_right ) {
  double kp=10;
  //double ki=0.01;
  //static int sumederror;
  
  double error_front=desired_front-measured_front;
  double error_right=desired_right-measured_right;
  //summederror=summederror+error;
  //if(error==0)summederror=0;
  
  int u_front = kp*error_front;
  int u_right = kp*error_right;
 
  if(u_front>0) follow_state=2; //back up
  else if(u_front<0) follow_state=1; //go straight

  if(u_right>0) follow_state=3; //turn slight left
  else if(u_right<0) follow_state=4; //turn slight right
  else follow_state=2; //go straight
}

void handleWallFollow() {
  static long last_right = millis();
  static long last_front = millis();
  static long since_turning_left = millis();
  static long since_turning_slight_left = millis();
  uint32_t ms2;
 

  ms2 = millis();
  
  //handle button states
  //may want to do this with interrupts
  long right_sensor = right_range(trigpin_right,echopin_right);
  long front_sensor = front_range(trigpin_front,echopin_front);
  int delay_bounce = 500; //how long (ms) to turn left after the right button is pressed
  int delay_back = 500; //how long to back up (ms) after the front button is pressed
  int delay_left = 500; //how long to turn left (ms) after backing up
  int delay_slight_left = 500; //how long to turn slight left (ms) after turning hard left - this returns the robot to close to the wall

  control(20,front_sensor,20,right_sensor); //controls and sets certain follow states

  Serial.printf("right: %d, front: %d, follow_state: %d \n", right_sensor, front_sensor, follow_state);


  if (follow_state == 0){ // Do nothing
    leftservo=SERVOOFF;
    rightservo=SERVOOFF;
  }

  if (follow_state == 1){ //Drive straight

    leftservo=FULLFRONT;
    left_dir=HIGH;
    rightservo=FULLFRONT;
    right_dir=LOW;
  }

  if (follow_state == 2) { //Drive backwards
    if(ms2 - last_front <= delay_back){

      leftservo=FULLBACK;
      left_dir=LOW;
      rightservo=FULLBACK;
      right_dir=HIGH;
    
    } else  follow_state=5;

   }
  
  if ((follow_state == 3) && (ms2-last_right <= delay_bounce)) { //slight left

     
      //insert motor driving code here
    leftservo=0.7*FULLFRONT;
    left_dir=HIGH;
    rightservo=FULLFRONT;
    right_dir=LOW;
  }

  if (follow_state == 4){ //slight right
    /*if (ms2 - since_turning_slight_left > delay_slight_left){
      follow_state = 0; //slight right
    }*/
    leftservo=FULLFRONT;
    left_dir=HIGH;
    rightservo=0.7*FULLFRONT;
    right_dir=LOW;
  }

  if ((follow_state == 5) && (ms2-since_turning_left > delay_left)) { //slight left

     
      //insert motor driving code here
    leftservo=FULLBACK;
    left_dir=LOW;
    rightservo=FULLFRONT;
    right_dir=LOW;
  }
  
}

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(trigpin_right,OUTPUT);
pinMode(echopin_right,INPUT);
pinMode(trigpin_front,OUTPUT);
pinMode(echopin_front,INPUT);

// Servo initialization
ledcSetup(RIGHT_CHANNEL0, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
ledcAttachPin(SERVOPIN1, RIGHT_CHANNEL0);
ledcSetup(LEFT_CHANNEL1, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
ledcAttachPin(SERVOPIN2, LEFT_CHANNEL1);
}

void loop() {
  // put your main code here, to run repeatedly:
  long range=right_range(trigpin_right,echopin_right);
  Serial.println(range);
}



/************************/
/* joystick mode  code  */

int leftarm, rightarm;
int x, y;

void handleJoy()
{
  int left, right;
  x = getVal(); // from -50 to +50
  y = getVal();
  String s = String(x) + "," + String(y);

  left = x - y;
  right = x + y;

  leftservo = map(left, -50, 50, FULLBACK, FULLFRONT);
  rightservo = map(right, -50, 50, FULLBACK, FULLFRONT);

  sendplain(s);
  Serial.printf("received X,Y:=%d,%d\n", x, y);
}

void handleArmdown()
{
  // do something?
  Serial.println("LEAVING WALL FOLLOW MODE");
  auto_state = 0;
  sendplain(""); //acknowledge
}

void handleArmup()
{
  // do something?
  Serial.println("IN WALL FOLLOW MODE");
  auto_state = 1;
  sendplain(""); //acknowledge
}

void handleWallFollow()
{
  static long last_right = millis();
  static long last_front = millis();
  static long since_turning_left = millis();
  static long since_turning_slight_left = millis();
  uint32_t ms2;

  ms2 = millis();

  //handle button states
  //may want to do this with interrupts
  int right_button = digitalRead(22);
  int front_button = digitalRead(23);
  int delay_bounce = ? ? ;      //how long (ms) to turn left after the right button is pressed
  int delay_back = ? ? ;        //how long to back up (ms) after the front button is pressed
  int delay_left = ?      ? ;   //how long to turn left (ms) after backing up
  int delay_slight_left = ? ? ; //how long to turn slight left (ms) after turning hard left - this returns the robot to close to the wall

  Serial.printf("right: %d, front: %d, follow_state: %d \n", right_button, front_button, follow_state);

  // Which Limit Switch is Pressed -> Front or Right

  if (right_button)
  {
    // turn slight left for delay_bounce ms
    follow_state = 1; // bounce
    last_right = millis();
  }

  if (front_button)
  {
    // back up and turn
    follow_state = 3;
    last_front = millis();
  }

  /** 
   * Motor Driving Code
   */

  if (follow_state == 0)
  { // DRIVE SLIGHT RIGHT
    //drive slight right
    //insert motor driving code here
  }

  if (follow_state == 1)
  { //DRIVE SLIGHT LEFT FOR delay_bounce MS
    if (ms2 - last_right > delay_bounce)
    {
      //turn slight right until hit the wall
      follow_state = 0;
    }
  }

  if (follow_state == 3)
  {
    if (ms2 - last_front > delay_back)
    {
      //hard left turn
      follow_state = 4;
      since_turning_left = millis();
      //insert motor driving code here
    }
  }

  if (follow_state == 4)
  {
    if (ms2 - since_turning_left > delay_left)
    {
      // slight left
      since_turning_slight_left = millis();
      follow_state = 5;
      //insert motor driving code here
    }
  }

  if (follow_state == 5)
  {
    if (ms2 - since_turning_slight_left > delay_slight_left)
    {
      //slight right
      follow_state = 0;
    }
  }
}

/*********************/
/* tank mode  code  */
int leftstate, rightstate;
long lastLeverMs;

void handleLever()
{
  leftarm = getVal();
  rightarm = getVal();
  leftstate = getVal();
  rightstate = getVal();
  String s = String(leftarm) + "," + String(rightarm) + "," +
             String(leftstate) + "," + String(rightstate);

  //  if (leftarm) do something?
  //  if (rightarm) do something?

  if (leftstate > 0)
    leftservo = FULLBACK;
  else if (leftstate < 0)
    leftservo = FULLFRONT;
  else
    leftservo = SERVOOFF;

  if (rightstate > 0)
    rightservo = FULLFRONT;
  else if (rightstate < 0)
    rightservo = FULLBACK;
  else
    rightservo = SERVOOFF;

  lastLeverMs = millis(); //timestamp command
  sendplain(s);
  Serial.printf("received %d %d %d %d \n", leftarm, rightarm, leftstate, rightstate); // move bot  or something
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192, 168, 43, 9), // change the last number to your assigned number
              IPAddress(192, 168, 43, 1),
              IPAddress(255, 255, 255, 0));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.printf("Use this URL http://%s/\n", WiFi.localIP().toString().c_str());
  server.begin(); //Start server

  // Servo initialization
  ledcSetup(RIGHT_CHANNEL0, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
  ledcAttachPin(SERVOPIN1, RIGHT_CHANNEL0);
  ledcSetup(LEFT_CHANNEL1, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
  ledcAttachPin(SERVOPIN2, LEFT_CHANNEL1);

  // HTML510 initialization
  attachHandler("/joy?val=", handleJoy);
  attachHandler("/armup", handleArmup);
  attachHandler("/armdown", handleArmdown);
  attachHandler("/switchmode", handleSwitch);
  attachHandler("/lever?val=", handleLever);
  body = joybody;

  attachHandler("/favicon.ico", handleFavicon);
  attachHandler("/ ", handleRoot);

  // Limit switch initialization
  pinMode(23, INPUT_PULLUP); // FRONT
  pinMode(22, INPUT_PULLUP); // RIGHT
  pinMode(21, INPUT_PULLUP);
}

void loop()
{
  static long lastWebCheck = millis();
  static long lastServoUpdate = millis();
  uint32_t ms;

  ms = millis();
  if (ms - lastWebCheck > 2)
  {
    serve(server, body);
    lastWebCheck = ms;
  }
  if (ms - lastServoUpdate > 1000 / SERVOFREQ)
  {
    if (auto_state == 1)
    { //IN WALL FOLLOW MODE
      handleWallFollow();
    }
    updateServos();
    lastServoUpdate = ms;
  }
}
