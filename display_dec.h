/* 
 * File:   display_dec.h
 * Author: home
 *
 * Created on May 31, 2016, 7:37 PM
 */

#ifndef DISPLAY_DEC_H
#define	DISPLAY_DEC_H

#ifdef	__cplusplus
extern "C" {
#endif
    void displayInit(void);
    void displayCtrl(unsigned char x);
    void updateDisplay(unsigned int rowData, unsigned int c1_16, unsigned int c17_32);
    void updateDisplayMstr(unsigned int rowData, unsigned int c1_16, unsigned int c17_32,unsigned int c33_48, unsigned int c49_56);
    void displayLatch(void);
    void printDspl(char* str);
    void adjDigits(char * orig, char * new, unsigned int digits);
    void printDsplMstr( char* str);
    void allDotsONOFF( char action);
    void allDotsONOFFMaster( char action);
    void allMstrDotsONOFF( char action);
    void blinkDspl( char* str, unsigned int mode);
    char * masterDsplString(char*destination , char * source);
#define LAT_BLANK LATBbits.LATB13
#define TRIS_BLANK TRISBbits.TRISB13
#define LAT_LATCH LATBbits.LATB12
#define TRIS_LATCH TRISBbits.TRISB12
#define DISP_ON 0
#define DISP_OFF 1


#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_DEC_H */

