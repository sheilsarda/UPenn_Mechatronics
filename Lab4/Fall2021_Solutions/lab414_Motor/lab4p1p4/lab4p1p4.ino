/* MEAM510-Lab4.1.4
 * Web Motor Driver with Direction and Speed
 *  By: LeeCS@seas.upenn.edu
 *  Date: 2021-11-04
 */

#include "buttonJS.h"  // contains string "body" html code
#include "html510.h"
HTML510Server h(80);

#define PWM0 18   // VSPI CLK, Define OUTPUT pin for external PWM for Servo
#define PWM1 21   // VSPI HD, Define OUTPUT pin for external LED or PWM
#define DIR1 22   // VSPI WP, Define OUTPUT pin for external LED or PWM
#define PWM2 19   // VSPI Q, Define OUTPUT pin for external LED or PWM
#define DIR2 23   // VSPI D, Define OUTPUT pin for external LED or PWM

#define PWM3 25   // DAC_1 / ADC2_CH8, Define OUTPUT pin for external Motor
#define DIR3 26   // DAC_2 / ADC2_CH9, Define OUTPUT pin for external Motor
#define SPEED3 37 // ADC1_CH1, Define INPUT pin for IR Speed sensor
#define PWM4 27   // ADC2_CH7 / TOUCH7, Define OUTPUT pin for external Motor
#define DIR4 14   // ADC2_CH6 / TOUCH6 / SPI CLK, Define OUTPUT pin for external Motor

// const char* ssid     = "FiOS-MLA98";
// const char* password = "BAMBOO42";
const char* ssid     = "ZipIOT";
const char* password = "Intell1Chubby";

// setting PWM properties
const int freq = 50000;        // PWM Motors Frequency in Hz
const int ledChannel0 = 0;
const int ledChannel1 = 1;
const int ledChannel2 = 2;
const int ledChannel3 = 3;
const int ledChannel4 = 4;
const int resolution  = 8;   // PWM Duty Cycle Resolution (2^8)

const int minDutyCycle = 0;    // Lowest Duty Cycle for motor to be STOPPED
const int maxDutyCycle = 256;   // Max Duty Cycle for Max Speed
const int minSlidervalue = 1;   // Min Slider Value
const int maxSlidervalue = 100; // Max Slider Value

const int rpmTicks = 21;        // Nbr of ticks per revolution on speed sensor3

const int revSpeedDelta3 = 0;    // SpeedDelta Offset in Reverse for PWM3
volatile int lastDutyCycle3 = 0; // Save latest Duty Cycle for PWM3

/*****************/
/* web handler   */
void handleRoot() {
  h.sendhtml(body);
}

// Helper function below converts value in "old_range" to "new_range"
int old2New(int oldMin, int oldMax, int oldValue
          , int newMin, int newMax) {
  int oldRange = (oldMax-oldMin);
  int newRange = (newMax-newMin);

  // Handle any values that are out of range
  if (oldValue<oldMin)
    oldValue=oldMin;

  if (oldValue>oldMax)
      oldValue=oldMax;

  return (((oldValue-oldMin)*newRange)/oldRange)+newMin;
}


// Helper Function to set PWM Speed based on Direction
void setSpeed(int ledChannel, int dutyCycle, int directionPin) {
  // NOTE: L298N H-Bridge wants inverse of duty cycle when changing directions
  //       to get the equivalent speed going BACKWARDS vs. FORWARDS
  if (digitalRead(directionPin)) // if going BACKWARDS
    dutyCycle = maxDutyCycle-dutyCycle; // Inverse Duty Cycle

  // changing motor speed with PWM
  ledcWrite(ledChannel, dutyCycle);  // Set Duty Cycle of PWM
}

//-----------< PWM3_SLIDER HANDLERS >------------------------
void handleSlider(){
  int dutyCycle;  // Current Duty Cycle
  int sliderValue = h.getVal();   // Get Slider Value

  dutyCycle=old2New(minSlidervalue,maxSlidervalue,sliderValue
                   ,minDutyCycle,maxDutyCycle);
  lastDutyCycle3=dutyCycle; // Save latest dutyCycle
  setSpeed(ledChannel3,lastDutyCycle3,DIR3);  // Set PWM Speed

  // Website Report Status (DEBUG)
  String s = "Slider ";
  s = s+String(sliderValue);  // Add Slider Value to Website Status String
  s = s+", DutyCycle ";
  s = s+String(dutyCycle);  // Add Duty Cycle to Website Status String
  h.sendplain(s); // Send updated values back to website
}


//-----------< PWM3_DIR_BUTTON HANDLERS >------------------------
void handleH(){
  digitalWrite(DIR3, HIGH); // LED ON
  setSpeed(ledChannel3,lastDutyCycle3,DIR3);  // Set PWM Speed
  h.sendhtml(body); // acknowledge   
}

void handleL(){
  digitalWrite(DIR3, LOW);  // LED OFF
  setSpeed(ledChannel3,lastDutyCycle3,DIR3);  // Set PWM Speed
  h.sendhtml(body); // acknowledge   
}

void handleHit(){
  static int toggle;
  if (++toggle%2)  digitalWrite(DIR3,HIGH);
  else digitalWrite(DIR3,LOW);
  setSpeed(ledChannel3,lastDutyCycle3,DIR3);  // Set PWM Speed
  h.sendplain(""); // acknowledge         
}

void handleLEDstate(){
  String s = "PWM3 Direction state is "+String(digitalRead(DIR3));
  h.sendplain(s);
}

void setup() {
  // --------------< PWMs SETUP >--------------------------
  Serial.begin(115200); // open the serial port at 115200 bps:
  pinMode(DIR1, OUTPUT);  // Set pin to OUTPUT mode
  pinMode(DIR2, OUTPUT);  // Set pin to OUTPUT mode
  pinMode(DIR3, OUTPUT);  // Set pin to OUTPUT mode
  pinMode(DIR4, OUTPUT);  // Set pin to OUTPUT mode
  pinMode(PWM1, OUTPUT);  // Set pin to OUTPUT mode
  pinMode(PWM2, OUTPUT);  // Set pin to OUTPUT mode
  pinMode(PWM3, OUTPUT);  // Set pin to OUTPUT mode
  pinMode(PWM4, OUTPUT);  // Set pin to OUTPUT mode
  pinMode(PWM0, OUTPUT);  // Set pin to OUTPUT mode
//  digitalWrite(LEDPIN,LOW); // Turn OFF LED to start

  // configure LED PWM functionalitites
  //ledcSetup(ledChannel, freq, resolution);
  ledcSetup(ledChannel0, freq, resolution);
  ledcSetup(ledChannel1, freq, resolution);
  ledcSetup(ledChannel2, freq, resolution);
  ledcSetup(ledChannel3, freq, resolution);
  ledcSetup(ledChannel4, freq, resolution);

  // attach the channel to the GPIO to be controlled
  //ledcAttachPin(ledPin, ledChannel);
  ledcAttachPin(PWM1, ledChannel1);
  ledcAttachPin(PWM2, ledChannel2);
  ledcAttachPin(PWM3, ledChannel3);
  ledcAttachPin(PWM4, ledChannel4);
  ledcAttachPin(PWM0, ledChannel0);

  // --------------< WiFi SETUP >--------------------------
  WiFi.mode(WIFI_MODE_STA);   // WiFi in Station Mode
  WiFi.begin(ssid, password);
  // Define IP Address, Gateway, Subnet Mask (If commented out, it'll go DHCP)
  // WiFi.config(IPAddress(172, 16, 16, 6),IPAddress(172, 16, 16, 1),IPAddress(255, 255, 255, 0));

  while(WiFi.status()!= WL_CONNECTED ) {  // Wait for ESP32 to connect to WiFi
    delay(500);
  }

  h.begin();  // Begin hosting WebSite
  h.attachHandler("/ ",handleRoot);
  // PWM3_Button_Handlers
  h.attachHandler("/H",handleH);
  h.attachHandler("/L",handleL);
  h.attachHandler("/hit",handleHit);
  h.attachHandler("/LEDstate",handleLEDstate);
  // PWM1_Slider_Handler
  h.attachHandler("/slider?val=",handleSlider);
}

void loop(){
  h.serve();
  delay(10);
}
