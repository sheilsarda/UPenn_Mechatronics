/* Name: main.c
 * Author: Sheil Sarda
 */
#include "teensy_general.h"
#include "t_usb.h"
#include <stdbool.h>
#include <string.h>

#define CLOCK_SPEED 16e6
#define PRESCALAR3 1024 // freq of 15.625kHz
#define PRESCALAR1 256

#define TARGET 10e3
#define USB 1
#define MAX_ADC 1023
#define MAX_ANG 300

#define FREQ_HZ    2    // variable for frequency
#define DUTY_CYCLE 50    // duty cycle

void setup_ADC(char adc_num){

    set(ADMUX, REFS0); // AVcc

    // 128 ADC prescalar
    set(ADCSRA, ADPS0); 
    set(ADCSRA, ADPS1); 
    set(ADCSRA, ADPS2); 

    switch(adc_num){ // Disable digital input
        case 0 : set(DIDR0, ADC0D);   break;
        case 1 : set(DIDR0, ADC1D);   break;
        case 4 : set(DIDR0, ADC4D);   break;
        case 5 : set(DIDR0, ADC5D);   break;
        case 6 : set(DIDR0, ADC6D);   break;
        case 7 : set(DIDR0, ADC7D);   break;
        case 8 : set(DIDR2, ADC8D);   break;
        case 9 : set(DIDR2, ADC9D);   break;
        case 10: set(DIDR2, ADC10D);  break;
        case 11: set(DIDR2, ADC11D);  break;
        case 12: set(DIDR2, ADC12D);  break;
        case 13: set(DIDR2, ADC13D);  break;
    }

    set(ADCSRA, ADEN); // enable ADC
    set(ADCSRA, ADSC); // first conversion
    
}

void setup_next(char adc_num){

    unsigned int mask1 = 
                        (1 << MUX0) | 
                        (1 << MUX1) | 
                        (1 << MUX2);

    ADMUX &= ~mask1;

    clear(ADCSRB, MUX5);

    switch(adc_num){
        case 0 :                        break;
        case 1 :    set(ADMUX, MUX0);   break;
        case 4 :    set(ADMUX, MUX2);   break;
        case 5 :    set(ADMUX, MUX2); 
                    set(ADMUX, MUX0);   break;
        case 6 :    set(ADMUX, MUX2);
                    set(ADMUX, MUX1);   break;
        case 7 :    set(ADMUX, MUX0);
                    set(ADMUX, MUX1);
                    set(ADMUX, MUX2);   break;
        case 8 :    set(ADCSRB, MUX5);  break;
        case 9 :    set(ADCSRB, MUX5);
                    set(ADMUX, MUX0);   break;
        case 10:    set(ADCSRB, MUX5);
                    set(ADMUX, MUX1);   break;
        case 11:    set(ADCSRB, MUX5);
                    set(ADMUX, MUX1);
                    set(ADMUX, MUX0);   break;
        case 12:    set(ADCSRB, MUX5);
                    set(ADMUX, MUX2);   break;
        case 13:    set(ADCSRB, MUX5);
                    set(ADMUX, MUX2);
                    set(ADMUX, MUX0);   break;
    }                    
    
}

int read_adc(char next_adc){
    
    set(ADCSRA, ADSC); // first conversion
    
    setup_next(next_adc);

    while(!bit_is_set(ADCSRA, ADIF)); 

    unsigned int result = ADC;

    set(ADCSRA, ADIF); // clear compute flag
    return result;
}

void setup_PWM(){
    set(DDRB, 5); // output compare

    // (mode 14) up to ICR1, PWM
    set(TCCR1B, WGM13);
    set(TCCR1B, WGM12);
    set(TCCR1A, WGM11);

    set(TCCR1A, COM1A1); // clear OC

    ICR1 = CLOCK_SPEED/(FREQ_HZ * PRESCALAR1);
    OCR1A = ICR1 * DUTY_CYCLE;
}

int main(void){
    #ifdef USB
        m_usb_init();
        while(!m_usb_isconnected());
    #endif

    // set 1024 prescalar - ADC clock
    set(TCCR3B, CS32); set(TCCR3B, CS30);
    
    // set 256 prescalar  - Servo PWM
    set(TCCR1B, CS12);
    setup_PWM();

    teensy_clockdivide(0); //set the clock speed

    setup_ADC(10);  // ADC10 or PD7
    setup_ADC(6);   // ADC6 or PF6

    unsigned int result1, result2;

    while(1){

        if(TCNT3 < TARGET * PRESCALAR3 / CLOCK_SPEED) continue;

        TCNT3 = 0;

        result2 = read_adc(10); // gives you ADC6
        result1 = read_adc(6);  // gives you ADC10

        result2 *= ((float) MAX_ANG / MAX_ADC);
        result1 *= ((float) MAX_ANG / MAX_ADC);

        #ifdef USB
            m_usb_tx_string("LEFT: ");
            m_usb_tx_uint(result2); 
            m_usb_tx_string("  RIGHT: ");
            m_usb_tx_uint(result1); 
            m_usb_tx_string("\r\n");
        #endif
    }
}



