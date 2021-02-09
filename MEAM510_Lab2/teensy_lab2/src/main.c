/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"
#include "t_usb.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define DEBUG_PRINTS 1
#define MIN_TIME 1563 // 100ms in clock ticks
#define CLOCK_SPEED 16e6 
#define PRESCALAR 1024


int main(void){
    m_usb_init();

    // set 1024 prescalar    
    set(TCCR3B, CS32); set(TCCR3B, CS30); 

    teensy_clockdivide(0); //set the clock speed

    clear(DDRB, 7); // B7 is input

    set(DDRC, 7);  // C7 is output 
    clear(PORTC, 7); // no LED
    
    while(1){
	
	// detect if light
        if(bit_is_set(PINB, 7)){
	    set(PORTC, 7);
	} else {
	    clear(PORTC, 7);
	}
    }
}

