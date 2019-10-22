/* 
 * File:   prog_mode_setting.h
 * Author: home
 *
 * Created on July 20, 2016, 6:40 PM
 */

#ifndef PROG_MODE_SETTING_H
#define	PROG_MODE_SETTING_H

#ifdef	__cplusplus
extern "C" {
#endif
#define NW_MODE     0x01    //for network mode and stand alone mode
                            //1-> Network Mode
                            //0-> Stand-alone mode
#define MS_MODE     0x02    //for Master/slave mode selection
                            //1-> Master Mode
                            //0-> Slave Mode
#define SERV        0x04    //IN network mode SELECT SERVICE TYPE SINGLE/MULTIPLE
                            //1-> MULTIPLE SERVICES 
                            //0-> SINGLE SERVICES
#define SELECT_ID   0X08    // SELECT DEVICE ID 
                            // 00 -> default for master mode
                            // 01 to 32 for slave/standalone mode
#define DGT_SEL     0x10    //for counter number of dgt selection valid for slave and standalone mode only
                            //1-> 2 digit
                            //0-> 3 digit
#define PREFIX      0x20    // prefix selection for slave and standalone mode
                            // A to Z: user can select any prefix, this is service ID of device
    
#define BEL_STS     0x40    //for network mode and stand alone mode
                            //1-> BELL ON 
                            //0-> BELL OFF
#define WRITE_EEPROM 0X80   // save all parameters to EEPROM
                            // 1-> SAVE TO EEPROM
                            // 0-> DISCARD CHANGES
    



#ifdef	__cplusplus
}
#endif

#endif	/* PROG_MODE_SETTING_H */

