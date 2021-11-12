/*
 * MEAM510 lab 4.1.3 LED via wifi with freq and duty cycle sliders
 * Ethan Donlon
 * University of Pennsylvania
 */

#include "sliderJS.h"  // contains string "body" html code
#include "html510.h"
HTML510Server h(80);

//set up LEDC
#define LEDC_CHANNEL       0
#define LEDC_RESOLUTION_BITS  20
#define LEDC_RESOLUTION  ((1<<LEDC_RESOLUTION_BITS)-1) 
#define LED_PIN           21

//set up wifi variables
IPAddress myIPaddress(192,168,1,120);
const char* ssid     = "Walrus";

//set global variables
uint32_t duty=50; //start duty at 50

//does analog write
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 100) {            
  uint32_t booty =  LEDC_RESOLUTION * min(value, valueMax) / valueMax;   
  ledcWrite(channel, booty);  // write duty to LEDC
}

/*****************/
/* web handler   */
void handleRoot() {
  h.sendhtml(body);
}

//get duty from slider and write to LED_PIN
void handleDuty(){
  duty = h.getVal();
  String s = "Duty Cycle is " + String(duty);
  ledcAnalogWrite(LEDC_CHANNEL,duty); 
  h.sendplain(s);
}

//get frequency from slider and write to LED_PIN
void handleFreq(){
  double freq = double(h.getVal())/10.0;
  String s = "Frequency is " + String(freq);
  ledcSetup(LEDC_CHANNEL, freq, LEDC_RESOLUTION_BITS); //when frequency is changed, reset LEDC setup
  ledcAnalogWrite(LEDC_CHANNEL,duty); //make sure to re-output duty
  h.sendplain(s);
}

void setup() { 
  
  //setup wifi
  WiFi.mode(WIFI_AP);    
  WiFi.softAP(ssid);
  delay(100);
  WiFi.softAPConfig(IPAddress(192, 168, 1, 120),  IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0)); 
  
  //setup ledc
  ledcSetup(LEDC_CHANNEL, freq, LEDC_RESOLUTION_BITS);//start it at 2 Hz
  ledcAttachPin(LED_PIN, LEDC_CHANNEL);
  pinMode(LED_PIN,OUTPUT);
  
  //setup dutycycle and frequency slider handles
  h.begin();
  h.attachHandler("/ ",handleRoot);
  h.attachHandler("/freq?val=",handleFreq);
  h.attachHandler("/duty?val=",handleDuty);
  
  
}

void loop(){
  //continuously call handle functions
  h.serve();
  delay(10);
}
