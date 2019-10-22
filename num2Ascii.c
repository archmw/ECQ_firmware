#include "num2Ascii_header.h"
#include <string.h>
/* This function converts unsigned int to string 
 * @params: array of char, int value i.e to be converted no of bits
 */
void uiToStr( char *s, unsigned int bin,  char n) {
    s += n;
    *s = '\0';
    while (n--) {
        *--s = bin % 10 + '0';
        bin /= 10;
    }
}
unsigned int charToInteger( char a){
    unsigned int num;
    num = a - '0';
    return num;

}
unsigned int strToUi(char *cnt){
    unsigned int tempNum = 0, i;
    for(i = 0; i<strlen(cnt);i++){
        if(cnt[i]< 0x3A && cnt[i]>0x2F)
            tempNum = tempNum * 10 +(cnt[i] - '0');
    }
    return tempNum;
    

}

unsigned int strToUiPartial( char *cnt, unsigned int len){
    unsigned int tempNum = 0, i;
    for(i = 0; i<len;i++){
        if(cnt[i]< 0x3A && cnt[i]>0x2F)
            tempNum = tempNum * 10 +(cnt[i] - '0');
    }
    return tempNum;
    

}
/* Convert string Token number to int count value, 
 * removing Service ID from token number
 * @param array of token number
 * @param length of array
 * @return int value of token
 */
unsigned int strCountUiCount( char *cnt, unsigned int len){
    unsigned int tempNum = 0, i;
    for(i = 1; i<len;i++){
        tempNum = tempNum * 10 +(cnt[i] - '0');
    }
    return tempNum;
    

}