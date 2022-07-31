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
    void Show_Countdown();
    void show_settime(Countdown_TypeDef* CountdownStruct);
    void Show_RestTime(void);
    uint16_t Get_TotalTime(void);
    bool is_Activated(void);

    private:
    void init(void);
    void Set_Countdown(Countdown_TypeDef* CountdownStruct);

    bool isActivated;
    Countdown_TypeDef rest_time;
    Countdown_TypeDef set_time;
};

#endif