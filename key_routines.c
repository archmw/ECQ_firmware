#include <xc.h>
#include "key_header.h"
void key_init(void) {
    TRISCbits.TRISC0 = 1; // P1Btn
    TRISCbits.TRISC1 = 1; // P2Btn
    TRISCbits.TRISC2 = 1; // P3Btn
    TRISCbits.TRISC3 = 1; // UPbtn
    TRISCbits.TRISC4 = 1; // DOWNbtn
    TRISCbits.TRISC5 = 1; // RESETbtn

}

unsigned int getKey(void) {
    static unsigned int Y1, Y0; // next state
    unsigned int y1, y0; // current state
    unsigned int z; // filter output
    unsigned int x;
    //  unsigned int key=0;
//    x = ~PORTC & 0x3F;
    x = ~PORTC & 0x07 ; // as 3 other buttons of up/dwn/clear has been removed in new design.
    y0 = Y0; // current state is last
    // cycle's next state
    y1 = Y1;
    Y0 = (y1 & y0) | (x & y0) | (x & y1); // compute state
    Y1 = x;
    z = Y0; // compute output
    return z;

}
