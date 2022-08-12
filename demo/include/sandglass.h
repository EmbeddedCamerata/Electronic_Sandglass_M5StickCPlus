#ifndef _SANDGLASS_H_
#define _SANDGLASS_H_

#include "../include/ledmatrix.h"
#include "../include/matrixsand.h"

#define LEDMATRIX_START_LAYER   11  // More than 8 is well

typedef struct {
    int mins;
    int secs;
} Countdown_TypeDef;

class Sandglass {
    public:
    Sandglass();

    matrix_sand::MatrxiSand sand1;
    matrix_sand::MatrxiSand sand2;

    LedMatrix m1;
    LedMatrix m2;
    
    void init(void);
    void start(Countdown_TypeDef* CountdownStruct);
    void pause(void);
    void resume(void);
    void restart(Countdown_TypeDef* CountdownStruct);
    void IRAM_ATTR clock_update(void);
    void tick(void);
    void stop(bool is_shutdown=false);
    void shutdown(void);
    void show_countdown(Countdown_TypeDef* CountdownStruct);
    bool is_activated(void);
    bool is_working(void);
    bool isTick;                        // Update led matrices when in the second interval and after

    // Led matrices related
    void random_idle(void);             // When in idle, two led matrices blink randomly
    void IRAM_ATTR ledmatrix_update(void);
    void IRAM_ATTR frame_refresh(void); // Frame refreshment
    int frame_refresh_interval;         // times per ms
    bool need_lm_refresh;               // Refresh led matrices data
    
    private:
    void update_matrix(LedMatrix *m, matrix_sand::MatrxiSand *s);

    // Led matrices related
    float accX, accY, accZ, xx, yy, zz;
    bool updated1, updated2;

    // Clock related
    bool isActivated;
    bool isWorking;
    Countdown_TypeDef rest_time;        // Rest time of countdown: mins and secs
    Countdown_TypeDef last_set_time;    // Record the last time of countdown/settime
};

#endif