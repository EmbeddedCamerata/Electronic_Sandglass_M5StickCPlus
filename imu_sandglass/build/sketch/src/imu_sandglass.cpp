#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/imu_sandglass/src/imu_sandglass.cpp"
#include "../include/imu_sandglass.h"
#include "../include/ledmatrix.h"
#include "../include/matrixsand.h"
#include <M5StickCPlus.h>

#define LEDMATRIX_D         GPIO_NUM_26	// Data pin
#define LEDMATRIX_SRCLK1    GPIO_NUM_25	// Clock pin
#define LEDMATRIX_SRCLK2    GPIO_NUM_33 // Clock pin
#define LEDMATRIX_RCLK		GPIO_NUM_0  // Latch pin
#define MUTEX_PIN           GPIO_NUM_36 // Remember float input

static void update_matrix(LedMatrix *m, matrix_sand::MatrxiSand *s);
static void gravity_sandglass(void);
static void Lcd_Setup(void);

void Imu_Sandglass_Setup(void) {
    int ret;
    pinMode(M5_LED, OUTPUT);
    gpio_pulldown_dis(MUTEX_PIN);
    gpio_pullup_dis(MUTEX_PIN);
    Lcd_Setup();

    ret = M5.IMU.Init();
    if (ret != 0) {
        Serial.println("Init IMU error!");
    }

    gravity_sandglass();
}

void update_matrix(LedMatrix *m, matrix_sand::MatrxiSand *s) {
    int i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            m->__setitem__(i, j, s->__getitem__(i, j));
        }
    }
}

void gravity_sandglass(void) {
    float accX, accY, accZ;
    float xx, yy, zz;
    int sx, sy;
    int count = 0;
    bool updated1, updated2;
    unsigned long _start_tick, _now_tick;

    matrix_sand::MatrxiSand sand1 = matrix_sand::MatrxiSand(8, 8);
    matrix_sand::MatrxiSand sand2 = matrix_sand::MatrxiSand(8, 8);

    LedMatrix m1 = LedMatrix(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK1, false);
    LedMatrix m2 = LedMatrix(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK2, false);
    
    for (sx = 0; sx < 8; sx++) {
        for (sy = 0; sy < 8; sy++) {
            sand1.__setitem__(sx, sy, true);
        }
    }

    sand1.__setitem__(0, 0, false);
    sand1.__setitem__(0, 1, false);
    sand1.__setitem__(1, 0, false);
    sand1.__setitem__(0, 2, false);
    sand1.__setitem__(1, 1, false);
    sand1.__setitem__(2, 0, false);

    update_matrix(&m1, &sand1);
    update_matrix(&m2, &sand2);

    m1.show();
    m2.show();

    updated1 = updated2 = false;

    _start_tick = millis();
    while (1) {
        _now_tick = millis();
        if ((_now_tick - _start_tick) >= (unsigned long)(1 * 1000)) {
            M5.IMU.getAccelData(&accX, &accY, &accZ);
            M5.Lcd.setCursor(10, 45);
            M5.Lcd.printf("X:%5.2f/nY:%5.2f/nZ:%5.2f ", accX, accY, accZ);

            // Maybe need to adjust accoring to your actual situation
            xx = accZ - accY;
            yy = accZ + accY;
            zz = accX;

            if (yy > 0 and sand1.__getitem__(7, 7) and not sand2.__getitem__(0, 0) and not updated2) {
                sand1.__setitem__(7, 7, false);
                sand2.__setitem__(0, 0, true);
                updated1 = updated2 = true;
            } 
            else if (yy <= 0 and sand2.__getitem__(0, 0) and not sand1.__getitem__(7, 7) and not updated1) {
                sand1.__setitem__(7, 7, true);
                sand2.__setitem__(0, 0, false);
                updated1 = updated2 = true;
            } 
            else {
                updated1 = sand1.iterate(xx, yy, zz);
                updated2 = sand2.iterate(xx, yy, zz);
            }

            if (updated1) {
                update_matrix(&m1, &sand1);
            }
            if (updated2) {
                update_matrix(&m2, &sand2);
            }
            
            digitalWrite(M5_LED, 1 - digitalRead(M5_LED));

            _start_tick = _now_tick;
        }

        m1.show();
        m2.show();
    }
}

static void Lcd_Setup(void) {
    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(80, 80);
    M5.Lcd.print("Hello World\n");
    delay(500);
    M5.Lcd.fillScreen(BLACK);
}