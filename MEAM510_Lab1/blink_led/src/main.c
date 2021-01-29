/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file

int main(void)
{
    teensy_clockdivide(0); //set the clock speed

    set(DDRC, 7);

    /* insert your hardware initialization here */
    for(;;){
        /* insert your main loop code here */
        toggle(PORTC, 7);
        teensy_led(TOGGLE);	// switch the led state
        teensy_wait(1000);		// wait
    }
    return 0;   /* never reached */
}
