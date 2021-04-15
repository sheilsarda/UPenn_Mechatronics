# Lab 5 - Final Project

Sheil Sarda <sheils@seas.upenn.edu>

## Sensor List

1. [Reflective Infrared IR Optical Sensor](https://www.adafruit.com/product/2349)
    - Cost: $1.95
    - Detection range is ~2-10mm
1. [IR distance sensor](https://www.adafruit.com/product/164)
1. Ultrasonic Ranging
    1. [RCWL-1601](https://www.adafruit.com/product/4007)
        - Cost: $3.95
        - Range: 10cm – 250cm
        - Speed: ~10 samples/sec
        - Beam width: ~75 degrees
    1. [HC-SR04](https://www.adafruit.com/product/3942)
1. [Sharp GP2Y0A21YK0F](https://www.pololu.com/product/136)
    - Cost: $10.90
    - Analog output can be connected to ADC pin for distance measurements
    - Detection range is ~10-80 cm (4″ to 32″)
    - [Datasheet](http://www.socle-tech.com/doc/IC%20Channel%20Product/Sensors/Distance%20Measuring%20Sensor/Analog%20Output/GP2Y0A21YK0F_spec.pdf)


## Actuator

1. 6V N20 DC Motor with Magnetic Encoder  
    - 1:50 Gear Ratio
    - Cost: $12.50
    - [Datasheet](https://www.adafruit.com/product/4638#technical-details)
    - [Motor shafts to use orange wheels](https://www.pololu.com/categMory/179/motor-shaft-adapters)

        ![Motor Pinout](imgs/motor_pinout.png)
1. SG90 Servo

## Base and Wheels

![Sketch](imgs/sketch.png)

| Part | Cost |
|--|--|
| [Right Mecanum Wheel - 48mm Diameter](https://www.adafruit.com/product/4678)  | $4.95 * 2 = $9.90 |
| [Left Mecanum Wheel - 48mm Diameter](https://www.adafruit.com/product/4679)   | $4.95 * 2 = $9.90 |

![Base](imgs/base.jpg)

## Holonomic Drive Refs

1. [Details of Mecanum Drive](https://www.servomagazine.com/magazine/article/a-look-at-holonomic-locomotion)

## Autonomous Behaviors

1. Follow the wall till you get to the can, pick it up and go back in reverse
1. Follow the beacon using a photo-reflective pair, pick up the can and follow a line on the way back

## Learnings from Lab 4

- Better controller design for 3-wheeled drive
- Better motor mounts than MDF
- Use Acrylic instead of MDF for entire chasis construction
