/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file

#define FREQ_HZ    50    // variable for frequency
#define PRESCALAR  256  // prescalar used
#define SYS_CLOCK  16e6 // clock speed (16 Mhz)
#define DUTY_CYCLE .5    // duty cycle 

int main(void)
{
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
    
    double arr[] = {0, 0.05, 0.2, 0.5, 1, 1, 1, 0.5, 0.2, 0.05};
    int len = (sizeof(arr) / sizeof(double));
    while(1){
    for(int i = 0; i < len; ++i){
        OCR1A = ICR1*arr[i];
	_delay_ms(2);
    }}

    // set duty cycle PWM
    // OCR1A = ICR1*DUTY_CYCLE;  
   
    while(1);
    return 0;   /* never reached */
}
