/* 
 * File:   EEPROM_h.h
 * Author: ABC
 *
 * 
 */

#ifndef EEPROM_H_H
#define	EEPROM_H_H

#ifdef	__cplusplus
extern "C" {
#endif
#define EE_CS LATBbits.LATB5 // EEPROM chip select
#define CS_Tris TRISBbits.TRISB5
    
#define EE_WP LATBbits.LATB6 // EEPROM Write protect
#define WP_Tris TRISBbits.TRISB6

//#define PIC_SDO LATBbits.LATB7 // EE_SI_PIC_SDO
//#define PIC_SDO_Tris TRISBbits.TRISB7
//
//#define PIC_SCK LATBbits.LATB8 // SCK
//#define PIC_SCK_Tris TRISBbits.TRISB8
//    
//#define PIC_SDI PORTBbits.RB9 // EE_SO_PIC_SDI
//#define PIC_SDI_Tris TRISBbits.TRISB9

    //instruction set
#define EEPROM_CMD_READ     (unsigned)0b00000011
#define EEPROM_CMD_WRITE    (unsigned)0b00000010
#define EEPROM_CMD_WRDI     (unsigned)0b00000100
#define EEPROM_CMD_WREN     (unsigned)0b00000110
#define EEPROM_CMD_RDSR     (unsigned)0b00000101
#define EEPROM_CMD_WRSR     (unsigned)0b00000001

//struct for the status register
struct  STATREG{
	unsigned    WIP:1;
	unsigned    WEL:1;
	unsigned    BP0:1;
	unsigned    BP1:1;
	unsigned    RESERVED:3;
	unsigned    WPEN:1;
};

union _EEPROMStatus_{
	struct  STATREG Bits;
	unsigned char	Char;
};


void Init_EEPROM(void);





//read the status regsiter
extern union _EEPROMStatus_ EEPROMReadStatus(void);

//set the macro for active SPI
#define spiEeSsLow()      EE_CS = 0;

//set the macro for inactive SPI
#define spiEeSsHigh()     EE_CS = 1;
#define Hi(X)   (unsigned char)((X>>8)&0x00ff)
#define Lo(X)   (unsigned char)(X&0x00ff)
//writes to the eeprom device
extern void spiEeByteWrite(unsigned long Address, unsigned char Data);

//reads from the eeprom device
extern unsigned char spiEeByteRead(unsigned long Address);

//enable write by changing status register
extern void spiEeWriteEnable(void);

//disable write by changing status register
extern void spiEeWriteDisable(void);

//polls to see if the SPI EEPROM is present
unsigned char spiEePoll();
void storeData(unsigned long addr, unsigned int dat);
void eeWriteArr(unsigned long startAddr,  char *arr, unsigned short lengthArr);
void eeReadArr(unsigned long startAddr,  char *arr, unsigned short lengthArr);
// Write font table in EEPROM location 0 to 600
//void Font_Write();
#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H_H */

