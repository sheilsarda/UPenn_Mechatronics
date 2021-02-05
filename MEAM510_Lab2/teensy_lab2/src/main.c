/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"
#include "t_usb.h"
#include <stdbool.h>

void toggle_print(){
    m_usb_tx_uint(TCNT3);
    m_usb_tx_string(" >> Switch Toggled\r\n");

}

int main(void){
    m_usb_init();

    // set 1024 prescalar    
    set(TCCR3B, CS32); set(TCCR3B, CS30); 

    teensy_clockdivide(0); //set the clock speed

    clear(DDRB, 7); // B7 is input
    set(PORTB, 7);  // set internal pullup

    set(DDRC, 7);  // C7 is output 
    clear(PORTC, 7); // no LED

    bool prev_state = false; // true when switch is depressed

    while(!m_usb_isconnected()); // wait for a connection
    while(1){
	// detect if button is pressed
        if(!bit_is_set(PINB, 7)){
            set(PORTC, 7);
	    teensy_led(ON);
	    if(!prev_state) toggle_print();
	    prev_state = true;
	} else {
	    // button is not pressed
	    if(prev_state) toggle_print();
	    clear(PORTC, 7);
	    teensy_led(OFF);
	    prev_state = false;
	}
    }
}

