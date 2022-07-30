#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/include/ledarray.h"
#ifndef _LEDARRAY_H_
#define _LEDARRAY_H_

#include <Arduino.h>

#define LEDARRAY_D      26 
#define LEDARRAY_SRCLK  25
#define LEDARRAY_RCLK    0
#define MUTEX_PIN       36  // Remember float input

void ledarray_init(void);

void scan_row(uint8_t _row, uint8_t _col);

#endif