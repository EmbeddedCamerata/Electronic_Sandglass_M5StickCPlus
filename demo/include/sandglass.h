#ifndef _SANDGLASS2_H_
#define _SANDGLASS2_H_

#include <M5StickCPlus.h>

typedef struct {
    int mins;
    int secs;
} Countdown_TypeDef;

class Sandglass2 {
    public:
    Sandglass2();
    
    void init(void);
    void start(Countdown_TypeDef* CountdownStruct);
    // void pause(void);
    // void resume(void);
    void update(void);
    void tick(void);
    void stop(void);
    void Show_Countdown(void);
    void show_settime(Countdown_TypeDef* CountdownStruct);
    bool is_activated(void);
    bool is_tick(void);
    bool need_refresh(void);
    
    private:
    RTC_TimeTypeDef TimeStruct;

    bool isActivated;
    bool isTick; // Update led matrices when in the second interval and after
    unsigned long _start_tick;
    Countdown_TypeDef rest_time;
    Countdown_TypeDef set_time;
};

void common_test(void);
void test(void);

#endif