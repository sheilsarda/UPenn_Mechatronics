/* Name: main.c
 * Author: Sheil Sarda
 */

#include "teensy_general.h"
#include "t_usb.h"

int main(void){
    m_usb_init();

    while(!m_usb_isconnected()); // wait for a connection
    while(1){
        m_usb_tx_string("Hello World\r\n");
	teensy_wait(1000);
    }
}

