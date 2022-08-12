#include "esp32-hal-timer.h"
#include "driver/timer.h"

hw_timer_t* timer1s(uint8_t num, void (*fn)(void), bool autoreload) {
    hw_timer_t* timer = timerBegin(num, (TIMER_BASE_CLK / 1000000), true);
    timerStop(timer);
    timerAttachInterrupt(timer, fn, false);
    timerAlarmWrite(timer, 1000000, autoreload);
    timerAlarmEnable(timer); 
    timerRestart(timer);
    timerStart(timer);

    return timer;
}

hw_timer_t* milli_timer(int milli, uint8_t num, void (*fn)(void), bool autoreload) {
    hw_timer_t* timer = timerBegin(num, (TIMER_BASE_CLK / 1000000), true);
    timerStop(timer);
    timerAttachInterrupt(timer, fn, false);
    timerAlarmWrite(timer, 1000 * milli, autoreload);
    timerAlarmEnable(timer); 
    timerRestart(timer);
    timerStart(timer);

    return timer;
}