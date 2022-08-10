#include <M5StickCPlus.h>
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

hw_timer_t* timer10s(uint8_t num, void (*fn)(void), bool autoreload) {
    hw_timer_t* timer = timerBegin(num, (TIMER_BASE_CLK / 1000000), true);
    timerStop(timer);
    timerAttachInterrupt(timer, fn, false);
    timerAlarmWrite(timer, 10000000, autoreload);
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

void timer_callback(void) {
    digitalWrite(M5_LED, 1 - digitalRead(M5_LED));
}

void idle_goto_sleep(void) {
    M5.Lcd.setCursor(30, 70);
    M5.Lcd.printf("Go sleep");
}

