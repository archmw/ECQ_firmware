/*
 * Check DS for Further Details FCY = FOSC/2 = 80/2 = 40MHz
 */
#include "include_header.h"

void OSC_setup(void) {

    //   OSCCONbits.COSC = 0x00; // FRC as current OSC
    PLLFBDbits.PLLDIV = 0x1E; // M = 30+2 = 32
//    OSCCONbits.COSC = 0x03; // Primary Oscillator (POSC) with PLL (XTPLL, HSPLL, ECPLL) //
//    OSCCONbits.COSC = 0x02; 
//    OSCCONbits.NOSC = 0x03; // XT with PLL as new oscillator
//    CLKDIVbits.DOZE = 0x00; // FCY/1 = 40MHz
    CLKDIVbits.PLLPRE = 0; // N1 = 2
    CLKDIVbits.PLLPOST = 0; // N2 = 2
//    OSCCONbits.COSC = 0b011;
//    while (OSCCONbits.COSC != 0b011);
//    while(!OSCCONbits.LOCK);

}
