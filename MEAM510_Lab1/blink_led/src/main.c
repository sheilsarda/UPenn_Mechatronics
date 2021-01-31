/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file

#define FREQ_HZ    50    // variable for frequency
#define PRESCALAR  256  // prescalar used
#define SYS_CLOCK  16e6 // clock speed (16 Mhz)

#define RISE_TIME 4000 // time in ms to full intensity
#define MAX_INTENSITY 1 // max intensity
#define LERP 5 // number of points to lerp between variable
double intensity[] = {0,100,75,50,25,0,50,37.5,25,12.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

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
    
    int len = (sizeof(intensity) / sizeof(double));

    // time to spend at each duty cycle
    int rise_ms = RISE_TIME/(len*LERP); 
    
    int lerp_i = 0;
    double lerp_intensity;
    
    while(1){
        for(int i = 0; i < len; ++i){
	    // LERP between values
	    if(i < len - 1){ 
            
	        for(lerp_i = 0; lerp_i < LERP; ++lerp_i){
               
  	           lerp_intensity = (intensity[i+1] - intensity[i])*(lerp_i/LERP);
    	           lerp_intensity += intensity[i];

	           OCR1A = ICR1*lerp_intensity*MAX_INTENSITY;
                   _delay_ms(rise_ms);
	        }
	    } else { // last value in array
		OCR1A = ICR1*intensity[i]*MAX_INTENSITY;
                _delay_ms(rise_ms);
	    }
        }
    }
}

int main(void)
{
    pulse_led();
    return 0;   /* never reached */
}
