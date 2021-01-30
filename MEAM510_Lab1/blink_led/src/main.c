/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file

#define FREQ_HZ 200 // variable for frequency
#define PRESCALAR 64
int main(void)
{
    /* insert your hardware initialization here */

    teensy_clockdivide(0); //set the clock speed
    set(DDRC, 7);
    // set(TCCR1B, CS12); // set 256 prescalar

    // 64 prescalar
    set(TCCR1B, CS10); 
    set(TCCR1B, CS11); 

    uint64_t cutoff = 16e6 / (FREQ_HZ * PRESCALAR);
    for(;;){
	
	if(TCNT1 > cutoff){
            toggle(PORTC, 7);
	    // teensy_led(TOGGLE);
	    TCNT1 = 0;
        }
    }
    return 0;   /* never reached */
}
