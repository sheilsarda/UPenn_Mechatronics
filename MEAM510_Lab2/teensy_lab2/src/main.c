/* Name: main.c
 * Author: Sheil Sarda
 */
#include "teensy_general.h"
#include <stdbool.h>

#define CLOCK_SPEED 16e6
#define PRESCALAR 1024
#define SW_SIZE 200

bool sw[SW_SIZE];
int num_entries = 0;

bool update_window(bool is20){

    if(num_entries < SW_SIZE) sw[num_entries - 1] = is20;
    else {
        num_entries = (num_entries % SW_SIZE);
        sw[num_entries] = is20;
    }
    num_entries++;

    int num_true = 0;
    for(int i = 0; i < SW_SIZE; ++i)
        if(sw[i]) num_true++;

    if(num_true > SW_SIZE/2) return true;
    return false;
}


int main(void){
    // set 1024 prescalar
    set(TCCR3B, CS32); set(TCCR3B, CS30);

    teensy_clockdivide(0); //set the clock speed

    clear(DDRB, 7); // B7 is input
    set(PORTB, 7);  // internal pullup

    set(DDRC, 7); clear(PORTC, 7); // RED
    set(DDRB, 4); clear(PORTB, 4); // GREEN

    bool result = false;
    memset(sw, 0, sizeof(sw));

    while(1){
        result = update_window(!bit_is_set(PINB, 7));

        // detect freq
        if(result){
            set(PORTC, 7);
            clear(PORTB, 4);
        } else {
            clear(PORTC, 7);
            set(PORTB, 4);
        }
    }
}
