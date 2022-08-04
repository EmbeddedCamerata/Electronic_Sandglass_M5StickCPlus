#include "../include/ledmatrix.h"

static uint8_t bigheart[8] = {0x00, 0x66, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00};
static uint8_t smallheart[8] = {0x00, 0x24, 0x7E, 0x7E, 0x3C, 0x18, 0x00, 0x00};
    // 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};

template <class T_SPII>
void SoftSpi<T_SPII>::show(uint8_t data[8], float _interval) {
    int i;
    for (i = 0; i < 8; i++) {
        this->LedMatrixStruct.matrix_data[i] = data[i];
    }
    this->refresh_for(_interval);
}

template <class T_SPII>
void SoftSpi<T_SPII>::clear(void) {
    int i;
    for (i = 0; i < 8; i++) {
        this->LedMatrixStruct.matrix_data[i] = LIGHT_0_MASK;
    }
    this->LedMatrixStruct.layer = 0;
    this->refresh();
}

template <class T_SPII>
void SoftSpi<T_SPII>::full(void) {
    int i;
    for (i = 0; i < 8; i++) {
        this->LedMatrixStruct.matrix_data[i] = LIGHT_8_MASK;
    }
    this->LedMatrixStruct.layer = 15;
    this->refresh();
}

template <class T_SPII>
void SoftSpi<T_SPII>::decrease(int layer) {
    int i, j, temp;
    for (i = 0; i < layer; i++) {
        for (j = 7; j >= 0; j--) {
            temp = (7 - this->LedMatrixStruct.layer + j) < 8 ? (15 - this->LedMatrixStruct.layer + j) : 8;  // ?
            this->LedMatrixStruct.matrix_data[j] = Light_Mask[temp];
        }
        this->LedMatrixStruct.layer--;
    }
    this->refresh();
}

template <class T_SPII>
void SoftSpi<T_SPII>::add(int layer) {
    int i, j, temp;
    for (i = 0; i < layer; i++) {
        for (j = 7; j >= 0; j--) {
            temp = (this->LedMatrixStruct.layer + j - 7) > 8 ? 8 : (this->LedMatrixStruct.layer + j - 7);
            this->LedMatrixStruct.matrix_data[j] = Light_Mask[temp];
        }
        this->LedMatrixStruct.layer++;
    }
    this->refresh();
}

template <class T_SPII>
void SoftSpi<T_SPII>::drop(int layer) {
    int drop_len = 7 - LedMatrixStruct.layer/2; // e.g. layer=9, just drop 3

}

template <class T_SPII>
void SoftSpi<T_SPII>::refresh_for(float _interval) {
    unsigned long _start_tick = millis();
    unsigned long _now_tick = millis();
    while ((_now_tick - _start_tick) < (unsigned long)(_interval * 1000)) {
        this->refresh();
        _now_tick = millis();
    }
}

template <class T_SPII>
void SoftSpi<T_SPII>::refresh(void) {
    int i;
    uint8_t buf[2];
    for (i = 0; i < 8; i++) {
        buf[0] = this->LedMatrixStruct.matrix_data[i];
        buf[1] = 0x01 << i;     // Choose the row
        this->mSpiInterface.send16(buf[1], buf[0]);
        delay(2);
    }
}

LedMatrix::LedMatrix(): lm_top(SimpleSpiInterface(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK1)),    \
    lm_bottom(SimpleSpiInterface(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK2)), isWorking(false) {}

void LedMatrix::test(void) {
    this->lm_top.show(bigheart, 1.5);
    this->lm_bottom.show(smallheart, 1.5);
}

void LedMatrix::init(void) {
    this->lm_top.get_spiInterface()->begin();
    this->lm_bottom.get_spiInterface()->begin();
    gpio_pulldown_dis(MUTEX_PIN);
    gpio_pullup_dis(MUTEX_PIN);
}

void LedMatrix::start(void) {
    this->isWorking = true;
    this->lm_top.full();
    this->lm_bottom.clear();
}

void LedMatrix::stop(void) {
    this->isWorking = false;
    this->lm_top.clear();
    this->lm_bottom.full();
}

void LedMatrix::update(void) {
    // 1. Top decreases one layer
    this->lm_top.decrease(1);

    // 2. Drop

    // 3. Bottom adds one layer
    this->lm_bottom.add(1);
}

void LedMatrix::refresh(void) {
    int i;
    uint8_t buf[3];
    for (i = 0; i < 8; i++) {
        buf[0] = this->lm_top.LedMatrixStruct.matrix_data[i];
        buf[1] = this->lm_bottom.LedMatrixStruct.matrix_data[i];
        buf[2] = 0x01 << i;     // Choose the row
        this->lm_top.get_spiInterface()->send16(buf[2], buf[0]);
        this->lm_bottom.get_spiInterface()->send16(buf[2], buf[1]);
        delay(2);
    }
}

void LedMatrix::refresh_for(float _interval) {
    unsigned long _start_tick = millis();
    unsigned long _now_tick = millis();
    while ((_now_tick - _start_tick) < (unsigned long)(_interval * 1000)) {
        this->refresh();
        _now_tick = millis();
    }
}