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
const char *ssid = "#Skyroam_1t9";
const char *password = "55687127";
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

#define RIGHT_CHANNEL0 0 // use first channel of 16
#define LEFT_CHANNEL1 1
#define SERVOPIN1 33
#define SERVOPIN2 32
#define SERVOFREQ 60
#define LEDC_RESOLUTION_BITS 12
#define LEDC_RESOLUTION ((1 << LEDC_RESOLUTION_BITS) - 1)
#define FULLBACK LEDC_RESOLUTION * 10 * 60 / 10000
#define SERVOOFF LEDC_RESOLUTION * 15 * 60 / 10000
#define FULLFRONT LEDC_RESOLUTION * 20 * 60 / 10000

int leftservo, rightservo;
int oldleftservo, oldrightservo;
int auto_state = 0;
int follow_state = 0;

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255)
{
  uint32_t duty = LEDC_RESOLUTION * min(value, valueMax) / valueMax;
  ledcWrite(channel, duty); // write duty to LEDC
}

void updateServos()
{
  ledcAnalogWrite(LEFT_CHANNEL1, leftservo, LEDC_RESOLUTION);
  ledcAnalogWrite(RIGHT_CHANNEL0, rightservo, LEDC_RESOLUTION);
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
