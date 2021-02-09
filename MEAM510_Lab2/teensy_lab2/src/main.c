/* Name: main.c
 * Author: Sheil Sarda
 */
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

