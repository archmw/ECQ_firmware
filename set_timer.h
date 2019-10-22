/* 
 * File:   set_timer.h
 * Author: ABC
 *
 * 
 */

#ifndef SET_TIMER_H
#define	SET_TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

    void InitializeTimer();
    void __attribute__((__interrupt__)) _T1Interrupt(void);



#ifdef	__cplusplus
}
#endif

#endif	/* SET_TIMER_H */

