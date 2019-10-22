
#include "spi.h"
#include "include_header.h"

 //SPI1 is used with EEPROM
void spi1Init(void){
//    SPI Mode CKP CKE
//     0,   0   0    1
//     0,   1   0    0
//     1,   0   1    1
//     1,   1   1    0
//Here we have used mode 0,0
    //Peripheral PIN Select(PPS) for SPI1
    RPOR3bits.RP7R=7; // SPI DATA (SDO)output on RP7 (pin43)
    RPOR4bits.RP8R=8; // Clock output on RP8(pin44)
    RPINR20bits.SDI1R=9; // SPI Serial Data INput(SI) on RP9 (pin1)
    // SPI SFR configuration
    PIC_SCK_Tris = 0; // SPI clock
    PIC_SDI_Tris = 1; // EE_SO_PIC_SDI
    PIC_SDO_Tris = 0; // EE_SI_PIC_SDO
//    
    SPI1CON1bits.PPRE = 0x02;     // 4:1 prescaler; 40/4 = 10 MHz // test 1,2,3
//    SPI1CON1bits.PPRE = 0x00;     // 64:1 prescaler; 40/64 = // test4
////    SPI1CON1bits.SPRE = 0x07; // 1:1 prescaler
//    SPI1CON1bits.SPRE = 0x06;     // 2:1 prescaler 10/2 = 5MHz as we are using VCC of 3.3V
//    SPI1CON1bits.SPRE = 0x05; // 3:1 sec prescaler 10/3 = 3.33MHz // test 2
    SPI1CON1bits.SPRE = 0x03; // 5:1 sec prescaler 10/5 = 2MHz // test 3
//    SPI1CON1bits.SPRE = 0x00; // 8:1 sec prescaler  78125Hz // test 4
//    SPI1CON1bits.MSTEN = 1;   // Master mode
//    SPI1CON1bits.CKP = 0;     // idle clock state is low
//    SPI1CON1bits.CKE = 0;     // Serial output data changes on transition from Idle clock state to active clock state
    SPI1CON1bits.SSEN = 0;      // no hardware SS is used, rather it is controlled manually
                                //SSEN: Slave Select Enable bit (Slave mode)(3)
                                //1 = SSx pin used for Slave mode
                                //0 = SSx pin not used by module. Pin controlled by port function
//    SPI1CON1bits.SMP = 0; // Input data is sampled at middle of data output time
//    SPI1CON1bits.MODE16 = 0; // 8-bit communication
//    SPI1CON1bits.DISSDO = 0; // SDOx pin is controlled by the module
//    SPI1CON1bits.DISSCK = 0;// SCK pin is controlled by the module
//    SPI1CON2 = 0x00;
//    SPI1STATbits.SPIEN = 1; //enable spi
    /* The following code sequence shows SPI register configuration for Master mode */
    IFS0bits.SPI1IF = 0; // Clear the Interrupt flag
    IEC0bits.SPI1IE = 0; // Disable the interrupt
    // SPI1CON1 Register Settings
    SPI1CON1bits.DISSCK = 0;  // Internal serial clock is enabled
    SPI1CON1bits.DISSDO = 0;  // SDOx pin is controlled by the module
    SPI1CON1bits.MODE16 = 0;  // Communication is byte-wide (8 bits)
    SPI1CON1bits.MSTEN = 1;   // Master mode enabled
//    SPI1CON1bits.SMP = 0;     // Input data is sampled at the middle of data output time
     SPI1CON1bits.SMP = 1;     // Input data is sampled at the middle of data output time
//    SPI1CON1bits.CKE = 0;     // Serial output data changes on transition from
//                          // Idle clock state to active clock state
    // Changes as mentioned in SPIEERPOM sample code from microchip
    SPI1CON1bits.CKE = 1;     // Serial output data changes on transition from
                          // active clock state to Idle clock state
    SPI1CON1bits.CKP = 0;     // Idle state for clock is a low level;
                          // active state is a high level
    SPI1STATbits.SPIEN = 1;   // Enable SPI module
                          // Interrupt Controller Settings
    IFS0bits.SPI1IF = 0;      // Clear the Interrupt flag
    //IEC0bits.SPI1IE = 1;    // Enable the interrupt
    

}
unsigned char spi1Write( unsigned char i ){
    SPI1STATbits.SPIROV=0;   //Clear SPIx Receive Overflow Flag bit
    // write to buffer for TX, wait for transfer, read
    SPI1BUF = i;
    while(!SPI1STATbits.SPIRBF);
    return SPI1BUF;
}

void spi2Init(void){
    //Peripheral PIN Select(PPS) for SPI2
    RPOR5bits.RP10R=10;  //SPI DATA (SDO)output on RP10 (pin8) 
    RPOR5bits.RP11R=11;  //SPI Clock output on RP11(pin9)
    //SPI SFR Configuration
    
    SPI2CON1bits.DISSCK=0;  //Internal SPI clock is enabled
    SPI2CON1bits.DISSDO=0;  //SDO2 pin is controlled by the module
    SPI2CON1bits.MODE16=1;  //Communication is word-wide (16 bits)
    SPI2CON1bits.SMP=0;     //0 = Input data is sampled at middle of data output time
    SPI2CON1bits.CKE=0;     //0 = Serial output data changes on transition from Idle clock state to active clock state
    SPI2CON1bits.SSEN=0;    // (Slave Select pin disabled)
    
    //Clock Polarity Select bit
    SPI2CON1bits.CKP=0;    //0 = Idle state for the clock is a low level; active state is a high level
   
    //Master Mode Enable bit
    SPI2CON1bits.MSTEN=1;  //1 = Master mode
    
    //Secondary Prescale bits (Master mode)
    SPI2CON1bits.SPRE=7;  //Secondary prescale 1:1
    
    //Primary Prescale bits (Master mode)
    SPI2CON1bits.PPRE=2;  //Primary prescale 4:1 (Fcy 40MHz/4=10MHz)
    SPI2STATbits.SPIEN=1;
}
unsigned char spi2Write(unsigned int data){
    SPI2STATbits.SPIROV=0;   //Clear SPIx Receive Overflow Flag bit
    SPI2BUF=data;
    while(!SPI2STATbits.SPIRBF); //SPIx Receive Buffer Full Status bit
                                 //0 = Receive is not complete, SPIxRXB is empty
    return (SPI2BUF);
}