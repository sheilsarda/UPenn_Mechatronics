/* Name: main.c
 * Author: Sheil Sarda
 */
#include "teensy_general.h"

#define CLOCK_SPEED 16e6
#define PRESCALAR 1024

int main(void){
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
