#include "uart_1.h"
#include <xc.h>
#include "include_header.h"
#define BAUDRATE 19200 
//#define BAUDRATE 38400 
#define BRGVAL   ((FCY/BAUDRATE)/16)-1
unsigned char recvData[15], rangeFlag = 0, recServ[110];
unsigned char Rindex, startFlag, stopFlag, serStopFlag, serRindex, serStartFlag;
//extern unsigned char selSlaveMode;
extern unsigned char recFlag;
extern unsigned int netMode, masterMode, multiService;

void init_uart(void) {
    unsigned int i;
    TRISBbits.TRISB4 = 0; // U1_TX_enable
    TRISBbits.TRISB3 = 0; // TX pin set as output_17-03-2017
    LATBbits.LATB4 = 1; // 06-01-2017 Arch disable it, it controls transmit Enable 
//    LATBbits.LATB4 = 0; // keep transmission ON
    TRISBbits.TRISB2 = 1; //set RB2 pin as input as we are going to map
    // U1Rx pin on RB2 port pin (pin#23)
    RPINR18bits.U1RXR = 2; // RX on RB2
    RPOR1bits.RP3R = 3; //U1TX on RB3,RP3,on pin#23
    // Lock Registers
    //    __builtin_write_OSCCONL(OSCCON | 0x40);
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1MODEbits.BRGH = 0; // Low Speed mode
    U1BRG = BRGVAL; // BAUD Rate Setting for 9600
    //    U1BRG = 129;
    U1STAbits.UTXISEL0 = 0; // Interrupt after one TX character is transmitted
    U1STAbits.UTXISEL1 = 0;
    UARTEnable(); // This function enables UART
 /* wait at least 104?s (1/9600) before sending first char */
    for (i = 0; i < 5208; i++) {
        Nop();
    }
    //U1TXREG = 'a'; 
}

void UARTEnable(void) {

    U1MODEbits.UARTEN = 1; // Enable UART
    U1STAbits.UTXEN = 1; // Enable UART TX
    //    IEC0bits.U1TXIE = 1; // Enable UART TX interrupt  
    IEC0bits.U1RXIE = 1; //Enable RX interrupt
}

void UARTDisable(void) {

    U1MODEbits.UARTEN = 0; // disable UART
    U1STAbits.UTXEN = 0; // disable UART TX
    IEC0bits.U1RXIE = 0; //Disable RX interrupt
}

//void UART1PutChar(char Ch) {
//    //transmit ONLY if TX buffer is empty
//    LATBbits.LATB4 = 0; // Enable TX_EN pin to start transmit
//    __delay_ms(10);
//    while (!U1STAbits.TRMT);
//    U1TXREG = Ch;
//    __delay_ms(10);
//    LATBbits.LATB4 = 1; // Disable TX_EN pin for RS485
//}
//
//void UART1PutString(char *st) {
////    LATBbits.LATB4 = 0; // Enable TX_EN pin to start transmit
////    __delay_us(200);
//    while (*st) {
//        UART1PutChar(*st++);
//    }
////    __delay_ms(3);
////    LATBbits.LATB4 = 1; // Disable TX_EN pin for RS485
//
//}

void UART1PutChar(char Ch) {
    //transmit ONLY if TX buffer is empty
    LATBbits.LATB4 = 0; // Enable TX_EN pin to start transmit
    __delay_us(500);
    while (U1STAbits.UTXBF == 1);
    U1TXREG = Ch;
    __delay_ms(5);
    LATBbits.LATB4 = 1; // Disable TX_EN pin for RS485
}

void UART1PutString(char *st) {
    LATBbits.LATB4 = 0; // Enable TX_EN pin to start transmit
    __delay_us(200);
    while (*st != NULL) {
        //     while (*st){
        while (U1STAbits.UTXBF == 1); // check if Transmit buffer is full or empty
        U1TXREG = *st;
        st++;
    }
    __delay_ms(3);
    LATBbits.LATB4 = 1; // Disable TX_EN pin for RS485

}

void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void) {
    unsigned char temp;
    //    LATAbits.LATA0 = ~LATAbits.LATA0;
    temp = U1RXREG;
//    UART1PutChar(temp);
    if (temp == '$') {
        rangeFlag = 1;
        serStopFlag = 0;
        serRindex = 0;
    }
        //    UART1PutChar(temp); // uncomment it for testing purpose only to see what is received on terminal
        // if master with multiple services, get start and stop count for services in use
    if (multiService == 1 && rangeFlag == 1) {
        if (rangeFlag && temp == '#') {
            if (serRindex > 0) {
                serStopFlag = 1;
                rangeFlag = 0;
            }
        }
        if (serRindex < 106 && rangeFlag == 1) {
            recServ[serRindex++] = temp;
        }
    }        //======================= computer data received ends here ===============================================================
    //=======================response from Keypad for Master/standalone, For slave response from Master ======================    
    else if (netMode && !rangeFlag) {
        LATAbits.LATA0 = ~LATAbits.LATA0;
        if (temp == '*' || recFlag) {
            if (temp != '$') {
                recFlag = 0;
                Rindex = 0;
                startFlag = 1;
                stopFlag = 0;
            }
            
        } else if (temp == '#') {
            if (startFlag == 1 && Rindex > 0) {
                stopFlag = 1;
                startFlag = 0;
            
            }
        }
        if (startFlag == 1 && Rindex < 15) {
            recvData[Rindex++] = temp; // Rindex <10 if space is not included 
    //            UART1PutChar(temp); for testing purpose only
        }
        } else if (!netMode && !rangeFlag) { // rx command for stand alone mode from Keypad
            if (recFlag) {
                recFlag = 0;
                Rindex = 0;
                startFlag = 1;
                stopFlag = 0;
            } else if (temp == '#') {
                if (startFlag == 1 && Rindex > 0) {
                    stopFlag = 1;
                    startFlag = 0;
                }
            }
            if (startFlag == 1 && Rindex < 15) { // 25-03-2017 new command for 16 key and 3 key
                recvData[Rindex++] = temp; // Rindex <10 if space is not included 
    //            UART1PutChar(temp);
            }
        }
        if (rangeFlag && temp == '#' && serRindex <= 0)
            rangeFlag = 0;
    IFS0bits.U1RXIF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; // clear TX interrupt flag
    //    U1TXREG = 'f';                     // Transmit one character
}
