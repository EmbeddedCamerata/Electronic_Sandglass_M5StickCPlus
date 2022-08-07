#ifndef _LEDMATRIX_H_
#define _LEDMATRIX_H_

#include <Arduino.h>
#include <AceSPI.h>

using ace_spi::SimpleSpiInterface;
using SpiInterface = SimpleSpiInterface;

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

const uint8_t Light_Mask[9] = {
    LIGHT_0_MASK,
    LIGHT_1_MASK,
    LIGHT_2_MASK,
    LIGHT_3_MASK,
    LIGHT_4_MASK,
    LIGHT_5_MASK,
    LIGHT_6_MASK,
    LIGHT_7_MASK,
    LIGHT_8_MASK
};

typedef struct {
    int layer;
	uint8_t matrix_data[8];
    uint8_t led_inlayer;
} LedMatrix_TypeDef;

template <typename T_SPII>
class SoftSpi {
    public:
    explicit SoftSpi(const T_SPII& spiInterface): mSpiInterface(spiInterface) {};

    void show(uint8_t data[8], float _interval);
    void clear(void);
    void full(void);
    void decrease(int layer);
    void add(int layer);
    void drop(int _cur_drop);
    void invert(uint8_t data[8]);

    const T_SPII* get_spiInterface() {
        return &mSpiInterface;
    }

    LedMatrix_TypeDef LedMatrixStruct;

    private:
    const T_SPII mSpiInterface; // copied by value

    // Refresh for a period of time, unit second. Float is universal type
    void refresh_for(float _interval);
    void refresh(void);
};

class LedMatrix {
    public:
    LedMatrix();
    void test(void);
    void init(void);
    void start(void);
    void stop(void);
    int get_drop_times(void);
    void update(int _cur_drop);
    void refresh_for(float _interval);  // Refresh two led matrices at the same time for an interval
    void refresh(void);

    protected:
    float interval; // Top class Sandglass will read it
    float drop_interval;
    int _drop_time;
    int _cur_drop; // ?

    private:
    SoftSpi<SpiInterface> lm_top;
    SoftSpi<SpiInterface> lm_bottom;
    bool isWorking;
};

#endif