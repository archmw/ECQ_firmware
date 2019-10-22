/*
 * 25LC640A 64KB 8 bit 10MHz max SPI communication EEPROM is used to
 * store font and last status
 * HOLD is always pulled high from hardware
 * // sample code with similar read and write protocols for EEPROM
 * http://www.engscope.com/pic24-tutorial/12-2-spi-master-usage/ 
 */

/*
 * 4-wire SPI is used to interface EEPROM
 * standard mode with 4MHZ clock  SPI
 * SDI,SDO, CS, SCK pins are used
 * HOLD is held high externally,
 * 
 */
#include "include_header.h"
#include "EEPROM_h.h"
#include "spi.h"
#include "display_dec.h"

// Initialize EEPROM
void Init_EEPROM(){
    
    CS_Tris = 0; // MCU is master to EEPROM
    WP_Tris = 0; // write protect

    spiEeSsHigh();
    EE_WP = 1; // pulled high as mentioned in datasheet



}

//writes to the eeprom device
void spiEeByteWrite(unsigned long Address, unsigned char Data)
{
    unsigned char var;
    spiEeWriteEnable();
    __delay_us(1);
//    __delay_ms(1);
    spiEeSsLow();

    var = spi1Write(EEPROM_CMD_WRITE);

//    var = spi1Write(Address);
//    var = spi1Write(Address);
    var = spi1Write((unsigned char)(Address>>8)&0x00FF);
    var = spi1Write((unsigned char)(Address & 0x00FF));

    var = spi1Write(Data);

    spiEeSsHigh();

    // wait for completion of previous write operation
    while(EEPROMReadStatus().Bits.WIP);
    
    spiEeWriteDisable();
}

//reads from the eeprom device
unsigned char spiEeByteRead(unsigned long Address)
{
    unsigned char var;

    spiEeSsLow();
//    __delay_ms(1);
   __delay_us(1);
    var = spi1Write(EEPROM_CMD_READ);

//    var = spi1Write(Address);
//    var = spi1Write(Address);
    var = spi1Write((unsigned char)(Address>>8)&0x00FF);
    var = spi1Write((unsigned char)(Address & 0x00FF));
    var = spi1Write(0xFF);

    spiEeSsHigh();
    return var;
}

//enable write by changing status register
void spiEeWriteEnable()
{
    unsigned char var;
    spiEeSsLow();
    var = spi1Write( EEPROM_CMD_WREN);
    spiEeSsHigh();
}

//disable write by changing status register
void spiEeWriteDisable()
{
    unsigned char var;
    spiEeSsLow();
    var = spi1Write(EEPROM_CMD_WRDI);
    spiEeSsHigh();
}

//read the status regsiter
union _EEPROMStatus_ EEPROMReadStatus()
{
    unsigned char var;

    spiEeSsLow();
    var = spi1Write(EEPROM_CMD_RDSR);
    var = spi1Write(0);
    spiEeSsHigh();

    return (union _EEPROMStatus_)var;
}

//polls to see if the SPI EEPROM is present
unsigned char spiEePoll()
{
    unsigned char temp = 0;
    spiEeWriteEnable();
    temp = EEPROMReadStatus().Bits.WEL;
    spiEeWriteDisable();
    return temp;
}
/*====================================================*/
/*========StoreData function==========================*/
/*prams: memory location, single byte of integer======*/
/*This functions writes single byte of integer by converting integer to char value*/
/*====================================================*/
void storeData(unsigned long addr, unsigned int dat){

    unsigned char charData;
    charData = dat%10 + '0';
//    itoa(dat,charData,10);
    //printDspl(charData);
    spiEeByteWrite(addr,charData);
 
}
/* 
 * eeWriteArr: This function writes array of specified length
 * 
 */
void eeWriteArr(unsigned long startAddr,  char *arr, unsigned short lengthArr){
    unsigned short i;
    for(i =0; i<lengthArr;i++){
        spiEeByteWrite(startAddr++,arr[i]);
    }
}

/* 
 * eeReadArr: This function reads array of specified length
 * 
 */
void eeReadArr(unsigned long startAddr,  char *arr, unsigned short lengthArr){
    unsigned short i;
    for(i =0; i<lengthArr;i++){
        arr[i] = spiEeByteRead(startAddr++);
    }
}