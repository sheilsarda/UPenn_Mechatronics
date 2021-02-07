/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"
#include "t_usb.h"
#include <string.h>
#include <stdbool.h>

#define DEBUG_PRINTS 1
#define MIN_TIME 1563 // 100ms in clock ticks
#define CLOCK_SPEED 16e6 
#define PRESCALAR 1024

int button_presses[5]; // record tcnt3 values for each press
int press_i; // ix of button press being recorded
bool prev_state; // true when switch is depressed
bool prompt_shown;

void compute_results(){
    unsigned long sum_t = 0;
    for(int i = 1; i < 5; ++i){
        sum_t += (button_presses[i] - button_presses[i-1]);
    }
    sum_t /= 4;
    sum_t = (long) sum_t * CLOCK_SPEED / PRESCALAR; 

    m_usb_tx_string("||=======  Reaction Time: "); 
    m_usb_tx_ulong(sum_t);
    m_usb_tx_string(" ms  ==========||\r\n");
    prompt_shown = false;
}

void toggle_print(){
    m_usb_tx_string("||======   Button Press  ==========||\r\n");
}

void display_prompt(){
    m_usb_tx_string("||======   Start Pressing!   ======|| \r\n");
    prompt_shown = true;
}

void record_press(bool new_state){
    if(prev_state != new_state){
	// only record down presses
        if(new_state){
            if(press_i == 0 || TCNT3 - button_presses[press_i-1] > 4*MIN_TIME){ 
    	        button_presses[press_i] = TCNT3;
                toggle_print();
                press_i++;
	    }
	}
    }
    prev_state = new_state;
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
    prompt_shown = false;

    while(!m_usb_isconnected()); // wait for a connection
    
    while(1){
	
	if(press_i == 5) compute_results();
	if(!prompt_shown) {
	    memset(button_presses, 0x00, sizeof(button_presses)); // clear array
	    press_i = 0;
	    display_prompt();
	}
	
	// detect if button is pressed
        if(!bit_is_set(PINB, 7)){
            record_press(true);
	    set(PORTC, 7);
	} else {
	    // button is not pressed
	    record_press(false);
	    clear(PORTC, 7);
	}
    }
}

