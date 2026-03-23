#include "adc.h"
#include "can.h"
//#include "clcd.h"
#include "digital_keypad.h"
#include "message_id.h"
#include <xc.h>

#define LEFT 1
#define RIGHT 2
#define HAZ 3





void init_config() {
    
    PEIE = 1;
    GIE = 1;
   
    
    init_adc();
    init_digital_keypad();
    init_can();
    
}



void main(void) {
    init_config();
    
    unsigned int delay = 0;
    uint8_t rpm = 0;
    uint8_t old = 0;
    uint16_t msg_id;
    uint8_t data, len;
    uint8_t indicator = 0;


    while (1) {


        rpm = (read_adc(CHANNEL4) / 10.23);
        
        if(rpm != old){
        can_transmit(RPM_MSG_ID, &rpm, sizeof (char));
        old = rpm;
        for (int del = 100; del--;);
        }

        volatile unsigned char key = read_digital_keypad(STATE_CHANGE);
        if (key == SWITCH1) {
            indicator = LEFT;
            //RE0 = 1;
        } else if (key == SWITCH2) {
            indicator = RIGHT;
            //RE0 = 1;
        } else if (key == SWITCH3) {
            indicator = HAZ;
            //RE0 = 1;
        } else if (key == SWITCH4) {
            indicator = 0;
            //RE0  = 0;
        }

        can_transmit(INDICATOR_MSG_ID, &indicator, sizeof (char));
        for (int del = 200; del--;);

        
    }
}
