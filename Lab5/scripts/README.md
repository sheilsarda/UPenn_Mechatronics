
## Walker's Wall Following code

| Follow State | Mode |
|--|--|
| Turn slight left | 1 |
| Back up and turn | 2 |
| Drive slight right | 3 |
| Drive slight left | 4 |
| Hard left turn | 5 |
| Drive slight left | 6 |
| Drive slight right | 6 |

````cpp
void handleWallFollow() {
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
  int delay_bounce = ??; //how long (ms) to turn left after the right button is pressed
  int delay_back = ??; //how long to back up (ms) after the front button is pressed
  int delay_left = ??; //how long to turn left (ms) after backing up
  
  /**
   * how long to turn slight left (ms) after turning hard left - 
   * this returns the robot to close to the wall
   */
  int delay_slight_left = ??; 

  Serial.printf("right: %d, front: %d, follow_state: %d \n", 
                right_button, front_button, follow_state);

  if (right_button) {
    follow_state = 1; // bounce
    last_right = millis();
    // turn slight left for delay_bounce ms
    //insert motor driving code here
  }

  if (front_button) {
    follow_state = 3; // back up and turn
    last_front = millis();
    //insert motor driving code here
  }

  
  if (follow_state == 0){ // DRIVE SLIGHT RIGHT
    //drive slight right
    //insert motor driving code here
  }

  if (follow_state == 1){ //DRIVE SLIGHT LEFT FOR delay_bounce MS
    if (ms2-last_right > delay_bounce){
      //turn slight right until hit the wall
      follow_state = 0;
    }
  }

  if (follow_state == 3){
    if (ms2 - last_front > delay_back){
      follow_state = 4; //hard left turn
      since_turning_left = millis();    
      //insert motor driving code here
    }
  }
  
  if (follow_state == 4){
    if (ms2-since_turning_left > delay_left){
      since_turning_slight_left = millis();
      follow_state = 5; // slight left
      //insert motor driving code here
    }
  }

  if (follow_state == 5){
    if (ms2 - since_turning_slight_left > delay_slight_left){
      follow_state = 0; //slight right
    }
  }

}
````