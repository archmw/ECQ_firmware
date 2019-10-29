/* 
 * File:   Main.c
 * Author: ABC
 * Measured Current (17-10-2016)
 * All OFF = 0.04A
 * ALL ON = 0.29A
 * Single LED = 0.04A
 * TEST (4 letters) on display = 0.17A
 * 18-10-2016 : adding buzzer is yet to implement
 */

#include <stdio.h>
#include <stdlib.h>
#include "MCU_Config.h"
#include "Oscillator.h"
#include "set_timer.h"
#include "include_header.h"
#include "spi.h"
#include "display_dec.h"
#include "uart_1.h"
#include "key_header.h"
#include "num2Ascii_header.h"
#include "EEPROM_h.h"
#include <string.h>
#include "MemoryAddress.h"
#include "bellControl.h"
#include <ctype.h>

#define true 1
#define false 0
unsigned int dispTmr;
unsigned int lineNumber;
unsigned int keyTmr;
unsigned int keyFlag;
unsigned char recFlag;
extern char recvData[15];
extern char recServ[110]; // receive range values for multiple services
char cpyRecvData[15];
extern char Rindex, startFlag, stopFlag, serStopFlag;
extern char rangeFlag;

char validateLastCalledTokenRestore(char *temp);


unsigned int displData[16][3] = {
    {0x0001, 0x7fff, 0x7fff},
    {0x0002, 0xbfff, 0xbfff},
    {0x0004, 0xdfff, 0xdfff},
    {0x0008, 0xefff, 0xefff},

    {0x0010, 0xf7ff, 0xf7ff},
    {0x0020, 0xfbff, 0xfbff},
    {0x0040, 0xfdff, 0xfdff},
    {0x0080, 0xfeff, 0xfeff},

    {0x0100, 0xff7f, 0xff7f},
    {0x0200, 0xffbf, 0xffbf},
    {0x0400, 0xffdf, 0xffdf},
    {0x0800, 0xffef, 0xffef},

    {0x1000, 0xfff7, 0xfff7},
    {0x2000, 0xfffb, 0xfffb},
    {0x4000, 0xfffd, 0xfffd},
    {0x8000, 0xfffe, 0xfffe},
};

unsigned int displDataMstr[16][5] = {
    {0x0001, 0x7fff, 0x7fff, 0x7fff, 0x7fff},
    {0x0002, 0xbfff, 0xbfff, 0xbfff, 0xbfff},
    {0x0004, 0xdfff, 0xdfff, 0xdfff, 0xdfff},
    {0x0008, 0xefff, 0xefff, 0xefff, 0xefff},

    {0x0010, 0xf7ff, 0xf7ff, 0xf7ff, 0xf7ff},
    {0x0020, 0xfbff, 0xfbff, 0xfbff, 0xfbff},
    {0x0040, 0xfdff, 0xfdff, 0xfdff, 0xfdff},
    {0x0080, 0xfeff, 0xfeff, 0xfeff, 0xfeff},

    {0x0100, 0xff7f, 0xff7f, 0xff7f, 0xffff},
    {0x0200, 0xffbf, 0xffbf, 0xffbf, 0xffff},
    {0x0400, 0xffdf, 0xffdf, 0xffdf, 0xffff},
    {0x0800, 0xffef, 0xffef, 0xffef, 0xffff},

    {0x1000, 0xfff7, 0xfff7, 0xfff7, 0xffff},
    {0x2000, 0xfffb, 0xfffb, 0xfffb, 0xffff},
    {0x4000, 0xfffd, 0xfffd, 0xfffd, 0xffff},
    {0x8000, 0xfffe, 0xfffe, 0xfffe, 0xffff},
};

/*
 * 
 */
char TEMP[5] = "TEST", RE_TMP[5] = {'N', 'O', 'T', 'E'};
char rawMasterStr[7], rawSlaveStr[4];
char master_displ_temp[7];
char idStr[4] = "00", recId[4] = "00", cntStr[5] = "000", riStr[4] = "00", cmdRec = '0';
char sendQuery[15];
char mstrModeFlag = 1;
char mstrDisp[8];
unsigned int netMode, masterMode = 1, multiService, mstrSecond = 0;
void makeMstrDsplString(char *cnt, char *di, char *mstrDisp);
unsigned char tempRange1[20][5] = {
    {'A', '0', '0', '1', '\0'},
    {'A', '9', '9', '9', '\0'},
    {'B', '0', '0', '1', '\0'},
    {'B', '9', '9', '9', '\0'},
    {'C', '0', '0', '1', '\0'},
    {'C', '9', '9', '9', '\0'},
    {'D', '0', '0', '1', '\0'},
    {'D', '9', '9', '9', '\0'},
    {'E', '0', '0', '1', '\0'},
    {'E', '9', '9', '9', '\0'},
    {'F', '0', '0', '1', '\0'},
    {'F', '9', '9', '9', '\0'},
    {'G', '0', '0', '1', '\0'},
    {'G', '9', '9', '9', '\0'},
    {'H', '0', '0', '1', '\0'},
    {'H', '9', '9', '9', '\0'},
    {'I', '0', '0', '1', '\0'},
    {'I', '9', '9', '9', '\0'},
    {'J', '0', '0', '1', '\0'},
    {'J', '9', '9', '9', '\0'}};

int main(int argc, char** argv) {
    unsigned int i = 0;
    unsigned int key, keyTemp;
    unsigned int num = 0;
    unsigned int idNum, threeDgt, bellStatus, prgMode = 0;
    char sufChar;
    char* chrPtr, *serPtr;
    char tempRange[20][5], numActiveService[2];
    unsigned int servCountStart[10];
    unsigned int servCountValue[10];
    unsigned int servCountStop[10]; // for multiple service mastermode check start and stop values 
    unsigned long adrs;
    char readBuff[4];
    int newSettings;
    unsigned int funCount, subFunCount;
    /*=========================oscillator start up issue is resolved on 19-03-2017=========================== */
    OSC_setup(); // set up oscillator for 40MHz freq; 
    OSCCONbits.NOSC = 3; // Primary OCS with PLL
    while (!OSCCONbits.LOCK);
    /*================================osicllator issue modification ends here======================================*/
    TRISAbits.TRISA0 = 0; //set led pin as output
    TRISBbits.TRISB14 = 0; // SET BUZZER PIN AS OUTPIT
    TRISC = 0x07; // set RC0,RC1,RC2 = input
    AD1PCFGL = 0xFFFF; // set all ports as Digital  
    key_init();
    sufChar = 'A';
    // blink LED 5 times
    for (i = 0; i < 5; i++) {
        LATAbits.LATA0 = ~LATAbits.LATA0;
        __delay_ms(250);
    }
    // test sound
    beep();
    LATAbits.LATA0 = 0; // LED OFF
    TRISBbits.TRISB4 = 0; //Transmission enabled
    key_init(); // initialize key pins for input
    InitializeTimer(); // it will initialize timer and interrupt at every 1ms.
    // LED at RA0 will blink at 1ms rate visbile on scope
    init_uart();
    spi2Init(); // Init SPI2 for display
    displayInit();
    __delay_ms(100);
    spi1Init(); // Init SPI1 for EEPROM
    Init_EEPROM();
    __delay_ms(100);
//    strcpy(rawMasterStr, "Welcome");
//    UART1PutString(rawMasterStr);
    strcpy(rawMasterStr, "TEST   ");
    printDsplMstr(rawMasterStr); // as per video sent by him
    __delay_ms(1000);
    strcpy(rawMasterStr,"V3.0   "); // states firmware current version in use.
    printDsplMstr(rawMasterStr);
    __delay_ms(1000);
    /*============================= WRITE DEFAULT PARAMETERS IN EEPROM=========================================*/
    unsigned char firstValue = spiEeByteRead(adrNetMode);
    readBuff[0] = spiEeByteRead(adrNetMode);
    readBuff[1] = spiEeByteRead(adrMasterMode);
    readBuff[2] = spiEeByteRead(adrMultiService);
    readBuff[3] = spiEeByteRead(adrIDHi);

    if (firstValue == 0xFF) { // write default parameters when EEPROM is empty
        netMode = 1; // NETWORK MODE ON
        masterMode = 1; // SET AS MASTER DEVICE
        mstrSecond = 0; // no secondary master mode
        multiService = 1; // SET MULTISERVICE
        idNum = 0; // AS MASTER DEVICE HAS ID 00
        uiToStr(idStr, idNum, 4); // CONVERT INT ID TO STRING
        idStr[2] = '0';
        idStr[3] = '0';
        threeDgt = 1; // KEEP COUNT VALUES OF 3 DIGIT
        bellStatus = 1; // KEEP BUZZER ON FOR MASTER DEVICE
        storeData(adrNetMode, netMode); // network mode
        storeData(adrMasterMode, masterMode); // Master/slave mode
        storeData(adrMultiService, multiService); //single service or multiple
        spiEeByteWrite(adrIDHi, idStr[2]); // id MSB
        spiEeByteWrite(adrIDLo, idStr[3]); // id LSB
        storeData(adrThreeDgt, threeDgt); // 2/3 digit
        storeData(adrBellStatus, bellStatus); // bell status
    } else { // if eeprom is not empty, get status of mastermode
        masterMode = charToInteger(readBuff[1]);
        mstrSecond = charToInteger(spiEeByteRead(adrMstrSecond)); 
        if (masterMode && !mstrSecond) mstrModeFlag = 1;
        else if(masterMode && mstrSecond) mstrModeFlag = 2;
        else mstrModeFlag = 0;
    }
    /*==============================DEFAULT PARAMETER WRITING ENDS HERE========================================*/
    // Blink all dots
    i = 0;
    if (masterMode) {
        while (i < 3) {
            allDotsONOFFMaster(0);
            __delay_ms(800);
            allDotsONOFFMaster(1);
            __delay_ms(800);
            i++;
        }
    } else {
        while (i < 3) {
            allDotsONOFF(0);
            __delay_ms(800);
            allDotsONOFF(1);
            __delay_ms(800);
            i++;
        }
    }
    /*=============== EEPROM TEST STARTS HERE ======================*/
    /*   for(adrs = 0x17; adrs <0x1A; adrs++){
               spiEeByteWrite(adrs,writBuff[i++]);
        
           }
           printDspl("WRIT");
           __delay_ms(1000);
           i=0;
           for(adrs = 0x17; adrs <0x1A; adrs++){
               readBuff[i++] = spiEeByteRead(adrs);
           }
           printDspl(readBuff);
           __delay_ms(1000);
           __delay_ms(1000);
     */ // EEPROM read write
    /*================== EEPROM TEST ENDS HERE ===================*/
    i = 0;
    adrs = 0;
    if ((~PORTCbits.RC0)&1) {
        mstrModeFlag = 1; // if enters setting mode, it uses printsDsplMaster, so make mster mode flag 1
        prgMode = 1; // Enters Setting Mode
        newSettings = 1;
        strcpy(rawMasterStr, "PRGM   ");
        printDsplMstr(rawMasterStr); // as per video sent by him
    } else {
        strcpy(rawMasterStr, "NRGM   ");
        printDsplMstr(rawMasterStr); // as per video sent by him
        prgMode = 0;
        newSettings = 0;
    }
    __delay_ms(2000);
    funCount = 7;
    subFunCount =2;
    key = 0;
    while (prgMode) {
        if (keyFlag) {
            keyTemp = key;
            key = getKey();
            if (keyTemp == 0 && key != 0) {
                switch (key) {
                        /*============================================================================*/
                        //                  KEY 1(Function Selection)
                        /*============================================================================*/
                    case 1:
                        if (++funCount > 8)funCount = 0;
//                        if(!masterMode && funCount == 2) ++funCount; // if mode = slave ,skip master secondary option
                        if(masterMode && mstrSecond && funCount==2) ++funCount; // skip single/multiple option in master secondary mode
                        if(masterMode && mstrSecond && funCount==4) ++funCount; // skip digit selection in secondary mode 
                        if (masterMode && multiService && funCount == 4) ++funCount; // no digit selection in multiservice master mode
                        if (masterMode && funCount == 5) ++funCount; // no sufchar selection in mastermode 
                        if (!netMode && funCount == 5) ++funCount; // no sufchar selection for standalone mode
                        if (!netMode && funCount == 2) {
                            ++funCount;
                            multiService = 0;
                        }
                        switch (funCount) {
                            case 0:
                                if (netMode) {
                                    strcpy(rawMasterStr, "NWT    ");
                                    printDsplMstr(rawMasterStr);
                                } else {
                                    strcpy(rawMasterStr, "NWF    ");
                                    printDsplMstr(rawMasterStr);
                                }
                                break;
                            case 1:
                             
                                if (masterMode && !mstrSecond) {
                                    strcpy(rawMasterStr, "MSTR   ");
                                    printDsplMstr(rawMasterStr);
                                } else if (masterMode && mstrSecond) {
                                    strcpy(rawMasterStr, "SECOND");
                                    printDsplMstr(rawMasterStr);
                                }
                                else if (netMode && !masterMode) {
                                    strcpy(rawMasterStr, "SLAV   ");
                                    printDsplMstr(rawMasterStr);
                                } else {
                                    strcpy(rawMasterStr, "STDA  ");
                                    printDsplMstr(rawMasterStr);
                                }// STANDALONE MODE

                                break;
//                            case 2:
//                                if (masterMode) {
//                                    printDsplMstr("SECOND ");
//                                }
//                                break;
                            case 2: // SKIP IN STANDALONE
                                if (multiService) {
                                    strcpy(rawMasterStr, "MULT   ");
                                    printDsplMstr(rawMasterStr);
                                } else {
                                    strcpy(rawMasterStr, "SING   ");
                                    printDsplMstr(rawMasterStr);
                                }
                                break;
                            case 3:
                                if (masterMode) idNum = 0;
                                uiToStr(TEMP, idNum, 4);
                                TEMP[0] = 'I';
                                TEMP[1] = 'D';
                                strcpy(rawMasterStr, TEMP);
                                rawMasterStr[4] = 0x20;
                                rawMasterStr[5] = 0x20;
                                rawMasterStr[6] = 0x20;
                                printDsplMstr(rawMasterStr);
                                break;
                            case 4:
                                if (threeDgt) {
                                    strcpy(rawMasterStr, "DIG3   ");
                                    printDsplMstr(rawMasterStr);
                                } else {
                                    strcpy(rawMasterStr, "DIG2   ");
                                    printDsplMstr(rawMasterStr);
                                }
                                break;
                            case 5:// SKIP IN STANDALONE
                                TEMP[0] = 'S';
                                TEMP[1] = 'U';
                                TEMP[2] = 'F';
                                TEMP[3] = sufChar;
                                strcpy(rawMasterStr, TEMP);
                                rawMasterStr[4] = 0x20;
                                rawMasterStr[5] = 0x20;
                                rawMasterStr[6] = 0x20;
                                printDsplMstr(rawMasterStr);
                                break;
                            case 6:
                                if (bellStatus) {
                                    strcpy(rawMasterStr, "BELT   ");
                                    printDsplMstr(rawMasterStr);
                                } else {
                                    strcpy(rawMasterStr, "BELF   ");
                                    printDsplMstr(rawMasterStr);
                                }
                                break;
                            case 7:
                                strcpy(rawMasterStr, "PRMS   ");
                                printDsplMstr(rawMasterStr);
                                break;
                        }//switch(funCount) ends here
                        break;
                        /*============================================================================*/
                        //                  KEY 2
                        /*============================================================================*/
                    case 2:
                        switch (funCount) {
                            case 0:
                                strcpy(rawMasterStr, "NWT    ");
                                printDsplMstr(rawMasterStr);
                                netMode = 1;
                                break;
                            case 1:
                                if(++subFunCount > 1) subFunCount = 0;
                                if (netMode && !subFunCount) {
                                    strcpy(rawMasterStr, "MSTR   ");
                                    printDsplMstr(rawMasterStr);
                                    masterMode = 1;
                                    mstrSecond = 0;
                                } else if (netMode && (subFunCount == 1)){
                                    strcpy(rawMasterStr, "SECNDRY");
                                    printDsplMstr(rawMasterStr);
                                    masterMode = 1;
                                    mstrSecond = 1;
                                    
                                }
                                else {
                                    strcpy(rawMasterStr, "STDA   ");
                                    printDsplMstr(rawMasterStr);
                                    masterMode = 0;
                                }
                                break;
//                            case 2:
////                                if(masterMode){
////                                    printDsplMstr("SECOND");
////                                    mstrSecond = 1;
////                                    multiService = 1;
////                                }
//                                break;
                            case 2:
                                strcpy(rawMasterStr, "MULT   ");
                                printDsplMstr(rawMasterStr);
                                multiService = 1;
                                break;
                            case 3:
                                if (masterMode) {
                                    idNum = 0;
                                    uiToStr(TEMP, idNum, 4);
                                    TEMP[0] = 'I';
                                    TEMP[1] = 'D';
                                    strcpy(rawMasterStr, TEMP);
                                    rawMasterStr[4] = 0x20;
                                    rawMasterStr[5] = 0x20;
                                    rawMasterStr[6] = 0x20;
                                    printDsplMstr(rawMasterStr);
                                } else {
                                    if (++idNum > 32)idNum = 1;
                                    uiToStr(TEMP, idNum, 4);
                                    TEMP[0] = 'I';
                                    TEMP[1] = 'D';
                                    strcpy(rawMasterStr, TEMP);
                                    rawMasterStr[4] = 0x20;
                                    rawMasterStr[5] = 0x20;
                                    rawMasterStr[6] = 0x20;
                                    printDsplMstr(rawMasterStr);
                                }
                                break;
                            case 4:
                                strcpy(rawMasterStr, "DIG2   ");
                                printDsplMstr(rawMasterStr);
                                threeDgt = 0;
                                break;
                            case 5:
                                if (multiService) { // if multiple service is used, choose sufchar
                                    if (++sufChar > 'Z')sufChar = 'A';
                                    TEMP[0] = 'S';
                                    TEMP[1] = 'U';
                                    TEMP[2] = 'F';
                                    TEMP[3] = sufChar;
                                    strcpy(rawMasterStr, TEMP);
                                    rawMasterStr[4] = 0x20;
                                    rawMasterStr[5] = 0x20;
                                    rawMasterStr[6] = 0x20;
                                    printDsplMstr(rawMasterStr);
                                } else {
                                    sufChar = '0'; //for single service sufchar is always 0
                                    TEMP[0] = 'S';
                                    TEMP[1] = 'U';
                                    TEMP[2] = 'F';
                                    TEMP[3] = sufChar;
                                    strcpy(rawMasterStr, TEMP);
                                    rawMasterStr[4] = 0x20;
                                    rawMasterStr[5] = 0x20;
                                    rawMasterStr[6] = 0x20;
                                    printDsplMstr(rawMasterStr);
                                }
                                break;
                            case 6:
                                strcpy(rawMasterStr, "BELT   ");
                                printDsplMstr(rawMasterStr);
                                bellStatus = 1;
                                break;
                            case 7:
                                strcpy(rawMasterStr, "SAVE   ");
                                printDsplMstr(rawMasterStr);
                                __delay_ms(1000);
                                __delay_ms(1000);
                                /*========================WRITE ALL SETTINGS TO EEPROM HERE===================*/
                                if (!netMode) sufChar = 0xF; // keep it empty
                                uiToStr(idStr, idNum, 4); // int ID to String
                                storeData(adrNetMode, netMode); // network mode
                                storeData(adrMasterMode, masterMode); // Master/slave mode
                                storeData(adrMultiService, multiService); //single service or multiple
                                spiEeByteWrite(adrIDHi, idStr[2]); // id MSB
                                spiEeByteWrite(adrIDLo, idStr[3]); // id LSB
                                storeData(adrThreeDgt, threeDgt); // 2/3 digit
                                spiEeByteWrite(adrSufChar, sufChar); // sufchar
                                storeData(adrBellStatus, bellStatus); // bell status
                                storeData(adrMstrSecond,mstrSecond); // secondary master status, if mstrSecond = 1 secondary master mode selected
                                /*========================EEPROM WRITE ENDS HERE==============================*/
                                prgMode = 0;
                                break;
                        }//switch(funCount) ends here
                        break;
                        /*============================================================================*/
                        //                  KEY 3
                        /*============================================================================*/
                    case 4:
                        switch (funCount) {
                            case 0:
                                strcpy(rawMasterStr, "NWF    ");
                                printDsplMstr(rawMasterStr);
                                netMode = 0;
                                break;
                            case 1:
                                if (netMode) {
                                    strcpy(rawMasterStr, "SLAV   ");
                                    printDsplMstr(rawMasterStr);
                                } else {
                                    strcpy(rawMasterStr, "STDA  ");
                                    printDsplMstr(rawMasterStr);
                                }// STANDALONE MODE
                                masterMode = 0;
                                break;
//                            case 2:
////                                if(masterMode){
////                                    printDsplMstr("PRIMARY"); // keep master primary
////                                    mstrSecond = 0;
////                                }
//                                break;
                            case 2:
                                if (netMode) {
                                    strcpy(rawMasterStr, "SING   ");
                                    printDsplMstr(rawMasterStr);
                                    multiService = 0;
                                } else { // for standalone mode,single service will mot be there, it uses multiple service
                                    strcpy(rawMasterStr, "MULT   ");
                                    printDsplMstr(rawMasterStr); // as standalone mode needs suffix with count so, it is kept multiple services
                                    multiService = 1;
                                }
                                break;
                            case 3:
                                if (masterMode) {
                                    idNum = 0;
                                    uiToStr(TEMP, idNum, 4);
                                    TEMP[0] = 'I';
                                    TEMP[1] = 'D';
                                    strcpy(rawMasterStr, TEMP);
                                    rawMasterStr[5] = 0x20;
                                    rawMasterStr[6] = 0x20;
                                    rawMasterStr[7] = 0x20;
                                    printDsplMstr(rawMasterStr);
                                } else {
                                    if (idNum == 1)idNum = 32;
                                    else idNum--;
                                    uiToStr(TEMP, idNum, 4);
                                    TEMP[0] = 'I';
                                    TEMP[1] = 'D';
                                    strcpy(rawMasterStr, TEMP);
                                    rawMasterStr[5] = 0x20;
                                    rawMasterStr[6] = 0x20;
                                    rawMasterStr[7] = 0x20;
                                    printDsplMstr(TEMP);
                                }

                                break;
                            case 4:
                                strcpy(rawMasterStr, "DIG3   ");
                                printDsplMstr(rawMasterStr);
                                threeDgt = 1;
                                break;
                            case 5:
                                if (multiService) { // if Multiservice is used take suffix from user
                                    if (--sufChar < 'A')sufChar = 'Z';
                                    TEMP[0] = 'S';
                                    TEMP[1] = 'U';
                                    TEMP[2] = 'F';
                                    TEMP[3] = sufChar;
                                    strcpy(rawMasterStr, TEMP);
                                    rawMasterStr[5] = 0x20;
                                    rawMasterStr[6] = 0x20;
                                    rawMasterStr[7] = 0x20;
                                    printDsplMstr(rawMasterStr);

                                } else { // if single service is used, suffix is always '0' - ZERO
                                    sufChar = '0';
                                    TEMP[0] = 'S';
                                    TEMP[1] = 'U';
                                    TEMP[2] = 'F';
                                    TEMP[3] = sufChar;
                                    strcpy(rawMasterStr, TEMP);
                                    rawMasterStr[5] = 0x20;
                                    rawMasterStr[6] = 0x20;
                                    rawMasterStr[7] = 0x20;
                                    printDsplMstr(rawMasterStr);
                                }
                                break;
                            case 6:
                                strcpy(rawMasterStr, "BELF   ");
                                printDsplMstr(rawMasterStr);
                                bellStatus = 0;
                                break;
                            case 7:
                                strcpy(rawMasterStr, "CANS   ");
                                printDsplMstr(rawMasterStr);
                                prgMode = 0;
                                break;
                        }//switch(funCount) ends here

                        break;
                }//switch(key) ends here

            }//if(keyTemp==0 && key!=0) ends here
            keyFlag = 0;
        }//if(keyFlag) ends here
    }//while(prgMode) ends here

    /*==============READ ALL SETTINGS FROM EEPROM HERE============================*/
    if (!masterMode) { // SLAVE OR STANDALONE MODE
        mstrModeFlag = 0;
        printDspl("OUT ");
        __delay_ms(1000);
        readBuff[0] = 'N';
        readBuff[1] = 'E';
        readBuff[2] = 'T';
        readBuff[3] = spiEeByteRead(adrNetMode);
        netMode = charToInteger(readBuff[3]);
        if (!masterMode && netMode)
            printDspl("SLAV");
        else
            printDspl("STDA");
        __delay_ms(1000);
        // read for multiple or single
        if (netMode) {
            readBuff[3] = spiEeByteRead(adrMultiService);
            multiService = charToInteger(readBuff[3]);
            if (multiService) {
                printDspl("MULT");
                __delay_ms(1000);
                readBuff[0] = 'S';
                readBuff[1] = 'U';
                readBuff[2] = 'F';
                readBuff[3] = spiEeByteRead(adrSufChar);
                sufChar = spiEeByteRead(adrSufChar);
                printDspl(readBuff);

            } else {
                sufChar = '0';
                printDspl("SING");
            }

            __delay_ms(1000);
        }//else sufChar = 0X20; // STANDALONE SUFCHAR = 0
        //        }else sufChar = 0X20; // STANDALONE SUFCHAR = SPACE
        readBuff[0] = 'I';
        readBuff[1] = 'D';
        readBuff[2] = spiEeByteRead(adrIDHi);
        idStr[2] = spiEeByteRead(adrIDHi);
        readBuff[3] = spiEeByteRead(adrIDLo);
        idStr[3] = spiEeByteRead(adrIDLo);
        printDspl(readBuff);
        idNum = (charToInteger(readBuff[2]) << 4)&0xf0;
        idNum |= charToInteger(readBuff[3])&0x0f;
        __delay_ms(1000);
        readBuff[0] = 'D';
        readBuff[1] = 'I';
        readBuff[2] = 'G';
        readBuff[3] = spiEeByteRead(adrThreeDgt);
        threeDgt = charToInteger(readBuff[3]);
        if (threeDgt)
            printDspl("DIG3");
        else
            printDspl("DIG2");
        __delay_ms(1000);
        readBuff[0] = 'B';
        readBuff[1] = 'E';
        readBuff[2] = 'L';
        readBuff[3] = spiEeByteRead(adrBellStatus);
        bellStatus = charToInteger(readBuff[3]);
        if (bellStatus)
            printDspl("BON ");
        else
            printDspl("BOFF");
    } else if(masterMode && !mstrSecond) { // primary master mode
        mstrModeFlag = 1;
        printDsplMstr("OUT    ");
        __delay_ms(1000);
        readBuff[0] = 'N';
        readBuff[1] = 'E';
        readBuff[2] = 'T';
        readBuff[3] = spiEeByteRead(adrNetMode);
        netMode = charToInteger(readBuff[3]);
        readBuff[0] = 'M';
        readBuff[1] = 'A';
        readBuff[2] = 'S';
        readBuff[3] = spiEeByteRead(adrMasterMode);
        masterMode = charToInteger(readBuff[3]);
        printDsplMstr("PRIMARY");
        __delay_ms(1000);
        __delay_ms(1000);
        // read for multiple or single
        readBuff[3] = spiEeByteRead(adrMultiService);
        multiService = charToInteger(readBuff[3]);
        if (multiService) printDsplMstr("MULTIPL");
        else printDsplMstr("SINGLE ");
        __delay_ms(1000);
        __delay_ms(1000);
        printDsplMstr("       "); // clear display to show ID
        readBuff[0] = 'I';
        readBuff[1] = 'D';
        readBuff[2] = spiEeByteRead(adrIDHi);
        idStr[2] = spiEeByteRead(adrIDHi);
        readBuff[3] = spiEeByteRead(adrIDLo);
        idStr[3] = spiEeByteRead(adrIDLo);
        strncpy(rawMasterStr,readBuff,4);
        strncat(rawMasterStr,"   ",3);
        printDsplMstr(rawMasterStr);
        idNum = (charToInteger(readBuff[2]) << 4)&0xf0;
        idNum |= charToInteger(readBuff[3])&0x0f;
        __delay_ms(1000);
        __delay_ms(1000);
        readBuff[0] = 'D';
        readBuff[1] = 'I';
        readBuff[2] = 'G';
        readBuff[3] = spiEeByteRead(adrThreeDgt);
        threeDgt = charToInteger(readBuff[3]);
        if(!multiService){
            if(readBuff[3] == '0')
                readBuff[3] = '2';
            else if(readBuff[3] == '1')
                readBuff[3] = '3';
            strncpy(rawMasterStr,readBuff,4);
            strncat(rawMasterStr,"   ",3);
            printDsplMstr(rawMasterStr);
            __delay_ms(1000);
            __delay_ms(1000);
        }
        
        
        readBuff[0] = 'B';
        readBuff[1] = 'E';
        readBuff[2] = 'L';
        readBuff[3] = spiEeByteRead(adrBellStatus);
        bellStatus = charToInteger(readBuff[3]);
        if (bellStatus)
            printDsplMstr("BELL ON");
        else
            printDsplMstr("BEL OFF");
    }
    else if(masterMode && mstrSecond){ // secondary master mode
        mstrModeFlag = 2;
        printDsplMstr("OUT    ");
        __delay_ms(1000);
        readBuff[0] = 'N';
        readBuff[1] = 'E';
        readBuff[2] = 'T';
        readBuff[3] = spiEeByteRead(adrNetMode);
        netMode = charToInteger(readBuff[3]);
        readBuff[0] = 'M';
        readBuff[1] = 'A';
        readBuff[2] = 'S';
        readBuff[3] = spiEeByteRead(adrMasterMode);
        masterMode = charToInteger(readBuff[3]);
        printDsplMstr("SECOND "); // secondary master
        __delay_ms(1000);
        __delay_ms(1000);
        printDsplMstr("       "); // clear display to show ID
        readBuff[0] = 'I';
        readBuff[1] = 'D';
        readBuff[2] = spiEeByteRead(adrIDHi);
        idStr[2] = spiEeByteRead(adrIDHi);
        readBuff[3] = spiEeByteRead(adrIDLo);
        idStr[3] = spiEeByteRead(adrIDLo);
        strncpy(rawMasterStr,readBuff,4);
        strncat(rawMasterStr,"   ",3);
        printDsplMstr(rawMasterStr);
        idNum = (charToInteger(readBuff[2]) << 4)&0xf0;
        idNum |= charToInteger(readBuff[3])&0x0f;
        __delay_ms(1000);
        __delay_ms(1000);
        readBuff[0] = 'B';
        readBuff[1] = 'E';
        readBuff[2] = 'L';
        readBuff[3] = spiEeByteRead(adrBellStatus);
        bellStatus = charToInteger(readBuff[3]);
        if (bellStatus)
            printDsplMstr("BELL ON");
        else
            printDsplMstr("BEL OFF");
    }
    /*==============READ FROM EEPROM ENDS HERE=====================================*/
    __delay_ms(1000);
    /*============================================================================*/
    if (netMode == 0)goto STAND_ALONE;
    else if (mstrModeFlag == 1)goto MASTER_MODE;
    else if(mstrModeFlag == 2) goto MASTER_SECONDARY_MODE;
    /*============================================================================*/
    /*                      SLAVE MODE                                            */
    /*     DI_RI_count_S_#                                                        */
    /*    DI_RI_count_# modified as in keypad it is internal buzzer to ON/OFF     */
    /* VERSION 2: *<SPACE>DI<SPACE>RI<SPACE>COUNTVALUE<SPACE># = 14 bytes         */
    /*            1  2    34  5    67  8    9ABC      D      E                    */
    /* Note 3: 17-oct-2019: disable print commands to                             */
    /* check if this resolves issue of                                            */
    /* garbage value display when a slave is added in system when system is kept  */
    /* ON                                                                         */
    /*============================================================================*/
   
    printDspl("SLAV"); //refer Note 3
    __delay_ms(2000);
    mstrModeFlag = 0;
    if (!newSettings) { // if newsettings are not called, load last saved number on display
        TEMP[0] = spiEeByteRead(adrLastNumSuf); // memory location 8
        TEMP[1] = spiEeByteRead(adrLastNumC1);
        TEMP[2] = spiEeByteRead(adrLastNumC2);
        TEMP[3] = spiEeByteRead(adrLastNumC3);
        if (TEMP[0] == sufChar) { // check if last stored num in eeprom has same value of prefix
            printDspl(TEMP);
            if (threeDgt) {
                num = strCountUiCount(TEMP, 4);
            } else {
                num = strCountUiCount(TEMP, 3);
            }
        } else num = 0;
    }
    while (1) {
        recFlag = 0; // keep it in slave mode
        if (stopFlag) {
            for (i = 0; i < 14; i++) {
                cpyRecvData[i] = recvData[i]; // copy recvdata to other array
                recvData[i] = 0; // clear data, so as to receive new data
            }
            stopFlag = 0;
            chrPtr = cpyRecvData;
            i = 0;
            chrPtr++; // discard *
            chrPtr++; // discard <space> after *
            while (* chrPtr != ' ') { //DI
                recId[i++] = *chrPtr++;

            }
            recId[3] = 0;
            i = 0;
            chrPtr++;
            while (* chrPtr != ' ') { // RI
                riStr[i++] = *chrPtr++;
            }
            riStr[3] = 0;
            i = 0;
            chrPtr++; // as we need suffix from count value
            while (* chrPtr != ' ') { // count Value
                cntStr[i++] = *chrPtr++;

            }
            if (cntStr[0] == sufChar && recId[0] == idStr[2] && recId[1] == idStr[3]) {
                for (i = 0; i < 4; i++) {
                    TEMP[i] = cntStr[i];
                }
                if (bellStatus) beep();
                blinkDspl(TEMP, masterMode);
                printDspl(TEMP);
                eeWriteArr(adrLastNumSuf, TEMP, 4); // write num to eeprom, from location 8: 8-->TEMP[0] .. 11-->TEMP[3]
            }
            for (i = 0; i < 13; i++)cpyRecvData[i] = 0; //clears the receive data 
        }//if(stopFlag) ends here
    }//while(1) ends here
    /*=================SLAVE MODE ENDS HERE============================================*/
    /*=================MASTER SECONDARY MODE STARTS HERE============================================*/
MASTER_SECONDARY_MODE:
                 
    printDsplMstr("SECOND ");
    __delay_ms(2000);
    if(!newSettings){
        for (adrs = adrLastNumSuf; adrs < 15; adrs++) {
                mstrDisp[adrs - 8] = spiEeByteRead(adrs);
        }
        printDsplMstr(mstrDisp);
    }
    else{
        printDsplMstr("XXXX.XX");
        
    }
    __delay_ms(1000);
    while (1) {
        recFlag = 0; // we need to process commands which start with * so keep recFlag = 0 as in slave mode
        if (stopFlag) {
            for (i = 0; i < 14; i++) {
                cpyRecvData[i] = recvData[i]; // copy recvdata to other array
                recvData[i] = 0; // clear data, so as to receive new data
            }
            stopFlag = 0;
            chrPtr = cpyRecvData;
            i = 0;
            chrPtr++; // discard *
            chrPtr++; // discard <space> after *
            while (* chrPtr != ' ') { //DI
                recId[i++] = *chrPtr++;

            }
            i = 0;
            chrPtr++;
            while (* chrPtr != ' ') { // RI
                riStr[i++] = *chrPtr++;
            }
            
            i = 0;
            chrPtr++; // as we need suffix from count value
            while (* chrPtr != ' ') { // count Value
                cntStr[i++] = *chrPtr++;

            }
            makeMstrDsplString(cntStr, recId, mstrDisp);
            eeWriteArr(adrLastNumSuf, mstrDisp, 7);
            if (bellStatus) beep();
            blinkDspl(mstrDisp, masterMode);
            printDsplMstr(mstrDisp);
            for (i = 0; i < 5; i++) cntStr[i] = 0;
            for (i = 0; i < 13; i++)cpyRecvData[i] = 0; //clears the receive data 
        }//if(stopFlag) ends here
    }//while(1) ends here                         
    /*=================MASTER SECONDARY MODE ENDS HERE============================================*/
    
                                
    /*==================================================================================*/
    /*                  STAND_ALONE MODE                                                */
    /*               Senthil sends command in following format                          */
    /* For 3 key remote: command received over serial is: @DIRIXXXC*                    */
    /* where  @ -start byte, DI  -Device ID, RI  -Reomte ID, XXX -token number (000-999)*/
    /* C   -command Next||Previous||Clear, *   -End byte. Example:@0102555N*            */
    /* 25-03-2017 change in receive command from keypad to operate it with 3-key and    */
    /* 16 key remote, with 16 key remote it will only process N,C,P commands            */
    /* New command DI<SPACE>RI<SPACE>0XXX<SPACE>C<SPACE>#                               */
    /*             12 3     45 6     789A B     C  D    E = 14BYTES COMMAND                                                 */
    /* example: 02 01 0112 N #                                                                   */
    /*==================================================================================*/
STAND_ALONE:

    mstrModeFlag = 0;
    printDspl("STDA");
//    UART1PutString("STANDALONE\n");
    __delay_ms(1000);
    if (!newSettings) { // if new settings are not called, load last saved number on display
        TEMP[1] = spiEeByteRead(adrLastNumC1);
        TEMP[2] = spiEeByteRead(adrLastNumC2);
        TEMP[3] = spiEeByteRead(adrLastNumC3);
        adjDigits(TEMP, RE_TMP, threeDgt);
        printDspl(RE_TMP);
        num = strCountUiCount(TEMP, 4);
        if (threeDgt) {
            if (num < 0 || num > 999)
                num = 0;
        } else {
            if (num < 0 || num > 99)
                num = 0;
        }
    }// read settings from memory ends here
    else {
        num = 0;
        uiToStr(TEMP, num, 4);
        adjDigits(TEMP, RE_TMP, threeDgt);
        printDspl(RE_TMP);
    }
    recFlag = 1;
    while (1) {
        if (stopFlag) {
            for (i = 0; i < 15; i++) {
                cpyRecvData[i] = recvData[i]; // copy recvdata to other array
                recvData[i] = 0; // clear data, so as to receive new data
            }
            stopFlag = 0;
            i = 0;
            chrPtr = cpyRecvData;
            i = 0;
            while (* chrPtr != ' ') { //DI
                recId[i++] = *chrPtr++;
            }
//            UART1PutString(recId);
            i = 0;
            chrPtr++;
            while (* chrPtr != ' ') { // RI
                riStr[i++] = *chrPtr++;
            }
            riStr[3] = '\0';
            i = 0;
            chrPtr++; // as we need suffix from count value
            while (* chrPtr != ' ') { // count Value
                cntStr[i++] = *chrPtr++;
            }
            chrPtr++;
            while (* chrPtr != ' ') cmdRec = *chrPtr++;
            recFlag = 1;
            switch (cmdRec) { // process Commands 'N', 'P', 'C'
                case 0x4E: // 'N'
                    if (threeDgt) {
                        if (++num <= 999);
                        else num = 0;
                        uiToStr(TEMP, num, 4);
                    } else {
                        if (++num <= 99);
                        else num = 0;
                        uiToStr(TEMP, num, 4);
                    }
                    break; //Next cmd received
                case 0x50: //'P'
                    if (threeDgt) {
                        if (num == 0)num = 999;
                        else num--;
                        uiToStr(TEMP, num, 4);

                    } else {
                        if (num == 0)num = 99;
                        else num--;
                        uiToStr(TEMP, num, 4);

                    }
                    break; // case Previous received
                case 0x43: // 'C'
                    num = 0;
                    if (threeDgt) uiToStr(TEMP, num, 4);
                    else {
                        uiToStr(TEMP, num, 4);
                        //                        TEMP[3] = 0x20;
                    }
                    break;
                default:
                    printDspl("ERR ");
                    break;

            }// switch case
            adjDigits(TEMP, RE_TMP, threeDgt);
            makeQuerry(recId, riStr, TEMP);
            UART1PutString(sendQuery); // send data to update LCD display of 16 key remote for standalone
            if (bellStatus) beep();
            blinkDspl(RE_TMP, 0);
            printDspl(RE_TMP);
            eeWriteArr(adrLastNumSuf, TEMP, 4); // write num to eeprom, from location 8: 8-->TEMP[0] .. 11-->TEMP[3]
            for (i = 0; i < 15; i++) {
                cpyRecvData[i] = 0; // clear array
            }

        }// stop flag ends here
    }//while standalone ends here
    /*Do not disable Serial comm as standalone now uses RS485 Remote keypad*/
    /*=========================== STANDALONE MODE ENDS HERE ========================================================*/

    /*==============================================================================================================*/
    /*                             MASTER MODE                                                                      */
    /*                                                                                                              */
    /*=============================================================================================================*/
MASTER_MODE:
    __delay_ms(1000);
    unsigned int tempNum = 0, emerNum = 0, j = 0, numOfService, recIDint, riIDint, servTempNum[10], servEmerNum[10];
    char tempCnt[5], withinRange;
    
    
    char serviceIDPtr[10]; // multipleCase;
    unsigned char readAddr;
    int multipleCase;
    mstrModeFlag = 1;
    printDsplMstr("MASTER ");
    __delay_ms(1000);
    if (!multiService) sufChar = '0';
    if (!newSettings) { // if new settings are not called, load last saved number on display
        if (multiService) { // if multiservices get start and stop values
            numActiveService[0] = spiEeByteRead(adrNumOfServiceLo);
            numActiveService[1] = spiEeByteRead(adrNumOfServiceHi);
            numOfService = strToUiPartial(numActiveService, 2);
            uiToStr(numActiveService, numOfService, 2);
            for (j = 0; j < (numOfService * 2); j++) {
                for (i = 0; i < 4; i++) {
                    TEMP[i] = spiEeByteRead(adrSer1StartSuf + (i)+(j * 4));
                    *(*(tempRange + j) + i) = TEMP[i]; // copy data to temp_range, it will be used later
                }
// ==============GET COUNT RANGE UPPER AND LOWER VALUES FROM STRING TO INTEGER ===============================
                if (j % 2 == 0) {
                    servCountStart[j-(j/2)] = strCountUiCount(TEMP, 4);
                    serviceIDPtr[j - (j / 2)] = TEMP[0]; //get list of serviceID
                } else if (j % 2 == 1) {
                    servCountStop[(j-1)-(j/2)] = strCountUiCount(TEMP, 4);
                }
                TEMP[i] = '\0'; // add null to end string format of count value
                *(*(tempRange + j) + i) = TEMP[i]; 
                *(*(tempRange + j) + i)-=1; // first count should start from A201 instead of A202
                masterDsplString(RE_TMP, TEMP);
                printDsplMstr(RE_TMP);// send token start and stop values to display
                __delay_ms(1000);
            }
            strncpy(TEMP, serviceIDPtr, numOfService);
            //printDsplMstr(TEMP);
            *serviceIDPtr = setServiceList(numOfService,serviceIDPtr);
//            printDsplMstr(serviceIDPtr);
//            __delay_ms(1000);
//            for (i = 0; i < (numOfService); i++) {
//                servCountValue[i] = servCountStart[i]; // Keep it 0 so as to start it from 1 
//                                                          //say if start values is A101 so here count value would be 
//                                                         //100 as first N command will be received
//            }
        } // multiple services ends here
//============================================================================================================
//======================Restore last num from memory ========================================================= 
        if (!multiService) {
            for (adrs = adrLastNumSuf; adrs < 15; adrs++) {
                mstrDisp[adrs - 8] = spiEeByteRead(adrs);
            }
            num =strToUiPartial(mstrDisp,4);
//            num = strToUi(tempCnt);
            printDsplMstr(mstrDisp);
            __delay_ms(1000)
            serviceIDPtr[0] = sufChar;
            *serviceIDPtr = setServiceList(1,serviceIDPtr);
        } 
        else {
            i = 0;
            long addr1 = newSettingFlag;
            long addr2 = lastServiceLoad;
//            for(i = 0; i<5;i++){
//                readBuff[i] = spiEeByteRead(addr1++); // read memory to check if new service flag is there
//            }
//            eeWriteArr(addr1,"10",2);
//            __delay_ms(500);
            readBuff[0] = spiEeByteRead(addr1);
            readBuff[1] = spiEeByteRead(++addr1);
            int flagNum = strToUiPartial(readBuff,1);
//          read last service load mem location, to verify if there was any token called before restart of board,
//            useful during power failure and to avoid read of junk values 
            readBuff[0] = spiEeByteRead(addr2);
            readBuff[1] = spiEeByteRead(++addr2);
            int lastServiceLoadFlag = strToUiPartial(readBuff,1);
//            itoa(rawMasterStr,flagNum,10); // flagNum value not to show on display so put it in comment
//            strcat(rawMasterStr,"      ");
//            printDsplMstr(rawMasterStr);
//            __delay_ms(1000);
            
            if(!flagNum){ // when no new service list is there reload old token values from memory
//                printDsplMstr("OLD FUL");
//                __delay_ms(1000);
                for (j = 0; j < numOfService; j++) {
                    for (adrs = adrSufSer1; adrs < adrSufSer2; adrs++)
                        mstrDisp[i++] = spiEeByteRead((adrs + (j * 8)));
                    i = 0;
                    const char *servID = strchr(serviceIDPtr,mstrDisp[0]); // check if memory read has same service ID then load last called token
                    if(servID && !lastServiceLoadFlag ){
                        eeWriteArr(lastServiceLoad,"11",2); // no last service was served
                        
                        strncpy(tempCnt, mstrDisp, 4);
                        
//                        servCountValue[j] = strToUi(tempCnt);
//                        if (servCountValue[j] < servCountStart[j] || servCountValue[j] > servCountStop[j])
//                            servCountValue[j] = servCountStart[j] - 1;
//                        printDsplMstr(mstrDisp);
//                        __delay_ms(1000);
                        if(validateLastCalledTokenRestore(tempCnt) == true){
                            servCountValue[j] = strToUi(tempCnt);
                            if (servCountValue[j] < servCountStart[j] || servCountValue[j] > servCountStop[j])
                                servCountValue[j] = servCountStart[j] - 1;
                            printDsplMstr(mstrDisp);
                            __delay_ms(1000);
                        } else{
//                            UART1PutString("* invalid string #");
                        }
/*===============================this does not show any garbage value but last stored value gets
 * restored to start value of token=====================================================================
 *                         if( validateLastCalledTokenRestore(tempCnt) == false){
                            strcpy(tempCnt,"0000");
                            //strncpy(mstrDisp,"0000",4);
                            UART1PutString("* invalid string #");
                        }
                        else{
                            servCountValue[j] = strToUi(tempCnt);
//                      check if last recalled value is out of range, set it equal to token start value
                            if (servCountValue[j] < servCountStart[j] || servCountValue[j] > servCountStop[j])
                                servCountValue[j] = servCountStart[j] - 1;
                            printDsplMstr(mstrDisp);
                            __delay_ms(1000);
                        }
==============================================================================================================*/
//                        servCountValue[j] = strToUi(tempCnt);
////                      check if last recalled value is out of range, set it equal to token start value
//                        if (servCountValue[j] < servCountStart[j] || servCountValue[j] > servCountStop[j])
//                            servCountValue[j] = servCountStart[j] - 1;
                        
                    }
                    

                }
            } // read new service flag ends here
            else{
                
                printDsplMstr("XXXX.XX");
                __delay_ms(1000);
                eeWriteArr(newSettingFlag,"01",2); // 01 to put flag off for next restart
                for (j = 0; j < numOfService; j++) {
                    servCountValue[j] = servCountStart[j] - 1;
                    
                }
            }
        }
    }// !newSettings ends here
    else { // if new settings are there, display 0000->00 for master 
        printDsplMstr("XXXX.XX"); // show XXXX on display when no data is given previously
        __delay_ms(1000);
        long addr = 0x0020;
        if (multiService) {
            readAddr = spiEeByteRead(addr); // read 1 byte from address of service list
            /*==================================================================================*/
            /* IF NEW SETTINGS ARE DONE FOR MASTER MODE MULTIPLE SERVICES,                      */
            /* AND NO SERVICE RANGE IS PROVIDED, IT WILL TAKE DEFAULT                           */
            /* VALUES OF SERVICE ID FROM A TO J WITH COUNT OF 000 TO 999 AND WRITE IT TO EEPROM */
            /* SO VALUES WILL BE A000 TO A999, B000 TO B999 ... J000 TO J999                    */
            /*==================================================================================*/
            if (readAddr == EEPROM_LOCATION_EMPTY) { //check if read byte from start of service list is '#',
                // location is empty
                printDsplMstr("       "); //clean the display
                // By default there will be 10(max) number of services active
                numActiveService[0] = '1';
                numActiveService[1] = '0';
                spiEeByteWrite(adrNumOfServiceLo, numActiveService[0]);
                spiEeByteWrite(adrNumOfServiceHi, numActiveService[1]);
                numOfService = strToUiPartial(numActiveService, 2);
//                serviceIDPtr = (char *)malloc(numOfService); // allocate array size according to number of service ID count value
                for (i = 0; i < (numOfService * 2); i++) { // write all service start and stop values to EEPROM 
                    // max number of services = 10
                    for (j = 0; j < 5; j++) {
                        TEMP[j] = *(*(tempRange1 + i) + j); // get single element from 2D array using pointer
                        *(*(tempRange + i) + j) = TEMP[j];
                    }
                    //strCpyArr(tempRange[i][0],TEMP); // copy contents to working register
                    if (i % 2 == 0) {
                        servCountStart[i-(i/2)] = strCountUiCount(TEMP, 4); // this function automatically drops alphabet prefix from count
                        serviceIDPtr[i - (i / 2)] = TEMP[0]; // load service ID to array

                    } else if (i % 2 == 1) {
                        servCountStop[(i-1)-(i/2)] = strCountUiCount(TEMP, 4);

                    }
                    eeWriteArr(adrSer1StartSuf + (i * 4), TEMP, 4); // write service to EEPROM
                    TEMP[4] = '\0'; // add null value to temp so as to detect string end;
                    masterDsplString(master_displ_temp, TEMP);
                    printDsplMstr(master_displ_temp); // display service on Matrix
                    __delay_ms(1000);
                }
                *serviceIDPtr = setServiceList(numOfService,serviceIDPtr);
                printDsplMstr("DEFAULT");
                __delay_ms(1000);
                printDsplMstr("LISTSET");
                __delay_ms(1000);

            } else { // if first byte from service list is not empty, read EEPROM to retrieve service list.
                printDsplMstr("       ");
                numActiveService[0] = spiEeByteRead(adrNumOfServiceLo);
                numActiveService[1] = spiEeByteRead(adrNumOfServiceHi);
                numOfService = strToUiPartial(numActiveService, 2);
                uiToStr(numActiveService, numOfService, 2);
                for (j = 0; j < (numOfService * 2); j++) {
                    for (i = 0; i < 4; i++) {
                        TEMP[i] = spiEeByteRead(adrSer1StartSuf + (i)+(j * 4));
                        *(*(tempRange + j) + i) = TEMP[i];
                    }
//-------------------LOAD service start and stop values as received from eeprom -------------------------------------------------
                    if (j % 2 == 0) {
                        servCountStart[j-(j/2)] = strCountUiCount(TEMP, 4);
                        serviceIDPtr[j - (j / 2)] = TEMP[0]; // load service ID to array
                    } else if (j % 2 == 1) {
                        servCountStop[(j-1)-(j/2)] = strCountUiCount(TEMP, 4);
                    }
                    TEMP[i] = '\0'; // add null to end string format of count value         
//                    /*======================================================================*/
//                    /* Note 5: to resolve garbage value issue for unused service,           */
//                    /* before showing value on display we will convert string token number  */
//                    /* to int value and check if int value is a valid number or not         */
//                    /* if it is not a valid number, display 0000.                           */
////                    int tokenNumToCount = strCountUiCount(TEMP,4);
////                    int i;
//////                    if(tokenNumToCount >= 1000 ){ // check if num is invalid, write 000 as a count value
//////                        for(i = 1; i < 4; i++)
//////                            TEMP[i] = '0';
//////                    }
////                    if((tokenNumToCount > 0) && (tokenNumToCount <1000) ){ // check if num is valid, write 000 as a count value for invalid number
////                        //do nothing
////                    }
////                    else{
////                        for(i = 1; i < 4; i++)
////                            TEMP[i] = '0';
////                    }
//                    //==========================2nd option==================================*/
//                    
//                    /* check each character, if valid character is found, display it else   */
//                    /* throw 0000 on display                                                */
//                    int i = 0, count = 0;
//                    char flag_invalid = false;
//                    
//               
//                    while(i<4){
//                        if(isalnum(TEMP[i]) != 1){
//                            flag_invalid = true;
//                           break;
//                           
//                        }
//                        else{
//                            count++;
//                        }
//                        i++;
//                    }
//                    if(flag_invalid == true){
//                        strcpy(TEMP,"0000\0");
//                        UART1PutString("* invalid String #");
//                    }
//                   
//                    
//                
//                    //==========================2nd option ends here =======================*/
//                    /*====================== Note5 ends here ===============================*/
                    masterDsplString(master_displ_temp, TEMP);
                    printDsplMstr(master_displ_temp);
                    __delay_ms(1000);
                }
                *serviceIDPtr = setServiceList(numOfService,serviceIDPtr);
                printDsplMstr("SERLIST");
                __delay_ms(1000);
                printDsplMstr("LOADED ");
                __delay_ms(1000);
                i = 0;
                for (j = 0; j < numOfService; j++) {
                    for (adrs = adrSufSer1; adrs < adrSufSer2; adrs++)
                        mstrDisp[i++] = spiEeByteRead((adrs + (j * 8)));
                    i = 0;
                    if(strchr(serviceIDPtr,mstrDisp[0])){ // check if token retrieved is in service list 
                        strncpy(tempCnt, mstrDisp, 4);
                        servCountValue[j] = strToUi(tempCnt);
                        //                  check if last recalled value is out of range, set it equal to serv start value
                        if (servCountValue[j] < servCountStart[j] || servCountValue[j] > servCountStop[j])
                            servCountValue[j] = servCountStart[j] - 1;
                    }
                }
            }// default values ends here
        }//multiservice ends here
        else if(!multiService){
            serviceIDPtr[0] = sufChar;
            *serviceIDPtr = setServiceList(1,serviceIDPtr);
        }
    }// new settings ends here
    recFlag = 1;
    while (1) {
        if (serStopFlag) { //  check if there is change in count range for any of services is received on RX of MCY 
            j = 0; // row
            i = 0; // column
            serPtr = recServ; // assign pointer to array
            ++serPtr;
            ++serPtr;
            while (*serPtr != ' ') {
                numActiveService[i++] = *serPtr++; // get num of active service
            }
            numActiveService[3] = '\0';
            numOfService = strToUiPartial(numActiveService, 2);
            eeWriteArr(adrNumOfServiceLo, numActiveService, 2); // write number of services to EEPROM
            i = 0;
            for (j = 0; j < (numOfService*2); j++) {
                ++serPtr; //skip space
                while (*serPtr != ' ') {
                    TEMP[i] = *serPtr++;
                    *(*(tempRange + j) + i++) = TEMP[i];
//                    UART1PutChar(*(*(tempRange + j)+ (i++)));
                }
                //============ get upper count and lower count of all services sent by pc ==================================================
                if (j % 2 == 0) {
                    servCountStart[j-(j/2)] = strCountUiCount(TEMP, 4);
                    serviceIDPtr[j-(j/2)] = TEMP[0]; // load service ID to array
                    
                } else if (j % 2 == 1) {
                    servCountStop[(j-1)-(j/2)] = strCountUiCount(TEMP, 4);
                }
//=========== Lower and Upper count values ends here =====================================================   
                eeWriteArr(adrSer1StartSuf + (j * 4), TEMP, 4);
                eeWriteArr(newSettingFlag, "11", 2); // raise flag in memory,when new setting is received on terminal
                i = 0;
            }
//            clear previously written memory location as new service list is received
            for(i = 0;i<10;i++){
                eeWriteArr((adrSufSer1 + (8 * i)), EEPROM_LOCATION_EMPTY, 7);
            }
            *serviceIDPtr = setServiceList(numOfService,serviceIDPtr);
//=====================Put start values in counter ============================================================            
            for (i = 0; i < (numOfService); i++){
                servCountValue[i] = servCountStart[i] - 1; // Keep it 0 so as to start it from 1 
                                                          //say if start values is A101 so here count value 
                                                         //would be 100 as first N command will be received
//                uiToStr(TEMP,servCountValue[i],4);
//                masterDsplString(RE_TMP,TEMP);
//                printDsplMstr(RE_TMP);
//                __delay_ms(1500);
            }
            printDsplMstr("SER SAV"); // to acknowledge user new services are saved
//            UART1PutString("SER SAV");
            __delay_ms(1000);
            for (i = 0; i < 110; i++)
                recServ[i] = 0; // Clear buffer array
            serStopFlag = 0; // clear stop flag;
            recFlag = 1;
        }//                                                   01 2     34 5     6789       10    11  12   13
        else if (stopFlag) { // response received from keypad DI<space>RI<space>countValue<space>CMD<space>#
            for (i = 0; i < 15; i++) {
                cpyRecvData[i] = recvData[i]; // copy recvdata to other array
                __delay_us(100); // requires delay else complete command is not received properly
                recvData[i] = 0; // clear data, so as to receive new data
            }
            stopFlag = 0;
            chrPtr = cpyRecvData;
            i = 0;
            while (* chrPtr != ' ') { //DI
                recId[i++] = *chrPtr++;
            }
            recId[3] = '\0';
            recIDint = strToUi(recId); // convert str form Display ID to int
            i = 0;
            chrPtr++;
            while (* chrPtr != ' ') { // RI
                riStr[i++] = *chrPtr++;
            }
            riStr[3] = '\0';
            i = 0;
            riIDint = strToUi(riStr);
            chrPtr++; // as we need suffix from count value
            while (* chrPtr != ' ') { // count Value
                cntStr[i++] = *chrPtr++;
            }
            sufChar = cntStr[0]; // get sufchar from received command ; used in multiple services
            chrPtr++;
            while (* chrPtr != ' ') cmdRec = *chrPtr++;
            recFlag = 1;
            if (!multiService && cntStr[0] != '0') {
                goto MSTRCLEAR;
            } else{
                withinRange = 1;
                                
            }
            if (multiService) {
                i = 0;
                j = 0;
                for (j = 0; j < (numOfService * 2); j++) {
                    if (j % 2 == 0) {
                        if (*(*(tempRange + j) + 0) == cntStr[0]) {
                            if (j > 0)
                                multipleCase = j / 2;
                            if (j == 0)
                                multipleCase = j;
                        }
                    }
                }
            }
            const char servPTR = strchr(serviceIDPtr, cntStr[0]); // check if serviceID is within set list 
            if (riIDint > 0 && riIDint < 33 && recIDint > 0 && recIDint < 33 && servPTR) { // to check if received ID is within range from 01 to 32
                switch (multiService) {
                    case 0:
                        switch (cmdRec) { // process Commands 'N', 'P', 'C','E','R','D'
                            case 0x4E: // 'N'
                                if (threeDgt) {
                                    if (++num <= 999);
                                    else num = 0;
                                    if (num == emerNum && num != 0) // to check if this has been already called in emergency case
                                        ++num;

                                } else {
                                    if (++num <= 99);
                                    else num = 0;
                                    if (num == emerNum && num != 0) // to check if this has be called for emergency
                                        ++num;
                                }
                                break; //Next cmd received
                            case 0x50: //'P'
                                if (threeDgt) {
                                    if (num == 0)num = 999;
                                    else num--;
                                    if (num == emerNum && num != 0) --num;
                                } else {
                                    if (num == 0)num = 99;
                                    else num--;
                                    if (num == emerNum && num != 0) --num;
                                }
                                break; // case Previous received
                            case 0x44:// 'D'
                                num = strToUi(cntStr);

                                break; // for direct number call
                            case 0x45: //'E'
                                tempNum = num; // store it in temporary variable
                                num = strToUi(cntStr);
                                emerNum = num; // save emergency number called
                                break;
                            case 0x52: // 'R'
                                tempNum = num;
                                num = strToUi(cntStr);
                                break; // repeat number
                            case 0x43: // 'C'
                                num = 0;
                                break; // clear count value

                        }// switch case single service ends here

                        break;
                    case 1: // if multiple services are there
                        switch (multipleCase) {
                            case 0:
                                switch (cmdRec) { // process Commands 'N', 'P', 'C','E','R','D'
                                    case 0x4E: // 'N'
                                        if ((servCountValue[multipleCase] < servCountStop[multipleCase])&& (servCountValue[multipleCase] >= servCountStart[multipleCase] - 1))
                                            servCountValue[multipleCase]++;
                                        else if (servCountValue[multipleCase] == servCountStop[multipleCase])
                                            servCountValue[multipleCase] = servCountStart[multipleCase];
                                        else if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                            servCountValue[multipleCase] = servCountStart[multipleCase];
                                        if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                            servCountValue[multipleCase]++;
                                    break; //Next cmd received
                                    case 0x50: //'P'
                                        if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                            servCountValue[multipleCase] = servCountStop[multipleCase];
                                        else if (servCountValue[multipleCase] > servCountStart[multipleCase])
                                            servCountValue[multipleCase]--;
                                        if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                            servCountValue[multipleCase]--;
                                    break; // case Previous received
                                    case 0x44:// 'D'
                                        servCountValue[multipleCase] = strToUiPartial(cntStr, 4);
                                    break; // for direct number call
                                    case 0x45: //'E'
                                        servTempNum[multipleCase] = servCountValue[multipleCase]; // store it in temporary variable
                                        servCountValue[multipleCase] = strToUi(cntStr);
                                        servEmerNum[multipleCase] = servCountValue[multipleCase]; // save emergency number called
                                    break;
                                    case 0x52: // 'R'
                                        servTempNum[multipleCase] = servCountValue[multipleCase];
                                        servCountValue[multipleCase] = strToUi(cntStr);
                                    break; // repeat number
                                    case 0x43: // 'C'
                                        servCountValue[multipleCase] = servCountStart[multipleCase]-1;
                                    break; // clear count value
                                }// switch case for cmdrec ends here
                                break;
                                case 1:
                                    switch (cmdRec) { // process Commands 'N', 'P', 'C','E','R','D'
                                        case 0x4E: // 'N'
                                            if ((servCountValue[multipleCase] < servCountStop[multipleCase])&& (servCountValue[multipleCase] >= servCountStart[multipleCase] - 1))
                                                servCountValue[multipleCase]++;
                                            else if (servCountValue[multipleCase] == servCountStop[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            else if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]++;
                                        break; //Next cmd received
                                        case 0x50: //'P'
                                            if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStop[multipleCase];
                                            else if (servCountValue[multipleCase] > servCountStart[multipleCase])
                                                servCountValue[multipleCase]--;
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]--;
                                        break; // case Previous received
                                        case 0x44:// 'D'
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                        break; // for direct number call
                                        case 0x45: //'E'
                                            servTempNum[multipleCase] = servCountValue[multipleCase]; // store it in temporary variable
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            servEmerNum[multipleCase] = servCountValue[multipleCase]; // save emergency number called
                                            break;
                                        case 0x52: // 'R'
                                            servTempNum[multipleCase] = servCountValue[multipleCase];
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            break; // repeat number
                                        case 0x43: // 'C'
                                            servCountValue[multipleCase] = servCountStart[multipleCase] - 1;
                                            break; // clear count value
                                    }// switch case for cmdrec ends here
                                    break;
                                case 2:
                                    switch (cmdRec) { // process Commands 'N', 'P', 'C','E','R','D'
                                        case 0x4E: // 'N'
                                            if ((servCountValue[multipleCase] < servCountStop[multipleCase])&& (servCountValue[multipleCase] >= servCountStart[multipleCase] - 1))
                                                servCountValue[multipleCase]++;
                                            else if (servCountValue[multipleCase] == servCountStop[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            else if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]++;
                                        break; //Next cmd received
                                        case 0x50: //'P'
                                            if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStop[multipleCase];
                                            else if (servCountValue[multipleCase] > servCountStart[multipleCase])
                                                servCountValue[multipleCase]--;
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]--;
                                        break; // case Previous received
                                        case 0x44:// 'D'
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                        break; // for direct number call
                                        case 0x45: //'E'
                                            servTempNum[multipleCase] = servCountValue[multipleCase]; // store it in temporary variable
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            servEmerNum[multipleCase] = servCountValue[multipleCase]; // save emergency number called
                                        break;
                                        case 0x52: // 'R'
                                            servTempNum[multipleCase] = servCountValue[multipleCase];
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            break; // repeat number
                                        case 0x43: // 'C'
                                            servCountValue[multipleCase] = servCountStart[multipleCase] - 1;
                                            break; // clear count value
                                    }// switch case for cmdrec ends here
                                    break;
                                case 3:
                                    switch (cmdRec) { // process Commands 'N', 'P', 'C','E','R','D'
                                        case 0x4E: // 'N'
                                            if ((servCountValue[multipleCase] < servCountStop[multipleCase])&& (servCountValue[multipleCase] >= servCountStart[multipleCase] - 1))
                                                servCountValue[multipleCase]++;
                                            else if (servCountValue[multipleCase] == servCountStop[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            else if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]++;
                                        break; //Next cmd received
                                        case 0x50: //'P'
                                            if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStop[multipleCase];
                                            else if (servCountValue[multipleCase] > servCountStart[multipleCase])
                                                servCountValue[multipleCase]--;
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]--;
                                        break; // case Previous received
                                        case 0x44:// 'D'
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                        break; // for direct number call
                                        case 0x45: //'E'
                                            servTempNum[multipleCase] = servCountValue[multipleCase]; // store it in temporary variable
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            servEmerNum[multipleCase] = servCountValue[multipleCase]; // save emergency number called
                                        break;
                                        case 0x52: // 'R'
                                            servTempNum[multipleCase] = servCountValue[multipleCase];
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                        break; // repeat number
                                        case 0x43: // 'C'
                                            servCountValue[multipleCase] = servCountStart[multipleCase] - 1;
                                        break; // clear count value
                                    }// switch case for cmdrec ends here
                                    break;
                                case 4:
                                    switch (cmdRec) { // process Commands 'N', 'P', 'C','E','R','D'
                                        case 0x4E: // 'N'
                                            if ((servCountValue[multipleCase] < servCountStop[multipleCase])&& (servCountValue[multipleCase] >= servCountStart[multipleCase] - 1))
                                                servCountValue[multipleCase]++;
                                            else if (servCountValue[multipleCase] == servCountStop[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            else if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]++;
                                            break; //Next cmd received
                                        case 0x50: //'P'
                                            if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStop[multipleCase];
                                            else if (servCountValue[multipleCase] > servCountStart[multipleCase])
                                                servCountValue[multipleCase]--;
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]--;
                                            break; // case Previous received
                                        case 0x44:// 'D'
                                            servCountValue[multipleCase] = strToUi(cntStr);

                                            break; // for direct number call
                                        case 0x45: //'E'
                                            servTempNum[multipleCase] = servCountValue[multipleCase]; // store it in temporary variable
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            servEmerNum[multipleCase] = servCountValue[multipleCase]; // save emergency number called
                                            break;
                                        case 0x52: // 'R'
                                            servTempNum[multipleCase] = servCountValue[multipleCase];
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            break; // repeat number
                                        case 0x43: // 'C'
                                            servCountValue[multipleCase] = servCountStart[multipleCase] - 1;
                                            break; // clear count value

                                    }// switch case for cmdrec ends here
                                    break;
                                case 5:
                                    switch (cmdRec) { // process Commands 'N', 'P', 'C','E','R','D'
                                        case 0x4E: // 'N'
                                            if ((servCountValue[multipleCase] < servCountStop[multipleCase])&& (servCountValue[multipleCase] >= servCountStart[multipleCase] - 1))
                                                servCountValue[multipleCase]++;
                                            else if (servCountValue[multipleCase] == servCountStop[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            else if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]++;
                                            break; //Next cmd received
                                        case 0x50: //'P'
                                            if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStop[multipleCase];
                                            else if (servCountValue[multipleCase] > servCountStart[multipleCase])
                                                servCountValue[multipleCase]--;
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]--;
                                            break; // case Previous received
                                        case 0x44:// 'D'
                                            servCountValue[multipleCase] = strToUi(cntStr);

                                            break; // for direct number call
                                        case 0x45: //'E'
                                            servTempNum[multipleCase] = servCountValue[multipleCase]; // store it in temporary variable
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            servEmerNum[multipleCase] = servCountValue[multipleCase]; // save emergency number called
                                            break;
                                        case 0x52: // 'R'
                                            servTempNum[multipleCase] = servCountValue[multipleCase];
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            break; // repeat number
                                        case 0x43: // 'C'
                                            servCountValue[multipleCase] = servCountStart[multipleCase] - 1;
                                            break; // clear count value

                                    }// switch case for cmdrec ends here
                                    break;
                                case 6:
                                    switch (cmdRec) { // process Commands 'N', 'P', 'C','E','R','D'
                                        case 0x4E: // 'N'
                                            if ((servCountValue[multipleCase] < servCountStop[multipleCase])&& (servCountValue[multipleCase] >= servCountStart[multipleCase] - 1))
                                                servCountValue[multipleCase]++;
                                            else if (servCountValue[multipleCase] == servCountStop[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            else if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]++;
                                            break; //Next cmd received
                                        case 0x50: //'P'
                                            if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStop[multipleCase];
                                            else if (servCountValue[multipleCase] > servCountStart[multipleCase])
                                                servCountValue[multipleCase]--;
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]--;
                                            break; // case Previous received
                                        case 0x44:// 'D'
                                            servCountValue[multipleCase] = strToUi(cntStr);

                                            break; // for direct number call
                                        case 0x45: //'E'
                                            servTempNum[multipleCase] = servCountValue[multipleCase]; // store it in temporary variable
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            servEmerNum[multipleCase] = servCountValue[multipleCase]; // save emergency number called
                                            break;
                                        case 0x52: // 'R'
                                            servTempNum[multipleCase] = servCountValue[multipleCase];
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            break; // repeat number
                                        case 0x43: // 'C'
                                            servCountValue[multipleCase] = servCountStart[multipleCase] - 1;
                                            break; // clear count value

                                    }// switch case for cmdrec ends here
                                    break;
                                case 7:
                                    switch (cmdRec) { // process Commands 'N', 'P', 'C','E','R','D'
                                        case 0x4E: // 'N'
                                            if ((servCountValue[multipleCase] < servCountStop[multipleCase])&& (servCountValue[multipleCase] >= servCountStart[multipleCase] - 1))
                                                servCountValue[multipleCase]++;
                                            else if (servCountValue[multipleCase] == servCountStop[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            else if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]++;
                                            break; //Next cmd received
                                        case 0x50: //'P'
                                            if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStop[multipleCase];
                                            else if (servCountValue[multipleCase] > servCountStart[multipleCase])
                                                servCountValue[multipleCase]--;
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]--;
                                            break; // case Previous received
                                        case 0x44:// 'D'
                                            servCountValue[multipleCase] = strToUi(cntStr);

                                            break; // for direct number call
                                        case 0x45: //'E'
                                            servTempNum[multipleCase] = servCountValue[multipleCase]; // store it in temporary variable
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            servEmerNum[multipleCase] = servCountValue[multipleCase]; // save emergency number called
                                            break;
                                        case 0x52: // 'R'
                                            servTempNum[multipleCase] = servCountValue[multipleCase];
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            break; // repeat number
                                        case 0x43: // 'C'
                                            servCountValue[multipleCase] = servCountStart[multipleCase] - 1;
                                            break; // clear count value

                                    }// switch case for cmdrec ends here
                                    break;
                                case 8:
                                    switch (cmdRec) { // process Commands 'N', 'P', 'C','E','R','D'
                                        case 0x4E: // 'N'
                                            if ((servCountValue[multipleCase] < servCountStop[multipleCase])&& (servCountValue[multipleCase] >= servCountStart[multipleCase] - 1))
                                                servCountValue[multipleCase]++;
                                            else if (servCountValue[multipleCase] == servCountStop[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            else if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]++;
                                            break; //Next cmd received
                                        case 0x50: //'P'
                                            if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStop[multipleCase];
                                            else if (servCountValue[multipleCase] > servCountStart[multipleCase])
                                                servCountValue[multipleCase]--;
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]--;
                                            break; // case Previous received
                                        case 0x44:// 'D'
                                            servCountValue[multipleCase] = strToUi(cntStr);

                                            break; // for direct number call
                                        case 0x45: //'E'
                                            servTempNum[multipleCase] = servCountValue[multipleCase]; // store it in temporary variable
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            servEmerNum[multipleCase] = servCountValue[multipleCase]; // save emergency number called
                                            break;
                                        case 0x52: // 'R'
                                            servTempNum[multipleCase] = servCountValue[multipleCase];
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            break; // repeat number
                                        case 0x43: // 'C'
                                            servCountValue[multipleCase] = servCountStart[multipleCase] - 1;
                                            break; // clear count value

                                    }// switch case for cmdrec ends here
                                    break;
                                case 9:
                                    switch (cmdRec) { // process Commands 'N', 'P', 'C','E','R','D'
                                        case 0x4E: // 'N'
                                            if ((servCountValue[multipleCase] < servCountStop[multipleCase])&& (servCountValue[multipleCase] >= servCountStart[multipleCase] - 1))
                                                servCountValue[multipleCase]++;
                                            else if (servCountValue[multipleCase] == servCountStop[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            else if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStart[multipleCase];
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]++;
                                            break; //Next cmd received
                                        case 0x50: //'P'
                                            if (servCountValue[multipleCase] <= servCountStart[multipleCase])
                                                servCountValue[multipleCase] = servCountStop[multipleCase];
                                            else if (servCountValue[multipleCase] > servCountStart[multipleCase])
                                                servCountValue[multipleCase]--;
                                            if (servCountValue[multipleCase] == servEmerNum[multipleCase])
                                                servCountValue[multipleCase]--;
                                            break; // case Previous received
                                        case 0x44:// 'D'
                                            servCountValue[multipleCase] = strToUi(cntStr);

                                            break; // for direct number call
                                        case 0x45: //'E'
                                            servTempNum[multipleCase] = servCountValue[multipleCase]; // store it in temporary variable
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            servEmerNum[multipleCase] = servCountValue[multipleCase]; // save emergency number called
                                            break;
                                        case 0x52: // 'R'
                                            servTempNum[multipleCase] = servCountValue[multipleCase];
                                            servCountValue[multipleCase] = strToUi(cntStr);
                                            break; // repeat number
                                        case 0x43: // 'C'
                                            servCountValue[multipleCase] = servCountStart[multipleCase] - 1;
                                            break; // clear count value

                                    }// switch case for cmdrec ends here
                        break;
                        }
                break;
                }// multiservices switch case ends here
            if (multiService) {
                if (servCountValue[multipleCase] > servCountStart[multipleCase] && servCountValue[multipleCase] < servCountStop[multipleCase])
                    uiToStr(cntStr, servCountValue[multipleCase], 4);
                else if (cmdRec == 'C'){ // if clear cmd is received set token count value to 000 i.e. for e.g. A100 as token start value is A101
                    uiToStr(cntStr, servCountValue[multipleCase], 4);
                }else {
                    servCountValue[multipleCase] = servCountStart[multipleCase];
                    uiToStr(cntStr, servCountValue[multipleCase], 4);
                }
                if (cmdRec == 'E' || cmdRec == 'R') servCountValue[multipleCase] = servTempNum[multipleCase]; // restore count value back
            } else {
                uiToStr(cntStr, num, 4);
                if (cmdRec == 'E' || cmdRec == 'R') num = tempNum; //restore num value from tempNum
            }
            cntStr[0] = sufChar;
            makeMstrDsplString(cntStr, recId, mstrDisp);
            makeQuerry(recId, riStr, cntStr);
            UART1PutString(sendQuery); // send data to update LCD of Keypad and salve Display
            if (bellStatus) beep();
            blinkDspl(mstrDisp, masterMode);
            printDsplMstr(mstrDisp);
            for (i = 0; i < 5; i++) cntStr[i] = 0;
            if (!multiService)
                eeWriteArr(adrLastNumSuf, mstrDisp, 7);
            else {
//                eeWriteArr(newSettingFlag,"01",2); // 01 to put flag off for next restart, so if only data was written then only it 
                  eeWriteArr(lastServiceLoad,"01",2);   // it shows last called values else it will show empty location for last called value.
                  __delay_ms(5);
                eeWriteArr((adrSufSer1 + (8 * multipleCase)), mstrDisp, 7);
            }
            }// limit check loop ends here
            else{
                /* Note 4: as discussed with senthil on 19th oct 2019,                                   */
                /* if ERROR is shown on display , we will send * 00 00 EEEE # cmd                        */
                /* from master board so that keypad can understand that cmd has been failed              */
                /* and keypad needs to resend this cmd. This is because when a slave board is            */
                /* is connected in a already Powered ON system, first command sent from keypad fails.    */
                /* next cmd onwards system works well. so to avoid missing a cmd, we found this solution.*/
//                const unsigned char errMsg[14] = "* 00 00 EEEE #";
                printDsplMstr(" ERROR ");
                UART1PutString("* 00 00 EEEE #"); // on error ask keypad to resend your cmd. refer Note 4
//                UART1PutString(errMsg);
                __delay_ms(1000);
            }
MSTRCLEAR:
        for (i = 0; i < 15; i++)cpyRecvData[i] = 0; //clear  received data   
        __delay_ms(1000);
        
        }//if(stopFlag) ends here

}//while(1) ends here for master mode
return (EXIT_SUCCESS);
} //main() ends here 

void makeMstrDsplString(char *cnt, char *di, char *mstrDspl) {
    unsigned char i;
    for (i = 0; i < 4; i++) {
        *mstrDspl = *cnt++;
        mstrDspl++;

    }
    *mstrDspl = '.'; // added to show ->(arrow) between count num and dI
    mstrDspl++;
    for (i = 0; i < 2; i++) {
        *mstrDspl = *di++;
        mstrDspl++;
    }
}

/* @param string to validate
 * @return boolean value true (1) if string is valid
 */
char validateLastCalledTokenRestore(char *temp){
    int i = 0, count = 0;
    while(i<4){
        if(isalnum(temp[i]) != 1){
            return false;
        }
        else{
            count++;
        }
        i++;
    }
    return true;
}