#include "adc.h"
#include "can.h"

#include "digital_keypad.h"
#include "message_id.h"
#include <xc.h>



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
    uint8_t speed = 0;
    uint8_t old = 0;
    uint16_t msg_id;
    uint8_t data, len;
    volatile uint8_t gear = 0;


    while (1) {


        speed = (read_adc(CHANNEL4) / 10.23);

        if(old != speed){
            
            old = speed;
        can_transmit(SPEED_MSG_ID, &speed, sizeof (char));


        for (int del = 100; del--;);
        }

        volatile unsigned char key = read_digital_keypad(STATE_CHANGE);
        if (key == SWITCH1) {
            if(gear == 7)
            {
                gear = 0;
            }
            else if(gear < 6)
            {
                gear++;
            }
        } else if (key == SWITCH2) {
            if(gear == 7)
                gear = 0;
            else if(gear > 0)
            {
                gear--;
            }
        } else if (key == SWITCH3) {
            gear = 7;
        }

        can_transmit(GEAR_MSG_ID, &gear, sizeof (char));
        for (int del = 400; del--;);

        
    }
}

