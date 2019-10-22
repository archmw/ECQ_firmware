/* 
 * File:   num2Ascii_header.h
 * Author: home
 *
 * Created on July 18, 2016, 7:34 PM
 */

#ifndef NUM2ASCII_HEADER_H
#define	NUM2ASCII_HEADER_H

#ifdef	__cplusplus
extern "C" {
#endif
    void uiToStr( char *s, unsigned int bin,  char n);
    unsigned int charToInteger(  char a);
    unsigned int strToUi( char *cnt);
    unsigned int strToUiPartial( char *cnt, unsigned int len);
    unsigned int strCountUiCount( char *cnt, unsigned int len);

#ifdef	__cplusplus
}
#endif

#endif	/* NUM2ASCII_HEADER_H */

