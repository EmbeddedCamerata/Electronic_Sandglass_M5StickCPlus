#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/src/sandglass.cpp"
#include "../include/sandglass.h"
#include <M5StickCPlus.h>

#define LEDMATRIX_D         GPIO_NUM_26	// Data pin
#define LEDMATRIX_SRCLK1    GPIO_NUM_25	// Clock pin
#define LEDMATRIX_SRCLK2    GPIO_NUM_33 // Clock pin
#define LEDMATRIX_RCLK		GPIO_NUM_0  // Latch pin
#define MUTEX_PIN           GPIO_NUM_36 // Remember float input

static int get_total_drop_steps(int _start_layer);

Sandglass2::Sandglass2(): \
    sand1(matrix_sand::MatrxiSand(_COLUMNS, _ROWS)), \
    sand2(matrix_sand::MatrxiSand(_COLUMNS, _ROWS)), \
    m1(LedMatrix(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK1, false)), \
    m2(LedMatrix(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK2, false)) {    
};

void Sandglass2::init(void) {
	this->isActivated = false;
	this->isTick = false;
	this->rest_time.mins = 0;
	this->rest_time.secs = 0;
    this->updated1 = this->updated2 = false;
    this->need_lm_refresh = false;
    this->frame_refresh_interval = 0;
}

void Sandglass2::start(Countdown_TypeDef* CountdownStruct) {
    int sx, sy;
    this->isActivated = true;
	this->rest_time.mins = CountdownStruct->mins;
    this->rest_time.secs = CountdownStruct->secs;
    this->counter = 0;
    this->frame_refresh_interval = (int)((this->rest_time.mins * 60 + this->rest_time.secs)*1000 / get_total_drop_steps(LEDMATRIX_START_LAYER));

    this->sand1.clear();
    this->sand2.clear();

    for (sx = 0; sx < _COLUMNS; sx++) {
        for (sy = 0; sy < _ROWS; sy++) {
            if (sx + sy <= 15 - LEDMATRIX_START_LAYER - 1) {
                this->sand1.__setitem__(sx, sy, false);
            }
            else {
                this->sand1.__setitem__(sx, sy, true);
            }
        }
    }
    update_matrix(&this->m1, &this->sand1);
    this->m2.fill(0);

    this->m1.show();
    this->m2.show();

    this->updated1 = this->updated2 = false;
}

void Sandglass2::tick(void) {
    this->counter++;
    this->isTick = true;
}

void IRAM_ATTR Sandglass2::clock_update(void) {
    this->rest_time.mins = (this->rest_time.secs == 0) ? this->rest_time.mins - 1 : this->rest_time.mins;
    this->rest_time.secs = (this->rest_time.secs == 0) ? 59 : this->rest_time.secs - 1;
    this->isTick = false;

    // Show countdown
	M5.Lcd.setCursor(10, 30);
    M5.Lcd.printf("%02d mins, %02d secs left\n", this->rest_time.mins, this->rest_time.secs);
    
    if (this->rest_time.mins == 0 and this->rest_time.secs == 0) {
		M5.Lcd.setCursor(10, 50);
    	M5.Lcd.printf("Time up!\n");
		this->stop();
	}
}

void IRAM_ATTR Sandglass2::ledmatrix_update(void) {
    int i;
    M5.IMU.getAccelData(&this->accX, &this->accY, &this->accZ);
    // M5.Lcd.setCursor(10, 45);
    // M5.Lcd.printf("X:%5.2f/nY:%5.2f/nZ:%5.2f ", accX, accY, accZ);

    this->xx = this->accZ - this->accY;
    this->yy = this->accZ + this->accY;
    this->zz = this->accX;

    if (this->yy > 0 and this->sand1.__getitem__(7, 7) and not this->sand2.__getitem__(0, 0) and not this->updated2) {
        this->sand1.__setitem__(7, 7, false);
        this->sand2.__setitem__(0, 0, true);
        this->updated1 = this->updated2 = true;
    } 
    else if (this->yy <= 0 and this->sand2.__getitem__(0, 0) and not this->sand1.__getitem__(7, 7) and not this->updated1) {
        this->sand1.__setitem__(7, 7, true);
        this->sand2.__setitem__(0, 0, false);
        this->updated1 = this->updated2 = true;
    } 
    else {
        this->updated1 = this->sand1.iterate(this->xx, this->yy, this->zz);
        this->updated2 = this->sand2.iterate(this->xx, this->yy, this->zz);
    }

    if (this->updated1) {
        this->update_matrix(&this->m1, &this->sand1);
    }
    if (this->updated2) {
        this->update_matrix(&this->m2, &this->sand2);
    }
}

void Sandglass2::stop(void) {
    this->isActivated = false;
	this->isTick = false;
    this->counter = 0;
	this->rest_time.mins = 0;
	this->rest_time.secs = 0;

    this->frame_refresh_interval = 0;
    this->need_lm_refresh = false;

    this->sand1.clear();
    this->sand2.clear();

    this->m1.fill(0);
    this->m2.fill(0);
}

void Sandglass2::show_settime(Countdown_TypeDef* CountdownStruct) {
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.printf("%02d mins, %02d secs set\n", CountdownStruct->mins, CountdownStruct->secs);
}

bool Sandglass2::is_activated(void) {
    return this->isActivated;
}

void Sandglass2::update_matrix(LedMatrix *m, matrix_sand::MatrxiSand *s) {
    int i, j;
    for (i = 0; i < _COLUMNS; i++) {
        for (j = 0; j < _ROWS; j++) {
            m->__setitem__(i, j, s->__getitem__(i, j));
        }
    }
}

void IRAM_ATTR Sandglass2::frame_refresh(void) {
    int i;
    for (i = 0; i < FRAME_REFRESHMENT_FREQ; i++) {  
        if (this->isTick) {
            break;
        }
        else {
            m1.show();
            m2.show();
        }
    }
}

int get_total_drop_steps(int _start_layer) {
    int _axis = 16 - _start_layer;
    int _add_to = 5 - (_start_layer - 7) / 2;
    int i, _total;
    
    for (i = 8; i >= _add_to; i--) {
        _total += i * (i*2 - 1);
        if (i <= _axis - 1) {
            _total -= 2*i * (_axis - i);
        }
    }

    return _total;
}

// void test(void) {
//     matrix_sand::MatrxiSand sand1 = matrix_sand::MatrxiSand(8, 8);
//     matrix_sand::MatrxiSand sand2 = matrix_sand::MatrxiSand(8, 8);

//     LedMatrix m1 = LedMatrix(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK1, false);
//     LedMatrix m2 = LedMatrix(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK2, false);

//     unsigned long _start_tick, _now_tick;
//     int i, j;
//     int x, y;
//     for (i = 0; i < 8; i++) {
//         sand1.__setitem__(i, i, true);
//     }

//     sand2.__setitem__(0, 0, true);
//     sand2.__setitem__(1, 1, true);
//     sand2.__setitem__(2, 2, true);
//     sand2.__setitem__(3, 3, true);
//     sand2.__setitem__(4, 4, true);
//     sand2.__setitem__(5, 5, true);
//     sand2.__setitem__(6, 6, true);
//     sand2.__setitem__(7, 7, true);

//     // update_matrix(m1, sand1);
//     update_matrix(&m2, &sand2);
//     m1.show();
//     // m1.fill();
//     // m2.fill();

//     M5.Lcd.setCursor(10, 15);
//     for (i = 0; i < 8; i++) {
//         M5.Lcd.printf("%d ", sand2.__getitem__(i, i));
//     }

//     M5.Lcd.setCursor(10, 30);
//     for (i = 0; i < 7; i++) {
//         M5.Lcd.printf("0x%02X ", m2._get_line(i));
//     }

//     for (i = 0; i < 40; i++) {
//         m1.show();
//         m2.show();
//         // delay(10);
//     }

//     m1.fill(0);
//     m2.fill(0);
//     m1.show();

//     _start_tick = millis();
//     while (1) {
//         M5.Lcd.setCursor(10, 15);
//         for (i = 0; i < 7; i++) {
//             M5.Lcd.printf("0x%02X ", m2._get_line(i));
//         }

//         _now_tick = millis();
//         if ((_now_tick - _start_tick) >= (unsigned long)(0.5 * 1000)) {
//             x = rand()%8;
//             y = rand()%8;
//             m2.__setitem__(x , y, 1 - m2.__getitem__(x, y));
//             _start_tick = _now_tick;
//         }
//         m2.show();
//     }
// }

// void common_test(void) {
//     float accX, accY, accZ;
//     float xx, yy, zz;
//     int sx, sy;
//     int count = 0;
//     bool updated1, updated2;
//     unsigned long _start_tick, _now_tick;

//     matrix_sand::MatrxiSand sand1 = matrix_sand::MatrxiSand(8, 8);
//     matrix_sand::MatrxiSand sand2 = matrix_sand::MatrxiSand(8, 8);

//     LedMatrix m1 = LedMatrix(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK1, false);
//     LedMatrix m2 = LedMatrix(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK2, false);
    
//     for (sx = 0; sx < 8; sx++) {
//         for (sy = 0; sy < 8; sy++) {
//             sand1.__setitem__(sx, sy, true);
//         }
//     }

//     sand1.__setitem__(0, 0, false);
//     sand1.__setitem__(0, 1, false);
//     sand1.__setitem__(1, 0, false);
//     sand1.__setitem__(0, 2, false);
//     sand1.__setitem__(1, 1, false);
//     sand1.__setitem__(2, 0, false);

//     update_matrix(&m1, &sand1);
//     update_matrix(&m2, &sand2);

//     m1.show();
//     m2.show();

//     updated1 = updated2 = false;

//     _start_tick = millis();
//     while (1) {
//         _now_tick = millis();
//         if ((_now_tick - _start_tick) >= (unsigned long)(1 * 1000)) {
//             M5.IMU.getAccelData(&accX, &accY, &accZ);
//             M5.Lcd.setCursor(10, 45);
//             M5.Lcd.printf("X:%5.2f/nY:%5.2f/nZ:%5.2f ", accX, accY, accZ);

//             xx = -accZ - accY;
//             yy = -accZ + accY;
//             zz = accX;

//             if (yy > 0 and sand1.__getitem__(7, 7) and not sand2.__getitem__(0, 0) and not updated2) {
//                 sand1.__setitem__(7, 7, false);
//                 sand2.__setitem__(0, 0, true);
//                 updated1 = updated2 = true;
//             } 
//             else if (yy <= 0 and sand2.__getitem__(0, 0) and not sand1.__getitem__(7, 7) and not updated1) {
//                 sand1.__setitem__(7, 7, true);
//                 sand2.__setitem__(0, 0, false);
//                 updated1 = updated2 = true;
//             } 
//             else {
//                 updated1 = sand1.iterate(xx, yy, zz);
//                 updated2 = sand2.iterate(xx, yy, zz);
//             }

//             if (updated1) {
//                 update_matrix(&m1, &sand1);
//             }
//             if (updated2) {
//                 update_matrix(&m2, &sand2);
//             }
//             _start_tick = _now_tick;
//         }
        
//         // For debugging
//         M5.Lcd.setCursor(10, 15);
//         for (int i = 0; i < 7; i++) {
//             M5.Lcd.printf("0x%02X ", m1._get_line(i));
//         }

//         m1.show();
//         m2.show();
//     }
// }