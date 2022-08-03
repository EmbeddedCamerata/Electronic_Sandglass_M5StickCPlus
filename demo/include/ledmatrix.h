#ifndef _LEDMATRIX_H_
#define _LEDMATRIX_H_

#include <Arduino.h>
#include <AceSPI.h>

#define LEDMATRIX_D         GPIO_NUM_26	// Data pin
#define LEDMATRIX_SRCLK1    GPIO_NUM_25	// Clock pin
#define LEDMATRIX_SRCLK2    GPIO_NUM_33 // Clock pin
#define LEDMATRIX_RCLK		GPIO_NUM_0  // Latch pin
#define MUTEX_PIN           GPIO_NUM_36 // Remember float input

#define LIGHT_8_MASK        ((uint8_t)0xFF)
#define LIGHT_7_MASK        ((uint8_t)0x7F)
#define LIGHT_6_MASK        ((uint8_t)0x3F)
#define LIGHT_5_MASK        ((uint8_t)0x1F)
#define LIGHT_4_MASK        ((uint8_t)0x0F)
#define LIGHT_3_MASK        ((uint8_t)0x07)
#define LIGHT_2_MASK        ((uint8_t)0x03)
#define LIGHT_1_MASK        ((uint8_t)0x01)
#define LIGHT_0_MASK        ((uint8_t)0x00)

typedef struct {
    int layer;
	uint8_t matrix_data[8];
    uint8_t led_inlayer;
} LedMatrix_TypeDef;

template <typename T_SPII>
class SoftSpi {
    public:
    explicit SoftSpi(const T_SPII& spiInterface) : mSpiInterface(spiInterface) {};

    template <typename _pt>
    void show(uint8_t data[8], _pt _time) {
        int i;
        for (i = 0; i < 8; i++) {
            LedMatrixStruct.matrix_data[i] = data[i];
        }
        SoftSpi::refresh_for(_time);
    }

    void clear(void) {
        int i;
        for (i = 0; i < 8; i++) {
            LedMatrixStruct.matrix_data[i] = LIGHT_0_MASK;
        }
        LedMatrixStruct.layer = 0;
        SoftSpi::refresh();
    }

    void full(void) {
        int i;
        for (i = 0; i < 8; i++) {
            LedMatrixStruct.matrix_data[i] = LIGHT_8_MASK;
        }
        LedMatrixStruct.layer = 15;
        SoftSpi::refresh();
    }

    void drop_layers(int layer) {
        int i, j;
        for (i = 0; i < layer; i++) {
            for (j = 7; j > 0; j--) {
                LedMatrixStruct.matrix_data[j] = LedMatrixStruct.matrix_data[j-1];
            }
            if (LedMatrixStruct.layer >= 8) {
                LedMatrixStruct.matrix_data[0] &= (0x7F >> (15 - LedMatrixStruct.layer));
            }
            else {
                LedMatrixStruct.matrix_data[0] = LIGHT_0_MASK;
            }
            LedMatrixStruct.layer--;
        }
        SoftSpi::refresh();
    }

    void add_layers(int layer) {

    }

    private:
    LedMatrix_TypeDef LedMatrixStruct;
    const T_SPII mSpiInterface; // copied by value

    /*
        Refresh for a period of time, unit second. Float or int.
    */
    template <typename _pt>
    void refresh_for(_pt _time) {
        unsigned long _start_tick = millis();
        unsigned long _now_tick = millis();
        while ((_now_tick - _start_tick) < (unsigned long)(_time * 1000)) {
            SoftSpi::refresh();
            _now_tick = millis();
        }
    }

    void refresh(void) {
        int i;
        uint8_t buf[2];
        for (i = 0; i < 8; i++) {
            buf[0] = LedMatrixStruct.matrix_data[i];
            buf[1] = 0x01 << i;     // Choose the row
            mSpiInterface.send16(buf[1], buf[0]);
            delay(2);
        }
    };
};

void LedMatrix_Init(void);
void LedMatrix_Test(void);

#endif