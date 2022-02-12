/* MEAM510-Lab4.1.1_simple
 * Simple Version (no SW debounce)
 *  By: LeeCS@seas.upenn.edu
 *  Date: 2021-11-03
 */

#define BUTTONPIN 14   // Define pin for external Switch
#define LEDPIN 15   // Define pin for external LED

void setup() {
  Serial.begin(115200); // open the serial port at 115200 bps:
  pinMode(BUTTONPIN, INPUT_PULLUP);  // Set pin to INPUT_PULLUP mode
  pinMode(LEDPIN, OUTPUT);  // Set pin to OUTPUT mode
  digitalWrite(LEDPIN,LOW); // Turn OFF LED to start
}

void loop(){
  if (digitalRead(BUTTONPIN)) {
    digitalWrite(LEDPIN,LOW);
  } else {
    digitalWrite(LEDPIN,HIGH);
  }
}
