#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/src/user.cpp"
#include <M5StickCPlus.h>
#include "../include/user.h"
#include "../include/matrixsand.h"
#include "../include/ledmatrix.h"
#include "../include/sandglass.h"
#include "../include/bsp_timer.h"
#include "../include/nonblock_delay.h"

#define LEDMATRIX_D         GPIO_NUM_26	// Data pin
#define LEDMATRIX_SRCLK1    GPIO_NUM_25	// Clock pin
#define LEDMATRIX_SRCLK2    GPIO_NUM_33 // Clock pin
#define LEDMATRIX_RCLK		GPIO_NUM_0  // Latch pin
#define MUTEX_PIN           GPIO_NUM_36 // Remember float input

TFT_eSprite Disbuff = TFT_eSprite(&M5.Lcd);
Countdown_TypeDef CountdownStruct = {.mins = 0, .secs = 0};
Sandglass2 sandglass2;
hw_timer_t* rtc_timer = NULL;
hw_timer_t* update_timer = NULL;
NonBlockDelay d;

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
}

void User_Loop(void) {

    if (not sandglass2.is_activated()) {
        Key_Handle();
    }

    while (1) {
        if (sandglass2.isTick) {
            sandglass2.clock_update();
            Disbuff.pushSprite(0, 0);
        }
        
        // Frame refresh as frequently as you can when not completed or tick.
        sandglass2.frame_refresh();

        if (sandglass2.need_lm_refresh) {
            sandglass2.ledmatrix_update();  // data update
        }
        
        if (not sandglass2.is_activated()) {
            // Stop timers
            timerStop(rtc_timer);
            timerStop(update_timer);
            break;
        }
    }
    delay(1000);
}

static void Key_Handle(void) {

    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Set");
    Disbuff.pushSprite(0, 0);

    d.Delay(500);

    while (1) {
        M5.update();
        if (M5.BtnA.wasReleasefor(800)) {
            sandglass2.start(&CountdownStruct);

            rtc_timer = timer1s(0, clock_update, true);
            if (rtc_timer == NULL) {
                Serial.println("Start rtc_timer error!");
            }
            update_timer = milli_timer(sandglass2.frame_refresh_interval, 1, ledmatrix_refresh, true);   // ms timer
            if (update_timer == NULL) {
                Serial.println("Start update_timer error!");
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

        if (d.Timeout()) {
            sandglass2.random_idle();
            d.Delay(500);
        }
        sandglass2.frame_refresh();
        
        sandglass2.show_countdown(&CountdownStruct);
        Disbuff.pushSprite(0, 0);

        // If idle, can go into low-cost mode
        // delay(100);
    }

}

/*
    Every 1 second, rtc tick.
*/
static void clock_update(void) {
    // In timer interrupt, reading RTC causes watchdog failed, core panic!
    sandglass2.tick();
    digitalWrite(M5_LED, 1 - digitalRead(M5_LED));
}

static void ledmatrix_refresh(void) {
    sandglass2.need_lm_refresh = true;
}

static void led_heartbeat(void) {
    digitalWrite(M5_LED, 1 - digitalRead(M5_LED));
}

static void Lcd_Setup(void) {
    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setCursor(80, 80);
    M5.Lcd.printf("Hello World\n");
    Disbuff.pushSprite(0, 0);

    sandglass2.smile_face();
    delay(500);
}