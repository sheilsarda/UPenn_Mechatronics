# Lab 2 - Capturing Inputs

Sheil Sarda <sheils@seas.upenn.edu>

## 1. Switches, Debouncing and Input Capture

### 2.1.1 SPST Switch Wiring

<img src="spst_schematic.jpeg" width=800>

````c
#include "teensy_general.h"
#include "t_usb.h"

int main(void){
    m_usb_init();

    set(TCCR1B, CS12); // set 256 prescalar    
    teensy_clockdivide(0); //set the clock speed

    clear(DDRB, 7); // B7 is input
    set(PORTB, 7);  // set internal pullup

    set(DDRC, 7);  // C7 is output 
    clear(PORTC, 7); // no LED

    while(!m_usb_isconnected()); // wait for a connection
    while(1){
	// detect if button is pressed
        if(!bit_is_set(PINB, 7)){
            set(PORTC, 7);
	    teensy_led(ON);
	    m_usb_tx_string("Bit is set\r\n");
	} else {
	    // button is not pressed
	    clear(PORTC, 7);
	    teensy_led(OFF);
	}
    }
}

````

### 2.1.2 Debouncing the SPST

#### Add resistor and capacitor low-pass filter between the switch and the Teensy

**Notes**

- Make sure you have a large enough RC time constant so that you don’t see multiple bounces on a single press (i.e., 100Hz bounces)
- Not too large that you distort button presses that occur at roughly 10 times per second (faster than most humans can press). 

#### Verify RC Circuit

- Use the output of a square wave from a function generator into the RC circuit and view the output on an oscilloscope or your OscilloSorta. 
- Notice how the signal changes as the frequency goes from below 10Hz to above 100Hz.
- Submit OscilloSorta images for each case. Ideally two traces:
    - one with output of input signal square wave 
    - one with filtered output 
- Show your calculation for the cutoff frequency of your low pass filter.

#### Input Capture

Use the input capture function of the timer on the Atmega32U4 to measure how fast you can depress a switch

**Note: ** make sure that the switch is debounced, so you only get valid presses. 

#### Human Reaction Speed

Change the code so that it:
1. prompts the person when to start
1. measures 5 presses 
1. prints out the average time between the 5 presses in milliseconds

**Leaderboard**

| Tries | Fastest Time |
| ----- | ------------ |
|       |              |

#### C Code 

````c

````

#### Schematic 

<img src="rc_schematic.jpeg" width=800>

### Extra Credit: Improve Granularity

Determine and implement the finest time resolution that you can measure these time presses using input capture 3 assuming the slowest a person will be is 0.5 seconds.
