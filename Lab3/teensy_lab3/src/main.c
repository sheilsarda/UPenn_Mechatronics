/* Name: main.c
 * Author: Sheil Sarda
 */
#include "teensy_general.h"
#include "t_usb.h"
#include <stdbool.h>
#include <string.h>

#define CLOCK_SPEED 16e6
#define PRESCALAR 1024 // freq of 15.625kHz
#define USB 1

void setup_ADC(char adc_num){

    set(ADMUX, REFS0); // AVcc
    set(ADCSRA, ADEN); // enable ADC
    set(ADCSRA, ADSC); // first conversion

    // 128 ADC prescalar
    set(ADCSRA, ADPS0); 
    set(ADCSRA, ADPS1); 
    set(ADCSRA, ADPS2); 

    switch(adc_num){ // Disable digital input
        case 0d00: set(DIDR0, ADC0D);   break;
        case 0d01: set(DIDR0, ADC1D);   break;
        case 0d04: set(DIDR0, ADC4D);   break;
        case 0d05: set(DIDR0, ADC5D);   break;
        case 0d06: set(DIDR0, ADC6D);   break;
        case 0d07: set(DIDR0, ADC7D);   break;
        case 0d08: set(DIDR2, ADC8D);   break;
        case 0d09: set(DIDR2, ADC9D);   break;
        case 0d10: set(DIDR2, ADC10D);  break;
        case 0d11: set(DIDR2, ADC11D);  break;
        case 0d12: set(DIDR2, ADC12D);  break;
        case 0d13: set(DIDR2, ADC13D);  break;
    }

    switch(adc_num){
        case 0d00:                      break;
        case 0d01:  set(ADMUX, MUX0);   break;
        case 0d04:  set(ADMUX, MUX2);   break;
        case 0d05:  set(ADMUX, MUX2); 
                    set(ADMUX, MUX0);   break;
        case 0d06:  set(ADMUX, MUX2);
                    set(ADMUX, MUX1);   break;
        case 0d07:  set(ADMUX, MUX0);
                    set(ADMUX, MUX1);
                    set(ADMUX, MUX2);   break;
        case 0d08:  set(ADCSRB, MUX5);  break;
        case 0d09:  set(ADCSRB, MUX5);
                    set(ADMUX, MUX0);   break;
        case 0d10:  set(ADCSRB, MUX5);
                    set(ADMUX, MUX1);   break;
        case 0d11:  set(ADCSRB, MUX5);
                    set(ADMUX, MUX1);
                    set(ADMUX, MUX0);   break;
        case 0d12:  set(ADCSRB, MUX5);
                    set(ADMUX, MUX2);   break;
        case 0d13:  set(ADCSRB, MUX5);
                    set(ADMUX, MUX2);
                    set(ADMUX, MUX0);   break;
    }                    
}

int main(void){
    #ifdef USB
        m_usb_init();
        while(!m_usb_isconnected());
    #endif

    // set 1024 prescalar
    set(TCCR3B, CS32); set(TCCR3B, CS30);
    teensy_clockdivide(0); //set the clock speed

    setup_ADC(); 

    while(1){
    
    }
}
