/* Name: main.c
 * Author: Sheil Sarda
 */
#include "teensy_general.h"
#include "t_usb.h"
#include <stdbool.h>
#include <string.h>

#define CLOCK_SPEED 16e6
#define PRESCALAR 1024 // freq of 15.625kHz
#define SW_SIZE 800
#define USB 0
#define TOLERANCE 10.0
#define FREQ1 23.0
#define FREQ2 200.0
#define TIMEOUT 0xFFFF

float t = 0.0;

unsigned int signal_died;

bool update_window(){

    TCNT3 = 0x00; // waiting for next edge
    signal_died = TIMEOUT; // stop looking for signal
    while(!bit_is_set(PINB, 7) && signal_died) signal_died--;
    while(bit_is_set(PINB, 7) && signal_died)  signal_died--;

    t =  TCNT3;
    t /= 1.0*CLOCK_SPEED;
    t *= 1000.0*PRESCALAR; // period in ms

    #ifdef USB 
        m_usb_tx_string("Period was: ");
        m_usb_tx_uint(t);
        m_usb_tx_string("\r\n");
    #endif
    
    if(t < 1000.0/(FREQ1 - TOLERANCE) && 
       t > 1000.0/(FREQ1 + TOLERANCE)) return true;
    return false;

}


int main(void){
    #ifdef USB
        m_usb_init();
        while(!m_usb_isconnected());
    #endif

    // set 1024 prescalar
    set(TCCR3B, CS32); set(TCCR3B, CS30);
    teensy_clockdivide(0); //set the clock speed

    clear(DDRB, 7); // B7 is input
    set(PORTB, 7);  // internal pullup

    set(DDRC, 7); clear(PORTC, 7); // RED
    set(DDRB, 4); clear(PORTB, 4); // GREEN

    bool result = false;
    bool is20   = false;

    while(1){
        result = !bit_is_set(PINB, 7);

        if(!result){ // no signal
            clear(PORTB, 4);
            clear(PORTC, 7);
            continue;
        }
        
        is20 = update_window();
        if(is20){
            #ifdef USB 
                m_usb_tx_string("Detecting 10-30Hz\r\n");
            #endif
            set(PORTC, 7);
            clear(PORTB, 4);
        } else {
            #ifdef USB 
                m_usb_tx_string("Detecting >30Hz\r\n");
            #endif
            clear(PORTC, 7);
            set(PORTB, 4);
        }
    }
}
