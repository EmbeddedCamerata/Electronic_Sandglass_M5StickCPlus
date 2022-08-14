#include <M5StickCPlus.h>
#include "../include/user.h"
#include "../include/matrixsand.h"
#include "../include/ledmatrix.h"
#include "../include/sandglass.h"
#include "../include/bsp_timer.h"
#include "../include/nonblock_delay.h"

#define MUTEX_PIN       GPIO_NUM_36           // Need set to mode float input
#define DEBUG_MODE

TFT_eSprite Disbuff = TFT_eSprite(&M5.Lcd);
Countdown_TypeDef CountdownStruct = {.mins = 0, .secs = 0};
Sandglass sandglass;
hw_timer_t* clock_timer = NULL;
hw_timer_t* update_timer = NULL;
NonBlockDelay d1, d2;
bool _is_break = false;

static void Lcd_Setup(void);
static void Key_Handle_inIdle(void);
static void Key_Handle_inWorking(void);
static void clock_update(void);
static void ledmatrix_refresh(void);
static void led_heartbeat(void);
static void start_timers(void);

void User_Setup(void) {
    int ret;
    pinMode(M5_LED, OUTPUT);
    gpio_pulldown_dis(MUTEX_PIN);
    gpio_pullup_dis(MUTEX_PIN);

    sandglass.init();
    Lcd_Setup();
    ret = M5.IMU.Init();

    if (ret != 0) {
        Serial.println("Init IMU error!");
    }
}

void User_Loop(void) {

    if (not sandglass.is_activated()) {
        Key_Handle_inIdle();
    }

    while (1) {
        if (sandglass.isTick) {
            sandglass.clock_update();
        }

        if (sandglass.need_lm_refresh) {
            sandglass.ledmatrix_update();  // data update
        }
        
        if (not sandglass.is_activated()) {
            timerStop(clock_timer);
            timerStop(update_timer);
            break;
        }

        // Frame refresh as frequently as you can when not completed or tick.
        sandglass.frame_refresh();
        Key_Handle_inWorking();

        if (_is_break) {
            _is_break = false;
            break;
        }

        if (d2.Timeout()) {
            M5.Lcd.fillRect(10, 10, M5.Lcd.height(), M5.Lcd.fontHeight(), TFT_BLACK);
        }

#ifdef DEBUG_MODE
        M5.Lcd.setCursor(10, 100);
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.setTextSize(1);
        M5.Lcd.printf("%d ms", sandglass.frame_refresh_interval);
#endif
        Disbuff.pushSprite(0, 0);
    }
    delay(1000);
}

static void Key_Handle_inIdle(void) {

    M5.Lcd.fillRect(10, 10, M5.Lcd.height(), M5.Lcd.fontHeight(), TFT_BLACK);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Set");
    Disbuff.pushSprite(0, 0);

    d1.Delay(500);

    while (1) {
        M5.update();
        if (M5.BtnA.wasReleasefor(800)) {
            sandglass.start(&CountdownStruct);
            start_timers();
            break;
        }
        else if (M5.BtnA.wasReleased()) {
            if (CountdownStruct.mins == 10) {
                CountdownStruct.mins = 0;
            }
            else {
                CountdownStruct.mins += 1;
            }
            
        }
        else if (M5.BtnB.wasReleased()) {
            if (CountdownStruct.secs == 59) {
                CountdownStruct.secs = 0;
            }
            else {
                CountdownStruct.secs += 1;
            }
        }
        else if (M5.BtnB.wasReleasefor(800)) {
            if (CountdownStruct.secs >= 50) {
                CountdownStruct.secs = 0;
            }
            else {
                CountdownStruct.secs += 10;
            }
        }

        if (d1.Timeout()) {
            sandglass.random_idle();
            d1.Delay(500);
        }
        
        sandglass.frame_refresh();
        sandglass.show_countdown(&CountdownStruct);
        Disbuff.pushSprite(0, 0);

        // TODO If in idle, go into low-cost mode
    }
}

/*
    Sacn buttons when sandglass is working.
    Button A for pause/resume and loog press for break and reset
    Button B for restart (from the initial countdown)
*/
static void Key_Handle_inWorking(void) {
    M5.update();
    if (M5.BtnA.wasReleased()) {
        if (sandglass.is_working()) {
            sandglass.pause();
            // Pause the timers
            timerStop(clock_timer);
            timerStop(update_timer);
            // Delay 1s for displaying the "Pause"
            d2.Delay(1000);
        }
        else {
            sandglass.resume();
            // Resume the timers
            timerStart(clock_timer);
            timerStart(update_timer);
            d2.Delay(1000);
        }
    }
    else if (M5.BtnA.wasReleasefor(800)) {
        // Break to reset
        timerStop(clock_timer);
        timerStop(update_timer);
        sandglass.shutdown();
        d2.Delay(1000);
        // Set the flag to break in while loop
        _is_break = true;
    }
    else if (M5.BtnB.wasReleased()) {
        // Restart from the initial
        timerStop(clock_timer);
        timerStop(update_timer);

        sandglass.restart(&CountdownStruct);
        start_timers();
        d2.Delay(1000);
    }
}

/*
    Every 1 second, clock tick.
*/
static void clock_update(void) {
    sandglass.tick();
    led_heartbeat();
}

/*
    Set the flag to refresh the led matrices
*/
static void ledmatrix_refresh(void) {
    sandglass.need_lm_refresh = true;
}

static void led_heartbeat(void) {
    digitalWrite(M5_LED, 1 - digitalRead(M5_LED));
}

static void Lcd_Setup(void) {
    M5.Lcd.setRotation(1);          // Horizontal view
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setCursor(M5.Lcd.width()/2 - M5.Lcd.textWidth("Hello World"), M5.Lcd.height()/2 - M5.Lcd.fontHeight()/2);
    M5.Lcd.setTextColor(TFT_RED);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Hello World");
    Disbuff.pushSprite(0, 0);
    delay(500);
    M5.Lcd.fillScreen(TFT_BLACK);
}

/*
    Creat or start this two timers. If created, just restart and start them.
*/
static void start_timers(void) {
    if (clock_timer == NULL) {
        clock_timer = timer1s(0, clock_update, true);   // Using Timer 0
        if (clock_timer == NULL) {
            Serial.println("Start clock_timer error!");
        }
    }
    else {
        timerRestart(clock_timer);
        timerStart(clock_timer);
    }
    
    if (update_timer == NULL) {
        // Using Timer 1
        update_timer = milli_timer(sandglass.frame_refresh_interval, 1, ledmatrix_refresh, true);
        if (update_timer == NULL) {
            Serial.println("Start update_timer error!");
        }
    }
    else {
        timerRestart(update_timer);
        timerStart(update_timer);
    }
}