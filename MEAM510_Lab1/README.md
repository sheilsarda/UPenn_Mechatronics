# Lab 1 - Microcontrollers

Sheil Sarda <sheils@seas.upenn.edu>

## 1. LED

### 1.1.1 Ranges of Resistance for LED

[Digikey datasheet for 160-1128-ND ​LTL-4224 ​5mm red LED](https://media.digikey.com/pdf/Data%20Sheets/Lite-On%20PDFs/LTL-4224.pdf)

#### From Page 3:

![](../imgs/led_datasheet.png)

#### Calculate the smallest resistor that you can safely use with this LED in series with 5V power. 


| 	| 		|
| :------------- | :----------: |
| Continuous Forward Current (If)	| 20 mA 				|
| Battery Voltage (V)				| 5 V   				|
| Forward Voltage (Vf)				| 2-2.6 V  				|
| Max Voltage across resistor		| 5 - 2 = 3V 			|
| **Resistance (R)**		| 3V / 20mA = **150 Ohms** 	|
| 	| 		|

### 1.1.2 Optimal Resistance for Dim Light

Resistance for Dim Light = 10 KOhms

| 	| 		|
| :------------- | :----------: |
| Battery Voltage (V)				| 5 V   					|
| Forward Voltage (Vf)				| 2-2.6 V  					|
| Max Voltage across resistor		| 5 - 2 = 3V 				|
| Resistance (R)					| 10 KOhms				 	|
| **Current (If)**					| 3V / 10 KOhms = **0.3 mA**|
| 	| 		|

### 1.1.3 Two LEDs in Parallel

LEDs chosen

| LED Type	| Forward Voltage	| Current |
| :------------- | :----------: | :----------: |
|[160-1128-ND ​LTL-4224 ​5mm red LED](https://media.digikey.com/pdf/Data%20Sheets/Lite-On%20PDFs/LTL-4238.pdf) 		| 	2V	| 20 mA |
|[160-1132-ND ​LTL-4238 ​ 5mm green LED](https://www.digikey.com/htmldatasheets/production/33348/0/0/1/ltl-4238.html) 	|  2.1V	| 20 mA |
| 	| 		|

Based on the specifications, I would expect the Red LED to be slightly brighter than the green LED because of the lower forward voltage drop.

Measured voltage at A: **1.90 V**. The Red LED is slightly brighter.

### 1.1.4 Two LEDs in Series

Measured voltage at A: **3.81V**. 

Voltage between the 2 LEDs: **1.95V**

This is expected based on the datasheets, because due to current being the same for both LEDs in series, causing the voltage drop in the middle of the two to be approximately half of the voltage at A.

### 1.1.5 Optimal LED for a Table Lamp

[American Bright Optoelectronics Corporation AB-EZP10A-B3-K13](https://www.digikey.com/en/products/detail/american-bright-optoelectronics-corporation/AB-EZP10A-B3-K13/9677930)

Cost: $1.91

Size / Dimension: 0.118"" L x 0.118"" W (3.00mm x 3.00mm)

Features:

- Luminous intensity remains constant for 10.4V < Vin < 24V
- Can actively modulate the power usage by decreasing the input current to avoid overheating.
- The input current, light output, and color temperature can stay consistent despite the exterior voltage changes or fluctuations.
- Uses conventional 12 V / 24 VDC voltage sources


## 2. Introduction to Teensy 

### 1.2.1 Modified Registers

- Data Direction Register `DDRC`
- GPIO Pinout `PORTC`

### 1.2.2 Circuit Diagram


### 1.2.3 Variable Frequency

Frequency appears to be continually on at around 60Hz

#### C Code

````c
#include "teensy_general.h"  // includes the resources included in the teensy_general.h file

#define FREQ_HZ 60 // variable for frequency

int main(void)
{
    teensy_clockdivide(0); //set the clock speed
    set(DDRC, 7);

    /* insert your hardware initialization here */
    for(;;){
        /* insert your main loop code here */
        toggle(PORTC, 7);
        teensy_led(TOGGLE);     // switch the led state
        teensy_wait(1000/FREQ_HZ);              // wait
    }
    return 0;   /* never reached */
}
````

### 1.2.4 Variable Duty Cycle

````c

````

### 1.2.5 Maximum LEDs from a GPIO Pin

#### Circuit Diagram


#### Limitations

## 3. Timers

## 4. Practice with Loops

## 5. Retrospective 

Spent approximately 10 hours on this lab.
