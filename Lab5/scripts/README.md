# Lab 5 Scripts

## Walker's Wall Following code

### Wall Following Modes

| Follow State | Mode | Next Mode |
|--|--|--|
| Drive slight right and bang the wall, then go slightly left | 0 | 0 |
| Drive slight left, then go slight right until you hit the wall | 1 | 0 |
| Back up, then turn slightly left | 3 | 4 |
| Drive slight left | 4 | 5 |
| Slight right turn | 5 | 0 |

### Finite State Machine Skeleton

````cpp
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
````
