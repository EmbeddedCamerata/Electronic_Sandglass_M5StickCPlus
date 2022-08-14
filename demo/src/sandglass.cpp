#include "../include/sandglass.h"
#include <M5StickCPlus.h>

#define LEDMATRIX_D         GPIO_NUM_26	// Data pin
#define LEDMATRIX_SRCLK1    GPIO_NUM_25	// Clock pin
#define LEDMATRIX_SRCLK2    GPIO_NUM_33 // Clock pin
#define LEDMATRIX_RCLK		GPIO_NUM_0  // Latch pin

static int get_total_drop_steps(int _start_layer);

Sandglass::Sandglass(): \
    sand1(matrix_sand::MatrxiSand(_COLUMNS, _ROWS)), \
    sand2(matrix_sand::MatrxiSand(_COLUMNS, _ROWS)), \
    m1(LedMatrix(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK1, false)), \
    m2(LedMatrix(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK2, false)) {}; // auto_write=false is necessary

void Sandglass::init(void) {
    this->isActivated = false;
    this->isWorking = false;
	this->isTick = false;

	this->rest_time = {.mins = 0, .secs = 0};
    this->last_set_time = {.mins = 0, .secs = 0};

    this->updated1 = this->updated2 = false;
    this->frame_refresh_interval = 0;
    this->need_lm_refresh = false;
}

void Sandglass::start(Countdown_TypeDef* CountdownStruct) {
    int sx, sy;
    this->isActivated = true;
    this->isWorking = true;
	this->rest_time.mins = CountdownStruct->mins;
    this->rest_time.secs = CountdownStruct->secs;
    this->frame_refresh_interval = (int)((CountdownStruct->mins * 60 + CountdownStruct->secs)*1000 / get_total_drop_steps(LEDMATRIX_START_LAYER));

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

void Sandglass::pause(void) {
    this->isWorking = false;
    M5.Lcd.fillRect(10, 10, M5.Lcd.height(), M5.Lcd.fontHeight(), TFT_BLACK);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Pause");
}

void Sandglass::resume(void) {
    this->isWorking = true;
    M5.Lcd.fillRect(10, 10, M5.Lcd.height(), M5.Lcd.fontHeight(), TFT_BLACK);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Resume");
}

void Sandglass::restart(Countdown_TypeDef* CountdownStruct) {
    M5.Lcd.fillRect(10, 10, M5.Lcd.height(), M5.Lcd.fontHeight(), TFT_BLACK);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Restart");

    this->show_countdown(CountdownStruct);
    this->start(CountdownStruct);
}

void Sandglass::tick(void) {
    this->isTick = true;
}

void IRAM_ATTR Sandglass::clock_update(void) {
    this->rest_time.mins = (this->rest_time.secs == 0) ? this->rest_time.mins - 1 : this->rest_time.mins;
    this->rest_time.secs = (this->rest_time.secs == 0) ? 59 : this->rest_time.secs - 1;
    this->isTick = false;

    // Show countdown
    this->show_countdown(&this->rest_time);
    
    if (this->rest_time.mins == 0 and this->rest_time.secs == 0) {
		this->stop();
	}
}

void IRAM_ATTR Sandglass::ledmatrix_update(void) {
    int i;
    M5.IMU.getAccelData(&this->accX, &this->accY, &this->accZ);

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

void Sandglass::stop(bool is_shutdown) {
    this->isActivated = false;
    this->isWorking = false;
	this->isTick = false;

	this->rest_time = {.mins = 0, .secs = 0};
    this->last_set_time = {.mins = 0, .secs = 0};

    this->updated1 = this->updated2 = false;
    this->frame_refresh_interval = 0;
    this->need_lm_refresh = false;

    this->sand1.clear();
    this->sand2.clear();

    this->m1.fill(0);
    this->m2.fill(0);
    this->frame_refresh();

    if (is_shutdown) {
        M5.Lcd.fillRect(10, 10, M5.Lcd.height(), M5.Lcd.fontHeight(), TFT_BLACK);
        M5.Lcd.setCursor(10, 10);
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.setTextSize(2);
        M5.Lcd.printf("Reset");
    }
    else {
        M5.Lcd.fillRect(10, 10, M5.Lcd.height(), M5.Lcd.fontHeight(), TFT_BLACK);
        M5.Lcd.setCursor(10, 10);
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.setTextSize(2);
        M5.Lcd.printf("Time up!");
    }
}

void Sandglass::shutdown(void) {
    this->stop(true);
}

void Sandglass::show_countdown(Countdown_TypeDef* CountdownStruct) {
    String _time = "%02d:%02d\n";

    M5.Lcd.setCursor(60, M5.Lcd.height()/2 - 12);
    M5.Lcd.setTextDatum(TL_DATUM);
    M5.Lcd.setTextColor(TFT_RED);
    M5.Lcd.setTextSize(4);

    if (this->last_set_time.mins != CountdownStruct->mins or this->last_set_time.secs != CountdownStruct->secs) {
        M5.Lcd.fillRect(M5.Lcd.width()/2 - M5.Lcd.textWidth(_time), M5.Lcd.height()/2 - M5.Lcd.fontHeight()/2, M5.Lcd.textWidth(_time), M5.Lcd.fontHeight(), TFT_BLACK);
        this->last_set_time.mins = CountdownStruct->mins;
        this->last_set_time.secs = CountdownStruct->secs;
    }
    M5.Lcd.printf(_time.c_str(), CountdownStruct->mins, CountdownStruct->secs);
}

bool Sandglass::is_activated(void) {
    return this->isActivated;
}

bool Sandglass::is_working(void) {
    return this->isWorking;
}

void Sandglass::update_matrix(LedMatrix *m, matrix_sand::MatrxiSand *s) {
    int i, j;
    for (i = 0; i < _COLUMNS; i++) {
        for (j = 0; j < _ROWS; j++) {
            m->__setitem__(i, j, s->__getitem__(i, j));
        }
    }
}

void IRAM_ATTR Sandglass::frame_refresh(void) {
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

void Sandglass::random_idle(void) {
    int _x, _y;
    _x = rand() % 8;
    _y = rand() % 8;
    this->m1.__setitem__(_x, _y, 1 - this->m1.__getitem__(_x, _y));

    _x = rand() % 8;
    _y = rand() % 8;
    this->m2.__setitem__(_x, _y, 1 - this->m2.__getitem__(_x, _y));
}