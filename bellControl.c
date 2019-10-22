#include "include_header.h"
#include <libpic30.h>
//void soundOn(void);
//void beep(void);
void soundOn(void){
    int i = 5;
    while(i>0){
        DING = 1;
        __delay_ms(20);
        
        --i;
    }
    DING = 1; // keep sound OFF
}

void beep(void){

    DING = 1;
    __delay_ms(200);
    DING = 0;
    __delay_ms(10);

}