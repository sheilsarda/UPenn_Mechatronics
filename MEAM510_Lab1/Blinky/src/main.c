/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file

int main(void)
{
    teensy_clockdivide(0); //set the clock speed
    teensy_led(ON);			// turn on the on board LED
    teensy_wait(1000);		// wait 1000 ms when at 16 MHz 
    /* insert your hardware initialization here */
    for(;;){
        /* insert your main loop code here */
        teensy_led(TOGGLE);	// switch the led state
        teensy_wait(1000);		// wait
    }
    return 0;   /* never reached */
}
