#include <stdio.h>
#include "display_dec.h"
#include "spi.h"
#include <xc.h>
#include "include_header.h"
#include <libpic30.h>

#include "font.h"
#include "uart_1.h"
#define LATCH_DELAY 20   //defines the latch pulse width (max value is 255)
extern unsigned int displData[16][3];
extern unsigned int displDataMstr[16][5];
//unsigned int displDataMstr[16][5] = {
//    {0x0001, 0x7fff, 0x7fff, 0x7fff, 0x7f},
//    {0x0002, 0xbfff, 0xbfff, 0xbfff, 0xbf},
//    {0x0004, 0xdfff, 0xdfff, 0xdfff, 0xdf},
//    {0x0008, 0xefff, 0xefff, 0xefff, 0xef},
//
//    {0x0010, 0xf7ff, 0xf7ff, 0xf7ff, 0xf7},
//    {0x0020, 0xfbff, 0xfbff, 0xfbff, 0xfb},
//    {0x0040, 0xfdff, 0xfdff, 0xfdff, 0xfd},
//    {0x0080, 0xfeff, 0xfeff, 0xfeff, 0xfe},
//
//    {0x0100, 0xff7f, 0xff7f, 0xff7f, 0xff},
//    {0x0200, 0xffbf, 0xffbf, 0xffbf, 0xff},
//    {0x0400, 0xffdf, 0xffdf, 0xffdf, 0xff},
//    {0x0800, 0xffef, 0xffef, 0xffef, 0xff},
//
//    {0x1000, 0xfff7, 0xfff7, 0xfff7, 0xff},
//    {0x2000, 0xfffb, 0xfffb, 0xfffb, 0xff},
//    {0x4000, 0xfffd, 0xfffd, 0xfffd, 0xff},
//    {0x8000, 0xfffe, 0xfffe, 0xfffe, 0xff},
//};

void displayInit(void) {
    TRIS_BLANK = 0; //set blank pin as output
    TRIS_LATCH = 0; // set latch pin as output
    LAT_BLANK = DISP_OFF;
    LAT_LATCH = 0;
}

void displayCtrl(unsigned char x) {
    LAT_BLANK = x;
}

void displayLatch(void) {
    unsigned char i;
    LAT_LATCH = 1; // Latches the data
    for (i = 0; i < LATCH_DELAY; i++);
    LAT_LATCH = 0; //Restoring latch to idle state

}

void updateDisplay(unsigned int rowData, unsigned int c1_16, unsigned int c17_32) {

    displayCtrl(DISP_OFF);
    spi2Write(0xFFFF^c17_32);
    spi2Write(0xFFFF^c1_16);
    spi2Write(rowData);
    displayLatch();
    displayCtrl(DISP_ON);
}

void updateDisplayMstr(unsigned int rowData, unsigned int c1_16, unsigned int c17_32, unsigned int c33_48, unsigned int c49_56) {

    displayCtrl(DISP_OFF);
    spi2Write(0xFFFF^c49_56);
    spi2Write(0xFFFF^c33_48);
    spi2Write(0xFFFF^c17_32);
    spi2Write(0xFFFF^c1_16);
    spi2Write(rowData);
    displayLatch();
    displayCtrl(DISP_ON);
}

/* Show 4 char string on slave display
 */
void printDspl( char* str) {
    unsigned char i, j, sub, myData[4][16];
    j = 0;
    while (*str && j < 4) {
        if (*str <= 0x39 && *str >= 0x30) {
            sub = *str - '0'; //as font table starts with 0 to9
        } else if (*str >= 'A' && *str <= 'Z') {
            sub = (*str - 'A' + 10);
        } else sub = 36;
        for (i = 0; i < 16; i++) {
            myData[j][i] = Font_tab[sub][i];
        }
        str++;
        j++;
    }
    //update the Array with new display data
    for (i = 0; i < 16; i++) {
        displData[i][1] = myData[0][i] | (myData[1][i] << 8);
        displData[i][2] = myData[2][i] | (myData[3][i] << 8);
    }


}

/*This function make string for 2 or 3 digit at center
 */
void adjDigits(char * orig, char * new, unsigned int digits){
    if(digits){ // 3 digit
        new[0] = 0xF;//keep dots off
        new[1] = orig[1];
        new[2] = orig[2];
        new[3] = orig[3];
    }
    else{ // 2 digit 
        new[0] = 0xF;//keep dots off
        new[1] = orig[2];
        new[2] = orig[3];
        new[3] = 0xF;
    }

}

/* shows up long string of 7 digit for master display
 */
void printDsplMstr( char* str) {
    unsigned char i, j, sub, myDataMstr[7][16];
    j = 0;
    while (*str && j < 7) {
        if (*str <= 0x39 && *str >= 0x30) {
            sub = *str - '0'; //as font table starts with 0 to9
        } else if (*str >= 'A' && *str <= 'Z') {
            sub = (*str - 'A' + 10);
        } else if ((j == 4)&&(*str == 46 ))sub = 37;
            else sub = 36;
        
        
        for (i = 0; i < 16; i++) {
            myDataMstr[j][i] = Font_tab[sub][i];
        }
        str++;
        j++;
    }
    //update the Array with new display data
    for (i = 0; i < 16; i++) {
        displDataMstr[i][1] = myDataMstr[0][i] | (myDataMstr[1][i] << 8);
        displDataMstr[i][2] = myDataMstr[2][i] | (myDataMstr[3][i] << 8);
        displDataMstr[i][3] = myDataMstr[4][i] | (myDataMstr[5][i] << 8);
        displDataMstr[i][4] = myDataMstr[6][i] |    0xffff         << 8 ;

    }


}
/* 
 * When 0 is sent all dots will turn ON
 * When 1 is sent all dots will turn OFF
 * Though Char is an input parameter, send it 
 * 0 or 1 in integer values no quotes please
 */
void allDotsONOFF(char action) {
    unsigned char i = 0;
    if (action == 0) { // DISP ON
        for (i = 0; i < 16; i++) {
            displData[i][1] = 0x00 | (0x00 << 8);
            displData[i][2] = 0x00 | (0x00 << 8);
        }
    } else { // DISP OFF
        for (i = 0; i < 16; i++) {
            displData[i][1] = 0xFF | (0xFF << 8); // Matrix 1 (col1-col8)|| matrix 2(col9-16)
            displData[i][2] = 0xFF | (0xFF << 8); // Matrix 1 (col17-col24)|| matrix 2(col25-32)
        }

    }
}
/* To turn all dots ON/OFF for master display
 * When 0 is sent all dots will turn ON
 * When 1 is sent all dots will turn OFF
 * Though Char is an input parameter, send it 
 * 0 or 1 in integer values no quotes please
 */
void allDotsONOFFMaster(char action) {
    unsigned char i = 0;
    if (action == 0) { // DISP ON
        for (i = 0; i < 16; i++) {
            displDataMstr[i][1] = 0x00 | (0x00 << 8);
            displDataMstr[i][2] = 0x00 | (0x00 << 8);
            displDataMstr[i][3] = 0x00 | (0x00 << 8);
            displDataMstr[i][4] = 0x00 | (0x00 << 8);
        }
    } else { // DISP OFF
        for (i = 0; i < 16; i++) {
            displDataMstr[i][1] = 0xFF | (0xFF << 8); // Matrix 1 (col1-col8)|| matrix 2(col9-16)
            displDataMstr[i][2] = 0xFF | (0xFF << 8); // Matrix 1 (col17-col24)|| matrix 2(col25-32)
            displDataMstr[i][3] = 0xFF | (0xFF << 8);
            displDataMstr[i][4] = 0xFF | (0xFF << 8);
        }

    }
}
/*
 *This function was used to measure single LED current/ no LED ON/ ALL LED ON
 * MEasured current All OFF = 0.04A
 * ALL ON = 0.29A
 * Single LED = 0.04A
 * TEST (4 letters) on display = 0.17A
 */
//void allDotsONOFF(unsigned char action) {
//    unsigned char i = 0;
//    if (action == 0) { // DISP ON
//        for (i = 0; i < 16; i++) {
//            displData[i][1] = 0x00 | (0x00 << 8);
//            displData[i][2] = 0x00 | (0x00 << 8);
//        }
//    } else { // DISP OFF
//        for (i = 0; i < 16; i++) {
//            if(!i ){
//                displData[i][1] = 0xFE | (0xFF << 8);
//            }
//            else
//                displData[i][1] = 0xFF | (0xFF << 8);
//            displData[i][2] = 0xFF | (0xFF << 8);
//        }
//
//    }
//}

void allMstrDotsONOFF( char action) {
    unsigned char i = 0;
    if (action == 0) { // DISP ON
        for (i = 0; i < 16; i++) {
            displDataMstr[i][1] = 0x00 | (0x00 << 8);
            displDataMstr[i][2] = 0x00 | (0x00 << 8);
            displDataMstr[i][3] = 0x00 | (0x00 << 8);
            displDataMstr[i][4] = 0x00;
        }
    } else {
        for (i = 0; i < 16; i++) { // DISP OFF
            displDataMstr[i][1] = 0xFF | (0xFF << 8);
            displDataMstr[i][2] = 0xFF | (0xFF << 8);
            displDataMstr[i][3] = 0xFF | (0xFF << 8);
            displDataMstr[i][4] = 0xFF;
        }

    }
}


void blinkDspl( char* str, unsigned int mode) {
    unsigned char len;
    if (!mode) len = 4;
    else len = 7;

    char tempStr[len];

    unsigned int i = 0;

    for (i = 0; i < len; i++) {
        tempStr[i] = *str++;

    }

    i = 0;
    if (!mode) {
        while (i < 3) {
            printDspl(tempStr);
            __delay_ms(800);
             allDotsONOFF(1); // all dots OFF
            __delay_ms(800);
            i++;
        }
    }else{
    while (i < 3) {
            printDsplMstr(tempStr);
            __delay_ms(800);
            allMstrDotsONOFF(1); // all dots OFF
            __delay_ms(800);
            i++;
        }
    
    }


}
/*
 * @param *destination : destination array
 * @param * source: source array
 * return: destination array
 */
char * masterDsplString(char*destination , char * source){
    while(*source != '\0'){
        *destination++ = *source++;
    }
    *destination++ = 0x20;
    *destination++ = 0x20;
    *destination = 0x20;
    return (char*)destination;

}