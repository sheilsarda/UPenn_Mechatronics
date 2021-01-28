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
| Continuous Forward Current (If)	| 30 mA 				|
| Battery Voltage (V)				| 5 V   				|
| Forward Voltage (Vf)				| 2-2.6 V  				|
| Max Voltage across resistor		| 5 - 2 = 3V 			|
| **Resistance (R)**		| 3V / 30mA = **100 Ohms** 	|
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

## 2. Introduction to Teensy 

## 3. Timers

## 4. Practice with Loops

## 5. Retrospective 

Spent approximately 10 hours on this lab.
