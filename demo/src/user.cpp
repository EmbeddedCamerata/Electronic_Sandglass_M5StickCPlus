#include <M5StickCPlus.h>
// #include "../include/sandglass.h"
#include "../include/user.h"
// #include "../include/ledmatrix.h"
#include "../include/sandglass.h"
#include "../include/bsp_timer.h"

#define MUTEX_PIN           GPIO_NUM_36 // Remember float input

Countdown_TypeDef CountdownStruct = {.mins = 0, .secs = 0};
Sandglass2 sandglass2;
unsigned long _now_tick, _start_tick;
hw_timer_t* rtc_timer;

static void Lcd_Setup(void);
static void Key_Handle(void);
static void rtc_update(void);
static void led_heartbeat(void);

void User_Setup(void) {
    int ret;
    pinMode(M5_LED, OUTPUT);
    pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
    gpio_pulldown_dis(MUTEX_PIN);
    gpio_pullup_dis(MUTEX_PIN);

    // sandglass.init();
    Lcd_Setup();

    ret = M5.IMU.Init();
    if (ret != 0) {
        Serial.println("Init IMU error!");
    }
    // test();
    // common_test();
}

void User_Loop(void) {
    if (not sandglass2.is_activated()) {
        Key_Handle();
    }

    while (1) {
        // sandglass2.update(); // Update RTC and led matrices data
        if (sandglass2.is_tick()) {
            sandglass2.update();
        }
        
        if (not sandglass2.is_activated()) {
            timerStop(rtc_timer);
            break;
        }
    }
    led_heartbeat();
    // sandglass2.tick();
}

static void Key_Handle(void) {
    
    while (1) {
        M5.update();
        if (M5.BtnA.wasReleasefor(800)) {
            sandglass2.start(&CountdownStruct);
            rtc_timer = timer1s(0, rtc_update, true);
            if (rtc_timer == NULL) {
                Serial.println("Start rtc_timer error!");
            }
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

        sandglass2.show_settime(&CountdownStruct);
        // If idle, can go into low-cost mode
        delay(50);
    }

}

/*
    Every 1 second, rtc tick.
*/
static void rtc_update(void) {
    // In timer interrupt, reading RTC causes watchdog failed, core panic!

    sandglass2.tick();

    // sandglass2.reset_tick();
}

static void led_heartbeat(void) {
    digitalWrite(M5_LED, 1 - digitalRead(M5_LED));
}

static void Lcd_Setup(void) {
    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(80, 80);
    M5.Lcd.print("Hello World\n");
    delay(500);
    M5.Lcd.fillScreen(BLACK);
}