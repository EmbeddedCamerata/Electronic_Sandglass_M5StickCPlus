#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/include/sandglass.h"
#ifndef _SANDGLASS2_H_
#define _SANDGLASS2_H_

#include "../include/ledmatrix.h"
#include "../include/matrixsand.h"

#define LEDMATRIX_START_LAYER   10  // More than 8

typedef struct {
    int mins;
    int secs;
} Countdown_TypeDef;

class Sandglass2 {
    public:
    Sandglass2();

    matrix_sand::MatrxiSand sand1;
    matrix_sand::MatrxiSand sand2;

    LedMatrix m1;
    LedMatrix m2;
    
    void init(void);
    void start(Countdown_TypeDef* CountdownStruct);
    void IRAM_ATTR clock_update(void);
    void tick(void);
    void stop(void);
    void show_settime(Countdown_TypeDef* CountdownStruct);
    bool is_activated(void);
    bool isTick;                    // Update led matrices when in the second interval and after

    // Led matrices related
    void IRAM_ATTR ledmatrix_update(void);
    void IRAM_ATTR frame_refresh(void);
    int frame_refresh_interval;     // times per ms
    bool need_lm_refresh;           // Refresh led matrices data
    
    private:
    void update_matrix(LedMatrix *m, matrix_sand::MatrxiSand *s);

    // Led matrices related
    float accX, accY, accZ, xx, yy, zz;
    bool updated1, updated2;

    // Clock related
    int counter;                    // second counter
    bool isActivated;
    Countdown_TypeDef rest_time;    // Rest time of countdown: mins and secs

    bool reset_settime_when_stop;   // config: reset the last countdown when stop
};

void common_test(void);
void test(void);

#endif