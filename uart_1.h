/* 
 * File:   uart_1.h
 * Author: home
 *
 * Created on July 2, 2016, 6:42 PM
 */

#ifndef UART_1_H
#define	UART_1_H

#ifdef	__cplusplus
extern "C" {
#endif
    void init_uart(void);
    void UART1PutChar(char Ch);
    void UART1PutString(char *st);
    void UARTEnable(void);
    void UARTDisable(void);


#ifdef	__cplusplus
}
#endif

#endif	/* UART_1_H */

