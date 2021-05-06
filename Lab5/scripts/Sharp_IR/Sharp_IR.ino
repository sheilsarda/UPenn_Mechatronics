#define ir A0


void setup() {

  pinMode (ir, INPUT);

  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {

  int dis = analogRead(ir);
  double distanceCM = 27.728 * pow(map(dis, 0, 1023, 0, 5000)/1000.0, -1.2045);

  Serial.print("Distance: ");  // returns it to the serial monitor
  Serial.println(distanceCM);
  delay(500);   
}
