#include <WiFi.h>
#include "html510.h"
#include "joyJS.h"
#include "tankJS.h"

WiFiServer server(80);
const char *ssid = "TunnelShade";
const char *password = "reddy4you";
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

#define RIGHT_CHANNEL0 0                                  // use first channel of 16
#define LEFT_CHANNEL1 1                                   // use 2nd channel of 16
#define rRIGHT_CHANNEL2 2                                 // use first channel of 16
#define rLEFT_CHANNEL3 3                                  // use 2nd channel of 16
#define SERVOPIN1 33                                      // PWM generating pin (33) for Right Motor
#define SERVOPIN2 32                                      // PWM generating pin (32) for left Motor
#define SERVOPIN3 14                                      // PWM generating pin (34) for Rear Right Motor
#define SERVOPIN4 27                                      // PWM generating pin (35) for Rear left Motor
#define SERVOFREQ 60                                      // Frequency of the PWM
#define LEDC_RESOLUTION_BITS 14                           //LEDC resolution in bits
#define LEDC_RESOLUTION ((1 << LEDC_RESOLUTION_BITS) - 1) //LEDC resolution
//#define FULLBACK  //LEDC_RESOLUTION*10*60/10000
//#define SERVOOFF  LEDC_RESOLUTION*15*60/10000
//#define FULLFRONT  0 //LEDC_RESOLUTION*20*60/10000
#define leftpin 15        //Direction pin for left motor
#define rightpin 13       //Direction for right motor
#define rleftpin 10       //Direction pin for rear left motor
#define rrightpin 9       //Direction for rear right motor
#define NEUTRAL 0         //Variable storing the no spin condition of motor
#define MAX 50 * 100      //Variable storing the full forward spin condition of motor
#define REVERSE -50 * 100 //Variable storing the full backward spin condition of motor

uint32_t LMduty;  //Duty Cycle variable for the left motor
uint32_t RMduty;  //Duty Cycle variable for the right motor
uint32_t rLMduty; //Duty Cycle variable for the REAR left motor
uint32_t rRMduty; //Duty Cycle variable for the REAR right motor
//AUTO
uint32_t lmduty;  //Duty Cycle variable for the left motor
uint32_t rmduty;  //Duty Cycle variable for the right motor
uint32_t rlmduty; //Duty Cycle variable for the REAR left motor
uint32_t rrmduty; //Duty Cycle variable for the REAR right motor

int leftmotor, rightmotor, rleftmotor, rrightmotor; //Variables determining the spin condition of motors
int smallduty = map(abs(4000), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
int fullduty = map(abs(5000), NEUTRAL, MAX, 0, LEDC_RESOLUTION);

int follow_state, auto_state;

void updateServos()
{
  if (leftmotor < 0)
  {
    digitalWrite(leftpin, LOW);
  }
  else
  {
    digitalWrite(leftpin, HIGH);
  }

  if (rightmotor < 0)
  {
    digitalWrite(rightpin, LOW);
  }
  else
  {
    digitalWrite(rightpin, HIGH);
  }

  //Rear Motors
  if (rleftmotor < 0)
  {
    digitalWrite(rleftpin, LOW);
  }
  else
  {
    digitalWrite(rleftpin, HIGH);
  }

  if (rrightmotor < 0)
  {
    digitalWrite(rrightpin, LOW);
  }
  else
  {
    digitalWrite(rrightpin, HIGH);
  }

  LMduty = map(abs(leftmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  RMduty = map(abs(rightmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  rLMduty = map(abs(rleftmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  rRMduty = map(abs(rrightmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  //  Serial.printf("LMduty = %d /n", LMduty);
  //  Serial.printf("RMduty = %d /n", RMduty);
  //  Serial.printf("rLMduty = %d /n", rLMduty);
  //  Serial.printf("rRMduty = %d /n", rRMduty);
  ledcWrite(LEFT_CHANNEL1, LMduty);
  ledcWrite(RIGHT_CHANNEL0, RMduty);
  //Rear Motors
  ledcWrite(rLEFT_CHANNEL3, rLMduty);
  ledcWrite(rRIGHT_CHANNEL2, rRMduty);
}

void AutoServos(int lm, int rm, int rlm, int rrm)
{
  if (lm < 0)
  {
    digitalWrite(leftpin, LOW);
  }
  else
  {
    digitalWrite(leftpin, HIGH);
  }

  if (rm < 0)
  {
    digitalWrite(rightpin, LOW);
  }
  else
  {
    digitalWrite(rightpin, HIGH);
  }

  //Rear Motors
  if (rlm < 0)
  {
    digitalWrite(rleftpin, LOW);
  }
  else
  {
    digitalWrite(rleftpin, HIGH);
  }

  if (rrm < 0)
  {
    digitalWrite(rrightpin, LOW);
  }
  else
  {
    digitalWrite(rrightpin, HIGH);
  }

  lmduty = map(abs(lm), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  rmduty = map(abs(rm), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  rlmduty = map(abs(rlm), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  rrmduty = map(abs(rrm), NEUTRAL, MAX, 0, LEDC_RESOLUTION);

  ledcWrite(LEFT_CHANNEL1, lmduty);
  ledcWrite(RIGHT_CHANNEL0, rmduty);
  ledcWrite(rLEFT_CHANNEL3, rlmduty);
  ledcWrite(rRIGHT_CHANNEL2, rrmduty);
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

  float Left;   //Left motor movement variable
  float Right;  //Right motor movement variable
  float rLeft;  //Rear Left motor movement variable
  float rRight; //Rear Right motor movement variable

  float z = sqrt(x * x + y * y); //hypotenuse
                                 // Serial.println("z = %d", z);

  float rad = acos(abs(x) / z); //Computing the unsigned angle in radians

  if (isnan(rad) == true) //Cater for NaN values
  {
    rad = 0;
  }

  float angle = rad * 180 / PI; //Computing the unsigned angle (0-90) in degrees

  // Measuring turns with the angle of turn
  //For angles between 0-90, with angle 0 the turn co-efficient is -1, with angle 45 the co-efficient is 0, and with angle 90 it is 1
  //For a straight line path the turn coefficient will remain the same

  float tcoeff = -1 + (angle / 90) * 2;       //turn co-efficient
  float turn = tcoeff * abs(abs(y) - abs(x)); //Variable determining the turn movement
  turn = round(turn * 100) / 100;

  float mov = max(abs(y), abs(x)); // Max of y or x is the movement

  // First and third quadrant
  if ((x >= 0 && y <= 0) || (x < 0 && y > 0))
  {
    Left = mov;
    Right = turn;
    rLeft = turn;
    rRight = mov;
  }
  else
  {
    Right = mov;
    Left = turn;
    rLeft = mov;
    rRight = turn;
  }

  // reverse spin direction condition
  if (y > 0)
  {
    Left = 0 - Left;
    Right = 0 - Right;
    rLeft = 0 - rLeft;
    rRight = 0 - rRight;
  }

  leftmotor = -100 * Left;
  rightmotor = -100 * Right;
  rleftmotor = 100 * rLeft;
  rrightmotor = 100 * rRight;

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

void handleclockwise()
{
  // do something?
  Serial.println("Rotating Clockwise");

  float mag = 40;

  leftmotor = -100 * mag;
  rightmotor = 100 * mag;
  rleftmotor = 100 * mag;
  rrightmotor = -100 * mag;

  //  int lmotor = -100*mag;
  //  int rmotor = 100*mag;
  //  int rlmotor = -100*mag;
  //  int rrmotor = 100*mag;
  //  lmotor = map(abs(lmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  //  rmotor = map(abs(rmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  //  rlmotor = map(abs(rlmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  //  rrmotor = map(abs(rrmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
  ////  Serial.printf("LMduty = %d /n", lmotor);
  ////  Serial.printf("RMduty = %d /n", rmotor);
  ////  Serial.printf("rLMduty = %d /n", rlmotor);
  ////  Serial.printf("rRMduty = %d /n", rrmotor);
  //
  //  ledcWrite(LEFT_CHANNEL1, lmotor);
  //  ledcWrite(RIGHT_CHANNEL0, rmotor);
  ////Rear Motors
  //  ledcWrite(rLEFT_CHANNEL3, rlmotor);
  //  ledcWrite(rRIGHT_CHANNEL2, rrmotor);
  //  delay (2000/SERVOFREQ);
  //  ledcWrite(LEFT_CHANNEL1, 0);
  //  ledcWrite(RIGHT_CHANNEL0, 0);
  ////Rear Motors
  //  ledcWrite(rLEFT_CHANNEL3, 0);
  //  ledcWrite(rRIGHT_CHANNEL2, 0);

  sendplain(""); //acknowledge
}

void handleanticlockwise()
{
  // do something?
  Serial.println("Rotating anticlockwise");

  float mag = 40;
  leftmotor = 100 * mag;
  rightmotor = -100 * mag;
  rleftmotor = -100 * mag;
  rrightmotor = 100 * mag;

  sendplain(""); //acknowledge
}

void handleWallFollow()
{
  static long last_right_bump = millis();
  static long last_front_bump = millis();
  static long last_left_bump = millis();
  static long last_back_bump = millis();

  static long since_turning_left = millis();
  static long since_turning_slight_left = millis();
  uint32_t ms2;

  ms2 = millis();

  //handle button states
  //may want to do this with interrupts
  int rightf = digitalRead(25);
  int rightb = digitalRead(26);
  int frontr = digitalRead(21);
  int frontl = digitalRead(04);
  int leftf = digitalRead(2);
  int leftb = digitalRead(12);
  int backr = digitalRead(38);
  int backl = digitalRead(37);

  int delay_bounce = 1000; //how long (ms) to turn left after the right button is pressed
  // int delay_back = 1000; //how long to back up (ms) after the front button is pressed
  // int delay_left = 1000; //how long to turn left (ms) after backing up
  // int delay_slight_left = 1000; //how long to turn slight left (ms) after turning hard left - this returns the robot to close to the wall
//
//   Serial.printf("rightf: %d, rightb: %d, leftf: %d, leftb: %d, follow_state: %d \n", rightf, rightb, leftf, leftb, follow_state);
   Serial.printf("rightf: %d, rightb: %d, leftf: %d, leftb: %d, \n", rightf, rightb, leftf, leftb);

  //Right Bump
  if (rightf && rightb)
  {
    //move left for 5cm equivalent of time = delay_bounce
    AutoServos(4000, -4000, 4000, -4000);
    delay(delay_bounce);
    if (last_front_bump > last_back_bump)
    {
      follow_state = 3; // move backwards
    }
    else
      follow_state = 1;

    last_right_bump = millis();
//    Serial.printf("rightboth");
  }
  else if (rightf || rightb)
  {
    //move right
    AutoServos(-4000, 4000, -4000, 4000);
    //    leftmotor = -100*40;
    //    rightmotor = 100*40;
    //    rleftmotor = -100*40;
    //    rrightmotor = 100*40;
    Serial.printf("rightf");
  }

  //Front Bump
  if (frontr && frontl)
  {
    //move back for 10cm equivalent of time = 2*delay_bounce
    AutoServos(4000, 4000, -4000, -4000);
    delay(delay_bounce);
    if (last_left_bump > last_right_bump)
    {
      follow_state = 0; // move right
    }
    else
      follow_state = 2; // move left
    last_front_bump = millis();
  }

  else if (frontr || frontl)
  {
    //move front
    AutoServos(-4000, -4000, 4000, 4000);
  }

  //Left Bump
  if (leftf && leftb)
  {
    //move right for 5cm equivalent of time = delay_bounce
    AutoServos(-4000, 4000, -4000, 4000);
    delay(delay_bounce);

    if (last_front_bump > last_back_bump)
    {
      follow_state = 3; // move backwards
    }
    else
      follow_state = 1;

    last_left_bump = millis();
  }
  else if (leftf || leftb)
  {
    //move left
    AutoServos(4000, -4000, 4000, -4000);
  }

  //Back Bump
  if (backr && backl)
  {
    //move front for 10cm equivalent of time = 2*delay_bounce
    AutoServos(-4000, -4000, 4000, 4000);
    delay(2 * delay_bounce);
    if (last_left_bump > last_right_bump)
    {
      follow_state = 0; // move right
    }
    else
      follow_state = 2;
    last_back_bump = millis();
  }
  else if (backr || backl)
  {
    //move back
    AutoServos(4000, 4000, -4000, -4000);
  }

  //Followstates

  if (follow_state == 0)
  { // Default, align with the right side wall
    //move right
    AutoServos(-5000, 5000, -5000, 5000);
    Serial.printf("follow_state: %d, \n", follow_state);
  }

  if (follow_state == 1)
  {
    //move front
    AutoServos(-5000, -5000, 5000, 5000);
    Serial.printf("follow_state: %d, \n", follow_state);
  }

  if (follow_state == 2)
  {
    //move left
    AutoServos(5000, -5000, 5000, -5000);
    Serial.printf("follow_state: %d, \n", follow_state);
  }

  if (follow_state == 3)
  {
    //move back
    AutoServos(5000, 5000, -5000, -5000);
    Serial.printf("follow_state: %d, \n", follow_state);
  }

  // if (follow_state == 4){
  //   if (ms2 - since_turning_slight_left > delay_slight_left){
  //     follow_state = 0; //slight right
  //   }
  }

  /*********************/
  /* tank mode  code  */
  int leftstate, rightstate;
  long lastLeverMs;

  void handleLever(){
    leftarm = getVal();
    rightarm = getVal();
    leftstate = getVal();
    rightstate = getVal();
    String s = String(leftarm) + "," + String(rightarm) + "," +
               String(leftstate) + "," + String(rightstate);

    //  if (leftarm) do something?
    //  if (rightarm) do something?

    if (leftstate > 0)
      leftmotor = REVERSE;
    else if (leftstate < 0)
      leftmotor = MAX;
    else
      leftmotor = NEUTRAL;

    if (rightstate > 0)
      rightmotor = REVERSE;
    else if (rightstate < 0)
      rightmotor = MAX;
    else
      rightmotor = NEUTRAL;

    lastLeverMs = millis(); //timestamp command
    sendplain(s);
    // Serial.printf("received %d %d %d %d \n", leftarm, rightarm, leftstate, rightstate); // move bot  or something
  }

  void setup()
  {
    Serial.begin(115200);
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    WiFi.config(IPAddress(192, 168, 1, 133), // change the last number to your assigned number
                IPAddress(192, 168, 1, 1),
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
    ledcSetup(rRIGHT_CHANNEL2, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
    ledcAttachPin(SERVOPIN3, rRIGHT_CHANNEL2);
    ledcSetup(rLEFT_CHANNEL3, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
    ledcAttachPin(SERVOPIN4, rLEFT_CHANNEL3);

    pinMode(leftpin, OUTPUT);
    pinMode(rightpin, OUTPUT);
    pinMode(rleftpin, OUTPUT);
    pinMode(rrightpin, OUTPUT);

    //Limit Switch Initialization
    pinMode(25, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);
    pinMode(21, INPUT_PULLUP);
    pinMode(04, INPUT_PULLUP);
    pinMode(02, INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);
    pinMode(38, INPUT);
    pinMode(37, INPUT);

    // HTML510 initialization
    attachHandler("/joy?val=", handleJoy);
    attachHandler("/clockwise", handleclockwise);
    attachHandler("/anticlockwise", handleanticlockwise);
    attachHandler("/armup", handleArmup);
    attachHandler("/armdown", handleArmdown);
    attachHandler("/switchmode", handleSwitch);
    attachHandler("/lever?val=", handleLever);
    body = joybody;

    attachHandler("/favicon.ico", handleFavicon);
    attachHandler("/ ", handleRoot);

    follow_state = 0;
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
      {//IN WALL FOLLOW MODE
      handleWallFollow();
      }
      else {
        follow_state = 0;
        updateServos();
      }
    lastServoUpdate = ms;
    }
  }
