#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/include/sandglass.h"
#ifndef _SANDGLASS_H_
#define _SANDGLASS_H_

#include <M5StickCPlus.h>
#include "ledarray.h"

typedef struct {
    uint8_t mins;
    uint8_t secs;
} Countdown_TypeDef;

class Sandglass {
    public:
    Sandglass();
    void start(Countdown_TypeDef* CountdownStruct);
    // void pause(void);
    // void resume(void);
    void update(void);
    void stop(void);
    void Set_Countdown(Countdown_TypeDef* CountdownStruct);
    void Get_Countdown(Countdown_TypeDef* CountdownStruct);
    void Show_Countdown(Countdown_TypeDef* CountdownStruct);
    uint16_t Get_TotalTime(void);
    bool is_Activated(void);

    private:
    bool isActivated;
    Countdown_TypeDef now_time;
    Countdown_TypeDef set_time;
};

#endif