/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file

#define FREQ_HZ 20 // variable for frequency
#define PRESCALAR 256 // prescalar used
#define SYS_CLOCK 16e6 // clock speed (16 Mhz)
#define DUTY_CYCLE 100 // duty cycle %

int main(void)
{
    /* insert your hardware initialization here */

    teensy_clockdivide(0); //set the clock speed
    set(DDRC, 7);
    set(TCCR1B, CS12); // set 256 prescalar
    
    TCNT1 = 0x00;

    // set compare match register
    OC1A = SYS_CLOCK/(2*FREQ_HZ*PRESCALAR); 
    for(;;){
	
	if(TCNT1 > cutoff){
            toggle(PORTC, 7);
	    teensy_led(TOGGLE);
	    TCNT1 = 0x00;
        }
    }
    return 0;   /* never reached */
}
