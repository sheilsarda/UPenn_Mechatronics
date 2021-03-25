/*
  Blink LED with variable frequency

*/

// the number of the LED pin
const int ledPin1 = 21;  
const int ledPin2 = 10;

// setting PWM properties
const int freq = 4000;
const int ledChannel = 0;
const int resolution = 8;

int val = 0;  // variable to store the value read
int dutyCycle = 255;
 
void setup(){
  analogReadResolution(10);
  pinMode(4, INPUT);
  Serial.begin(115200);
  
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin1, ledChannel);
  ledcAttachPin(ledPin2, ledChannel);
}
 
void loop(){
  
  val       = analogRead(4);   
  dutyCycle = 255*((float) val/1024.0);
  Serial.print(analogRead(A0));
  Serial.print(" : ADC 10-bit \r\n");
  
  
  
  ledcWrite(ledChannel, dutyCycle);
  
}
