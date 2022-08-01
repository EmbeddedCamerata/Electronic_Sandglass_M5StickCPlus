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

    void show(uint8_t data[8]) {
        int i;
        for (i = 0; i < 8; i++) {
            LedMatrixStruct.matrix_data[i] = data[i];
        }
        SoftSpi::refresh();
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

    void drop_one_layer(void) {
        int i;
        if (LedMatrixStruct.layer >= 8) {
            for (i = 7; i > 0; i--) {
                LedMatrixStruct.matrix_data[i] = LedMatrixStruct.matrix_data[i-1];
            }
            LedMatrixStruct.matrix_data[0] = LedMatrixStruct.matrix_data[0] & (0x7F >> (15 - LedMatrixStruct.layer));
            LedMatrixStruct.layer--;
        }
        
        SoftSpi::refresh();
    }

    void add_one_sand() {

    }

    private:
    LedMatrix_TypeDef LedMatrixStruct;
    const T_SPII mSpiInterface; // copied by value

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
void ledmatrix_test(void);

#endif