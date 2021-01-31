/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file

#define FREQ_HZ    50    // variable for frequency
#define PRESCALAR  256  // prescalar used
#define SYS_CLOCK  16e6 // clock speed (16 Mhz)

#define RISE_TIME 4000 // time in ms to full intensity
#define FALL_TIME 4000 // time in ms to 0 intensity
#define MAX_INTENSITY 1 // max intensity


/* 
 * Uses the following defined variables
 * `RISE_TIME`, `FALL_TIME`, `MAX_INTENSITY`,
 * `FREQ_HZ`, `PRESCALAR` and `SYS_CLOCK`
 * Assumes LED is plugged into Port `B5`
 * Uses Timer 1 for PWM functionality
 */
void pulse_led(){
    set(TCCR1B, CS12); // set 256 prescalar    
    teensy_clockdivide(0); //set the clock speed

    set(DDRB, 5); // B5 is output compare pin

    // (mode 14) UP to ICR1, PWM mode
    set(TCCR1B, WGM13); set(TCCR1B, WGM12); set(TCCR1A, WGM11);

    // toggle B5 at OC
    set(TCCR1A, COM1A1); 

    /* Set OC1A on compare match when upcounting. Clear OC1A 
     * on compare match when down-counting. 
     */
    set(TCCR1B, WGM13); set(TCCR1B, WGM12);
    
    // set compare match register
    ICR1  = SYS_CLOCK/(FREQ_HZ*PRESCALAR); 
    
    double arr[] = {
        0,100,75,50,25,0,50,37.5,25,12.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,75,50,25,0,50,37.5,25,12.5,0
    };
    
    int len = (sizeof(arr) / sizeof(double));

    // time to spend at each duty cycle
    int rise_ms = RISE_TIME/len; 
    int fall_ms = FALL_TIME/len; 
    
    while(1){
        // rising
        for(int i = 0; i < len; ++i){
            OCR1A = ICR1*arr[i]*MAX_INTENSITY;
	    _delay_ms(rise_ms);
        }
        // falling
        for(int i = len-1; i >= 0; --i){
            OCR1A = ICR1*arr[i]*MAX_INTENSITY;
	    _delay_ms(fall_ms);
        }
    }


}

int main(void)
{
    return 0;   /* never reached */
}
