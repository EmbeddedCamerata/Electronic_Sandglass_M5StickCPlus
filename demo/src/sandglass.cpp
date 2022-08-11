#include "../include/sandglass.h"
#include <M5StickCPlus.h>

#define LEDMATRIX_D         GPIO_NUM_26	// Data pin
#define LEDMATRIX_SRCLK1    GPIO_NUM_25	// Clock pin
#define LEDMATRIX_SRCLK2    GPIO_NUM_33 // Clock pin
#define LEDMATRIX_RCLK		GPIO_NUM_0  // Latch pin
#define MUTEX_PIN           GPIO_NUM_36 // Remember float input

static int get_total_drop_steps(int _start_layer);
const uint8_t Smile_Face[8] = {0x7E, 0x42, 0XA5, 0x81, 0xA5, 0x99, 0x42, 0x7E};

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

    this->last_set_time = {.mins = 0, .secs = 0};
}

void Sandglass2::start(Countdown_TypeDef* CountdownStruct) {
    int sx, sy;
    this->isActivated = true;
	this->rest_time.mins = CountdownStruct->mins;
    this->rest_time.secs = CountdownStruct->secs;
    this->frame_refresh_interval = (int)((CountdownStruct->mins * 60 + CountdownStruct->secs)*1000 / get_total_drop_steps(LEDMATRIX_START_LAYER));

    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.fillRect(10, 10, M5.Lcd.textWidth("Set"), M5.Lcd.fontHeight(), TFT_BLACK);
    M5.Lcd.printf("Counting down...");

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
    this->isTick = true;
}

void IRAM_ATTR Sandglass2::clock_update(void) {
    this->rest_time.mins = (this->rest_time.secs == 0) ? this->rest_time.mins - 1 : this->rest_time.mins;
    this->rest_time.secs = (this->rest_time.secs == 0) ? 59 : this->rest_time.secs - 1;
    this->isTick = false;

    // Show countdown
    this->show_countdown(&this->rest_time);
    
    if (this->rest_time.mins == 0 and this->rest_time.secs == 0) {
		this->stop();
	}
}

void IRAM_ATTR Sandglass2::ledmatrix_update(void) {
    int i;
    M5.IMU.getAccelData(&this->accX, &this->accY, &this->accZ);
    // M5.Lcd.setCursor(10, 45);
    // M5.Lcd.printf("X:%5.2f/nY:%5.2f/nZ:%5.2f ", accX, accY, accZ);

    // Using acceleration
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

    this->need_lm_refresh = false;
}

void Sandglass2::stop(void) {
    this->isActivated = false;
	this->isTick = false;
	this->rest_time.mins = 0;
	this->rest_time.secs = 0;

    this->frame_refresh_interval = 0;
    this->need_lm_refresh = false;

    this->last_set_time = {.mins = 0, .secs = 0};

    this->m1.fill(0);
    this->m2.fill(0);

    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.fillRect(10, 10, M5.Lcd.textWidth("Counting down..."), M5.Lcd.fontHeight(), TFT_BLACK);
    M5.Lcd.printf("Time up!");
}

void Sandglass2::show_countdown(Countdown_TypeDef* CountdownStruct) {
    String _time = "%02d:%02d\n";

    M5.Lcd.setCursor(60, M5.Lcd.height()/2 - 12);
    M5.Lcd.setTextDatum(TL_DATUM);
    M5.Lcd.setTextColor(TFT_RED);
    M5.Lcd.setTextSize(4);
    if (this->last_set_time.mins != CountdownStruct->mins or this->last_set_time.secs != CountdownStruct->secs) {
        M5.Lcd.fillRect(60, M5.Lcd.height()/2 - 12, M5.Lcd.textWidth(_time), M5.Lcd.fontHeight(), TFT_BLACK);
        
        this->last_set_time.mins = CountdownStruct->mins;
        this->last_set_time.secs = CountdownStruct->secs;
    }
    M5.Lcd.printf(_time.c_str(), CountdownStruct->mins, CountdownStruct->secs);
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
    if (!this->isTick) {
        m1.show();
        m2.show();
    } 
}

int get_total_drop_steps(int _start_layer) {
    int _axis = 16 - _start_layer;
    int _add_to = 5 - (_start_layer - 7) / 2;
    int i, _total = 38;             // Need an amend value
    
    for (i = 8; i >= _add_to; i--) {
        _total += i * (i*2 - 1);    // + (16 - i);
        if (i <= _axis - 1) {
            _total -= 2*i * (_axis - i);
        }
    }

    return _total;
}

void IRAM_ATTR Sandglass2::random_idle(void) {
    int _x, _y;
    _x = rand() % 8;
    _y = rand() % 8;
    this->m1.__setitem__(_x, _y, 1 - this->m1.__getitem__(_x, _y));

    _x = rand() % 8;
    _y = rand() % 8;
    this->m2.__setitem__(_x, _y, 1 - this->m2.__getitem__(_x, _y));
}

void Sandglass2::smile_face(void) {
    this->m1.write(Smile_Face);
    this->m2.write(Smile_Face);

    this->m1.show();
    this->m2.show();
}