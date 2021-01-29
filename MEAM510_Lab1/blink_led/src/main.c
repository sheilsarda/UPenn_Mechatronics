/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file

#define FREQ_HZ 10 // variable for frequency

int main(void)
{
    /* insert your hardware initialization here */

    teensy_clockdivide(0); //set the clock speed
    set(DDRC, 7);
    set(TCCR1B, CS12); // set 256 prescalar

    /* 
     * 16e6/((2^16)*256) = 0.95
     * 200 / 0.95 = 210
     */

    for(;;){
	if(TCNT1 > 40000){
            toggle(PORTC, 7);
	    teensy_led(TOGGLE);
	    TCNT1 &= 0;
        }
    }
    return 0;   /* never reached */
}
