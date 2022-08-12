#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

hw_timer_t* timer1s(uint8_t num, void (*fn)(void), bool autoreload=false);
hw_timer_t* milli_timer(int milli, uint8_t num, void (*fn)(void), bool autoreload=false);

#endif