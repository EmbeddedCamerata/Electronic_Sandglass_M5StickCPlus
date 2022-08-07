#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

hw_timer_t* timer1s(uint8_t num, void (*fn)(void), bool autoreload=false);
hw_timer_t* timer10s(uint8_t num, void (*fn)(void), bool autoreload=false);
void timer_callback(void);
void idle_goto_sleep(void);

#endif