

#include "include_header.h"
#include "set_timer.h"
#include "display_dec.h"
#include "EEPROM_h.h"
//#include "Main.c"
extern unsigned int displData[16][3];
extern unsigned int displDataMstr[16][5];
extern unsigned int lineNumber; // Global variable used in timer routine to keep 
// track of which line is to be updated
extern unsigned int dispTmr;
extern unsigned int keyTmr;
extern unsigned int keyFlag;
extern  char  mstrModeFlag, masterMode;


void InitializeTimer() {
    T1CONbits.TON = 0; // Disable Timer
    T1CONbits.TCS = 0; // Select internal instruction cycle clock
    T1CONbits.TGATE = 0; // Disable Gated Timer mode
    T1CONbits.TCKPS = 0b01; // Select 1:8 Prescaler
    TMR1 = 0x00; // Clear timer register
    PR1 = 5000; // Load the period value for 1ms
//    PR1 = 625;
    IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
    IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
    IEC0bits.T1IE = 1; // Enable Timer1 interrupt
    T1CONbits.TON = 1; // Start Timer
}

void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void) {

    if (++keyTmr >= 50) {
        keyTmr = 0;
        keyFlag = 1;
        LATAbits.LATA0 = ~LATAbits.LATA0;
    }
    //    updateDisplay(displData[lineNumber][0], displData[lineNumber][1], displData[lineNumber][2]);
    if (mstrModeFlag)
        updateDisplayMstr(displDataMstr[lineNumber][0], displDataMstr[lineNumber][1], displDataMstr[lineNumber][2], displDataMstr[lineNumber][3], displDataMstr[lineNumber][4]);
    else updateDisplay(displData[lineNumber][0], displData[lineNumber][1], displData[lineNumber][2]);
    if (++lineNumber >= 16)lineNumber = 0;

    IFS0bits.T1IF = 0;
    PR1 = 5000;
//    PR1 = 625;
}