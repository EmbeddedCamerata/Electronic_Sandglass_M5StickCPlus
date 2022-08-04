#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/include/sandglass.h"
#ifndef _SANDGLASS_H_
#define _SANDGLASS_H_

#include <M5StickCPlus.h>
#include "ledmatrix.h"

typedef struct {
    int8_t mins;
    int8_t secs;
} Countdown_TypeDef;

class Sandglass: public LedMatrix {
    public:
    Sandglass(): LedMatrix(){};
    void init(void);
    void start(Countdown_TypeDef* CountdownStruct);
    // void pause(void);
    // void resume(void);
    void update(void);
    void stop(void);
    void Show_Countdown(void);
    void show_settime(Countdown_TypeDef* CountdownStruct);
    void Show_RestTime(void);
    bool is_activated(void);
    bool need_refresh(void);

    void test(void) {
        LedMatrix::test();
    };

    float interval;

    private:
    void Set_Countdown(Countdown_TypeDef* CountdownStruct);

    bool isActivated;
    bool first_interval; // Update led matrices when in the second interval and after
    unsigned long _start_tick;
    Countdown_TypeDef rest_time;
    Countdown_TypeDef set_time;
};

#endif