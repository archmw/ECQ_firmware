/* 
 * File:   spi.h
 * Author: LHCD
 *
 * Created on 31 May, 2016, 9:38 AM
 */

#ifndef SPI_H
#define	SPI_H

#ifdef	__cplusplus
extern "C" {
#endif
#define PIC_SDO LATBbits.LATB7 // EE_SI_PIC_SDO
#define PIC_SDO_Tris TRISBbits.TRISB7

#define PIC_SCK LATBbits.LATB8 // SCK
#define PIC_SCK_Tris TRISBbits.TRISB8
    
#define PIC_SDI PORTBbits.RB9 // EE_SO_PIC_SDI
#define PIC_SDI_Tris TRISBbits.TRISB9
void spi1Init(void);
void spi2Init(void);
unsigned char spi2Write(unsigned int);
unsigned char spi1Write( unsigned char i );

#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */

