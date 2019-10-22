#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include_header.h"
#include "num2Ascii_header.h"
#include "uart_1.h"
#include "display_dec.h"
//void makeQuerry( char * DI,  char *RI, char *cntStr);

extern unsigned char sendQuery[15];

//void makeQuerry(unsigned int DI, unsigned int RI, unsigned int countVal,unsigned char suffix) {
void makeQuerry( char *DI,  char *RI, char *cntStr) {
    unsigned int j;
    //0  1    23 4     56 7     89AB       C     D
   // *<space>DI<space>RI<space>countValue<space># - 14 bytes
    
//    uiToStr(sendQuery,RI,7);
//    uiToStr(sendQuery,DI,4);
    sendQuery[0] = 0x2A; // *
    sendQuery[1] = 0x20; // <Space>
//    UART1PutString(" DI = ");
//    UART1PutString(DI);
    for(j = 0; j<2; j++){
        sendQuery[j+2] = *DI++;
    }
    
    sendQuery[4] = 0x20;// <SPACE>
    
    for(j = 0; j<2; j++){
        sendQuery[j+5] = *RI++;
    }
    sendQuery[7] = 0x20;// <SPACE>
    
    for( j = 0; j<4;j++){
        sendQuery[j+8] = *cntStr++;
    }
//    uiToStr(sendQuery,countVal,12);
//    sendQuery[0] = 0x2A; // *
//    sendQuery[1] = 0x20; // <Space>
//    sendQuery[4] = 0x20;
//    sendQuery[7] = 0x20;
    sendQuery[12] = 0x20;
    sendQuery[13] = 0x23; // #
//    UART1PutString(sendQuery);
}


/* copy char by char from one destination array to source array
 * copy length of source arr - 1
 * @params destination arr, source arr
 */
void strCpyArr(char * dest, char * src){
    unsigned int i = 0;
    for(;i<strlen(src);i++){
        dest[i] = src[i];
    }

}
/*Fill unused places of serviceID array with 0x20(SPACE) and return complete array of 10 elements
 * @params num = number of services
 * @params service list array that carries list of service ID
 * @return array with 10 elements, where empty elements are replaced with 0xFF
 */
char* setServiceList(unsigned int num, char* serviceList){
    char serviceID[10];
    unsigned int i = 0;
    strncpy(serviceID,serviceList,num);
    if(num<10){
        for(i=num;i<10;i++) // write 0xFF at empty places
            serviceID[i] = 0x20;
    }
    return *serviceID;
}