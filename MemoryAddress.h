/* 
 * File:   MemoryAddress.h
 * Author: ABC
 *
 * Created on January 25, 2017, 5:20 PM
 */

#ifndef MEMORYADDRESS_H
#define	MEMORYADDRESS_H

#ifdef	__cplusplus
extern "C" {
#endif
    //================= Parameters address =====================================================================
#define adrNetMode          0x0000
#define adrMasterMode       0x0001
#define adrMultiService     0x0002
#define adrIDHi             0x0003
#define adrIDLo             0x0004
#define adrThreeDgt         0x0005
#define adrSufChar          0x0006
#define adrBellStatus       0x0007
#define adrMstrSecond       0x00C8 // secondary master mode
    //===============================================================================================================
    //=============== Last number for both Master and slave displays ================================================    
    //=============== Master Last Number needs 7 bytes and slave needs 4 bytes========================================
#define adrLastNumSuf       0x0008
#define adrLastNumC1        0x0009
#define adrLastNumC2        0x000A
#define adrLastNumC3        0x000B
#define adrLastNumArrow     0x000C // at memory location it will store dot Symbol '.' insteed of arrow but at display it will show arrow
#define adrLastNumIDHi      0x000D
#define adrLastNumIDLo      0x000F
    //==========================================================================================================
    //========== Service start and Service end values ==========================================================
  
#define adrNumOfServiceLo        0x0010
#define adrNumOfServiceHi        0x0011
    // Page 1
#define adrSer1StartSuf          0x0020
#define adrSer1StartC1           0x0021
#define adrSer1StartC2           0x0022
#define adrSer1StartC3           0x0023
#define adrSer1EndSuf            0x0024
#define adrSer1EndC1             0x0025
#define adrSer1EndC2             0x0026
#define adrSer1EndC3             0x0027

#define adrSer2StartSuf          0x0028
#define adrSer2StartC1           0x0029
#define adrSer2StartC2           0x002A
#define adrSer2StartC3           0x002B
#define adrSer2EndSuf            0x002C
#define adrSer2EndC1             0x002D
#define adrSer2EndC2             0x002E
#define adrSer2EndC3             0x002F

#define adrSer3StartSuf          0x0030
#define adrSer3StartC1           0x0031
#define adrSer3StartC2           0x0032
#define adrSer3StartC3           0x0033
#define adrSer3EndSuf            0x0034
#define adrSer3EndC1             0x0035
#define adrSer3EndC2             0x0036
#define adrSer3EndC3             0x0037

#define adrSer4StartSuf          0x0038
#define adrSer4StartC1           0x0039
#define adrSer4StartC2           0x003A
#define adrSer4StartC3           0x003B
#define adrSer4EndSuf            0x003C
#define adrSer4EndC1             0x003D
#define adrSer4EndC2             0x003E
#define adrSer4EndC3             0x003F
    // Page 2
#define adrSer5StartSuf          0x0040
#define adrSer5StartC1           0x0041
#define adrSer5StartC2           0x0042
#define adrSer5StartC3           0x0043
#define adrSer5EndSuf            0x0044
#define adrSer5EndC1             0x0045
#define adrSer5EndC2             0x0046
#define adrSer5EndC3             0x0047

#define adrSer6StartSuf          0x0048
#define adrSer6StartC1           0x0049
#define adrSer6StartC2           0x004A
#define adrSer6StartC3           0x004B
#define adrSer6EndSuf            0x004C
#define adrSer6EndC1             0x004D
#define adrSer6EndC2             0x004E
#define adrSer6EndC3             0x004F

#define adrSer7StartSuf          0x0050
#define adrSer7StartC1           0x0051
#define adrSer7StartC2           0x0052
#define adrSer7StartC3           0x0053
#define adrSer7EndSuf            0x0054
#define adrSer7EndC1             0x0055
#define adrSer7EndC2             0x0056
#define adrSer7EndC3             0x0057

#define adrSer8StartSuf          0x0058
#define adrSer8StartC1           0x0059
#define adrSer8StartC2           0x005A
#define adrSer8StartC3           0x005B
#define adrSer8EndSuf            0x005C
#define adrSer8EndC1             0x005D
#define adrSer8EndC2             0x005E
#define adrSer8EndC3             0x005F
    // Page 3
#define adrSer9StartSuf          0x0060
#define adrSer9StartC1           0x0061
#define adrSer9StartC2           0x0062
#define adrSer9StartC3           0x0063
#define adrSer9EndSuf            0x0064
#define adrSer9EndC1             0x0065
#define adrSer9EndC2             0x0066
#define adrSer9EndC3             0x0067

#define adrSer10StartSuf         0x0068
#define adrSer10StartC1          0x0069
#define adrSer10StartC2          0x006A
#define adrSer10StartC3          0x006B
#define adrSer10EndSuf           0x006C
#define adrSer10EndC1            0x006D
#define adrSer10EndC2            0x006E
#define adrSer10EndC3            0x006F    

#define adrSufSer1               0x0070
#define adrSufSer2               0x0078
    
#define adrSufSer3               0x0080
#define adrSufSer4               0x0088
    
#define adrSufSer5               0x0090
#define adrSufSer6               0x0098
    
#define adrSufSer7               0x00A0
#define adrSufSer8               0x00A8
    //Page 4
#define adrSufSer9               0x00B0
#define adrSufSer10              0x00B8

#define newSettingFlag           0x00C0
#define lastServiceLoad          0x00D0    
//unsigned char EEPROM_LOCATION_EMPTY = 0xFF;
/* 22-10-2019 as discussed with Senthil onver telephonic conversion,            */
/* insteaf of writing 0xFF, write '0' (zero in string format)                   */
/* because it might be possible that oxFF is not getting converted into         */
/* string format showing up garbage value when service remains unused.          */
/* Instead of writing '0' if preferd '#' as this symbol is used nowhere*/
    unsigned char EEPROM_LOCATION_EMPTY = 0x23; // ascii value of # --> 0x23    






#ifdef	__cplusplus
}
#endif

#endif	/* MEMORYADDRESS_H */

