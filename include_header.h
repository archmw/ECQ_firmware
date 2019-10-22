/* 
 * File:   include_header.h
 * Author: ABC
 *
 * 
 */

#ifndef INCLUDE_HEADER_H
#define	INCLUDE_HEADER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
    //Definitions
    //#define __XTAL_FREQ 1000000
    // #define FCY 5000000UL
    //#define __XTAL_FREQ 40000000
#define FCY 40000000UL // 40MIPS
//#define FCY 5000000UL // 10MIPS
#define DING    LATBbits.LATB14 // RINGS DING DONG BELL
                                // 0-> BELL ON
                                // 1-> BELL OFF

#include <libpic30.h>
#include <stdio.h>
#include <stdlib.h>

    //Function Prototypes    
    void Init_Peripherals(void);
    void makeQuerry( char * DI,  char *RI, char *cntStr);
    char* setServiceList(unsigned int num,  char* serviceList);
    //Global Variables


#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_HEADER_H */

