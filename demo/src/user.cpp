#include <M5StickCPlus.h>
#include "../include/user.h"
#include "../include/matrixsand.h"
#include "../include/ledmatrix.h"
#include "../include/sandglass.h"
#include "../include/bsp_timer.h"

#define LEDMATRIX_D         GPIO_NUM_26	// Data pin
#define LEDMATRIX_SRCLK1    GPIO_NUM_25	// Clock pin
#define LEDMATRIX_SRCLK2    GPIO_NUM_33 // Clock pin
#define LEDMATRIX_RCLK		GPIO_NUM_0  // Latch pin
#define MUTEX_PIN           GPIO_NUM_36 // Remember float input

Countdown_TypeDef CountdownStruct = {.mins = 0, .secs = 0};
Sandglass2 sandglass2;
hw_timer_t* rtc_timer;
hw_timer_t* update_timer;

static void Lcd_Setup(void);
static void Key_Handle(void);
static void clock_update(void);
static void ledmatrix_refresh(void);
static void led_heartbeat(void);

void User_Setup(void) {
    int ret;
    pinMode(M5_LED, OUTPUT);
    pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
    gpio_pulldown_dis(MUTEX_PIN);
    gpio_pullup_dis(MUTEX_PIN);

    sandglass2.init();
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
        if (sandglass2.isTick) {
            sandglass2.clock_update();
            
        }
        else {
            sandglass2.frame_refresh();
        }

        if (sandglass2.need_lm_refresh) {
            sandglass2.ledmatrix_update();
        }
        
        if (not sandglass2.is_activated()) {
            timerStop(rtc_timer);
            timerStop(update_timer);
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
            rtc_timer = timer1s(0, clock_update, true);
            if (rtc_timer == NULL) {
                Serial.println("Start rtc_timer error!");
            }
            update_timer = milli_timer(sandglass2.frame_refresh_interval, 1, ledmatrix_refresh, true);   // ms timer
            if (update_timer == NULL) {
                Serial.println("Start update_timer error!");
            }

            sandglass2.start(&CountdownStruct);
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
static void clock_update(void) {
    // In timer interrupt, reading RTC causes watchdog failed, core panic!
    sandglass2.tick();
}

static void ledmatrix_refresh(void) {
    sandglass2.need_lm_refresh = true;
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