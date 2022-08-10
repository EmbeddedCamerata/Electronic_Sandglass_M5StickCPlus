#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/include/nonblock_delay.h"
#ifndef _NONBLOCK_DELAY_H_
#define _NONBLOCK_DELAY_H_

#include <Arduino.h>

class NonBlockDelay {
    public:
    void Delay(unsigned long);
    bool Timeout(void);
    unsigned long Time(void);

    private:
    unsigned long iTimeout;
};

#endif