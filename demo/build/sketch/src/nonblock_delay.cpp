#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/src/nonblock_delay.cpp"
#include "../include/nonblock_delay.h"

void NonBlockDelay::Delay(unsigned long t)
{
    this->iTimeout = millis() + t;
    return;
};

bool NonBlockDelay::Timeout(void)
{
    return (this->iTimeout < millis());
}

unsigned long NonBlockDelay::Time(void)
{
    return this->iTimeout;
}