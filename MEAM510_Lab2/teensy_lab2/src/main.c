/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"

int main(void)
{
    /* insert your hardware initialization here */
	teensy_clockdivide(0);
    teensy_wait(1000);
    for(;;){
        /* insert your main loop code here */
		teensy_led(TOGGLE);
        teensy_wait(1000);
    }
    return 0;   /* never reached */
}
