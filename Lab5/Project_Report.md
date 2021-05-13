# Lab 5 - Final Project Report

Sheil Sarda <sheils@seas.upenn.edu>

## Functionality
### Pictures of Robot

![Robot](imgs/robot_top.jpg)
![Robot](imgs/robot_front.jpg)
![Robot](imgs/robot_right1.jpg)
![Robot](imgs/robot_left.jpg)

### Strategy for the Game (Minimal Functionality)

#### *Can Grabbing*

My approach to can grabbing evolved over the course of the competition from using gripper arms to pushing cans and beacons.

Approach 1: **Gripper Arms**:


- Design approved prior for manufacturing used 2 servo motors with arms to lock in the can 
- Main challenge with this design was that servo arms did not have enough torque to keep the can locked during movement
- Also the servo horn did not stay locked on to the servo even with the aid of hot glue

Approach 2: **Can Pushing**:

- With holonomic drive the robot can easily navigate around the beacon and push it into the doubling zone
- Only drawback with this approach is the robot cannot pull in cans or beacons that are located near the edge of the map because it doesn't have the space to nudge itself between the wall and can 
- Lost a few matches because robot pushed the beacon into the wall and was unable to move it closer to the center
- Partnership with Rafael for the playoff games allowed me to focus on just pushing the beacon into the doubling zone and then venture out in the opposing side to get time points, so did not have to worry about getting cans from the periphery of the map since his nimble robot was able to fetch all the cans

#### *Autonomous Modes*

- Left, Right and Front sensor ranging data was available on my dashboard to indicate where obstacles were
- Could leverage wall-following behavior to drive myself to the opposite side of the map and wait there to gain points

### Extra Components / Functionality

- Using a short-range Time-of-Flight (ToF) sensor to autonomously align the robot's gripper arms to the can
- ToF range was < 10cm so it would not be sufficient to also do wall-following
- Use the Sharp IR sensor also located on that side of the robot to maintain a preset distance from the wall on the robot's left and detect cans in an interrupt-driven manner
- Once can is detected, minimize the distance that the ToF is reading, indicating that the can has been pushed into reach of the grippers
- Close the grippers, turn 180 degrees, and use wall following to arrive back on the home side of the field

*BOM in Appendix*

## Mechanical Design

### Lessons for improvement

- Sensor mounts were too high for the walls of the field, so had to drill new mounting holes in the front, right and left plates to mount the ultrasonic and Sharp IR sensors lower
- Cutout for picking up the can was too small, so it was hard to precisely align the robot to pick up the can just through visual feedback over the internet
- Making motor mounts out of 0.25" MDF meant that I was not able to find long enough screws to go through both sides and secure them with a nut (25mm is the largest size available in the GM lab)

### Actual vs Intended performance

Besides the evolving can grabbing approach described in the previsous ection, another difference between simulated vs actual performance was that over time, motors started to wiggle left and right making it harder to strafe with the holonomic drive since the geometry of the wheels was changing

*Measured drawings (e.g. CAD) in Appendix*

## Electrical Design

- Describe intended approach and actual performance
- Include things that you tried but failed (and thus learned from)
- Include schematics in appendix.

## Processor architecture and code architecture

- Include a block diagram of how MCU's are logically connected
- Describe software approach
- Include things that you tried but failed (and thus learned from)

## Retrospective (non-graded)
- What you feel was most important that you learned
- What was the best parts of the class
- What you had the most trouble with
- What you wish was improved
- Anything else about class.

## Appendix

- BOM (Bill of Materials), which list the items and quantities used to construct your robot and controller.
- Schematics of all electronic circuits
- Nice photo or rendering of full robot
- CAD drawings (or mechanical drawings) highlighting anything special
- All data sheets for all components that are not parts supplied by the ministore (either links or copies of data sheets)
- Upload all code to canvas (separately)
- Include links to videos of functionality
- Optional if you have fun competition video links please add.
