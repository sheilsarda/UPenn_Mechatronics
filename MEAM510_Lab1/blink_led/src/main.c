/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file

#define FREQ_HZ 200 // variable for frequency
#define DUTY_CYCLE 50 // variable for duty cycle percentage

int main(void)
{
    teensy_clockdivide(0); //set the clock speed
    set(DDRC, 7);

    // 16e6/((2^16)*256) = 0.95
    set(TCCR1B, CS12) // set 256 prescalar


    /* insert your hardware initialization here */
    for(;;){
        /* ON Part of the Duty Cycle */
        toggle(PORTC, 7);
        teensy_wait(10 * DUTY_CYCLE / FREQ_HZ);		// wait

        /* OFF Part of the Duty Cycle */
        clear(PORTC, 7);
	teensy_wait(10 * (100 - DUTY_CYCLE) / FREQ_HZ);		// wait
    }
    return 0;   /* never reached */
}
