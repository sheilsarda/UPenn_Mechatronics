/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"
#include "t_usb.h"
#include <stdbool.h>

#define DEBUG_PRINTS 1


int button_presses[5]; // record tcnt3 values for each press
int press_i; // ix of button press being recorded
bool prev_state; // true when switch is depressed

void compute_results(){
    unsigned long sum_t = 0;
    for(int i = 1; i < 5; ++i){
        sum_t += (button_presses[i] - button_presses[i-1]);
    }
    sum_t /= 4;

    m_usb_tx_string("=========================\r\n");
    m_usb_tx_string("|| Reaction Time: "); 
    m_usb_tx_ulong(sum_t);
    m_usb_tx_string("   ||\r\n");
    m_usb_tx_string("=========================\r\n");

}

void toggle_print(){
    m_usb_tx_int(press_i);
    m_usb_tx_string(" >> Button Press Recorded\r\n");

}

void record_press(bool new_state){

    #ifdef DEBUG_PRINTS
    if(prev_state != new_state) toggle_print();
    #endif

    prev_state = new_state;
    button_presses[press_i++] = TCNT3;
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

    prev_state = false;

    #ifdef DEBUG_PRINTS
    while(!m_usb_isconnected()); // wait for a connection
    #endif
    
    while(1){
	
	if(press_i = 4) compute_results();
	if(press_i == 0 || press_i = 4){
	    memset(button_presses, 0x00); // clear array
	    press_i = 0;
	}
	if(press_i == 0) display_prompt();
	
	// detect if button is pressed
        if(!bit_is_set(PINB, 7)){
            record_press(true);
	    set(PORTC, 7);
	    teensy_led(ON);
	} else {
	    // button is not pressed
	    record_press(false);
	    clear(PORTC, 7);
	    teensy_led(OFF);
	    prev_state = false;
	}
    }
}

