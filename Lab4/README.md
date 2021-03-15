# Lab 4 - Mobility

Sheil Sarda <sheils@seas.upenn.edu>

## 4.1 Fabrication and Motor Driving

### 4.1.1 Driving Motors

#### Setup the H-Bridge motor driver SN754420 to drive one of the supplied yellow motors

*Refer to Slide 51 of Lecture 14*

1. Optionally, use the FAN8100
1. Power the motor using the Beston battery

#### Use a NAND chip 74HC00 in your kit to make an inverter

*Refer to Lecture 04*

1. Use this inverter so you need only one I/O line to set the direction of the motor
  1. the I/O line drives 1A and the input to the inverter, so the output of the
    inverter drives 2A
  1. To use a NAND gate as an inverter, connect the signal to both inputs and use
  the output as the inverted signal
1. Set up the OscilloSorta to drive the enable pin with PWM and control the
direction of the motor by switching voltages manually
  1. optionally connect a switch and pullup resistor
  1. Use 50Hz with 0% to 100% duty cycle to change velocity of motor
1. Demonstrate the motor changing speeds with varying PWM and different directions
with different speeds.
1. Show a TA for check off.
1. Submit a photo of your
circuit where the lines and motor are visible.

<img src="imgs/h_bridge.jpg" width=500px>

### 4.1.2 Car Architecture

#### Research different types of mobile bases:

1. holonomic drives
1. differential drives
    1. [AWD vs 4WD](https://www.youtube.com/watch?v=rHMDtEPeuQM&ab_channel=WonderWorld)
    1. [MEAM510 Car Demo](https://www.youtube.com/watch?v=fsVbaADWTgg)
    1. [4 DC Motor + Servo RC Car build](https://www.youtube.com/watch?v=NtPyDhM1dF0&ab_channel=EliteWorm)
    1. [RC Car with power in opposite wheels](https://www.youtube.com/watch?v=vRmBVLQngJ8&ab_channel=DattaBenurCreation)

    <img src="imgs/2motor.png" width=500px>

    1. [RC Car with Servo and DC Motor](https://www.youtube.com/watch?v=jjZU6sNkEV8&ab_channel=MOCreative)

    <img src="imgs/step1.png" width=500px>
    <img src="imgs/step2.png" width=500px>
    <img src="imgs/step3.png" width=500px>

    1. [Straight Gear Rack Steering](https://www.youtube.com/watch?v=ZdtPTUsrAA4&ab_channel=MaxImagination)

    <img src="imgs/straight_gear.png" width=500px>

1. tank-steering drives
    1. [Wikipedia](https://en.wikipedia.org/wiki/Tank_steering_systems)

If you choose to use different motors, be careful about the power
capability of the battery and the limits of the motor drivers.

**Motor Dimensions**

<img src="imgs/motor_dims.jpg" width=400>

#### Discuss with your TA/coach about your design.

1. Have a completed draft CAD assembly of your car ready to be reviewed
by teaching staff in lecture on Monday 3/15.
1. Be ready to create and submit `.dwg` files if your
TA/coach approves your design.
1. Drafts should be near complete so the TA’s can judge how
they assemble.
1. Be sure to follow the laser-cutting guidelines (reproduced below for your
convenience).
1. ​Submit drafts to canvas.

**Draft 1**

<img src="imgs/draft1_1.png" width=600>
<img src="imgs/draft1_2.png" width=600>
<img src="imgs/draft1_3.png" width=600>

**Draft 4 (vFinal)**

<img src="imgs/final_1.png" width=600>
<img src="imgs/final_2.png" width=600>
<img src="imgs/final_3.png" width=600>
