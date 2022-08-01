#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/src/ledmatrix.cpp"
#include "../include/ledmatrix.h"

using ace_spi::SimpleSpiInterface;
using SpiInterface = SimpleSpiInterface;
SpiInterface spiInterface1(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK1);
SoftSpi<SpiInterface> ledmatrix_top(spiInterface1);

SpiInterface spiInterface2(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK2);
SoftSpi<SpiInterface> ledmatrix_bottom(spiInterface2);

static uint8_t bigheart[8] = {0x00, 0x66, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00};
static uint8_t smallheart[8] = {0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};

void LedMatrix_Init(void)
{
    spiInterface1.begin();
    spiInterface2.begin();
    gpio_pulldown_dis(MUTEX_PIN);
    gpio_pullup_dis(MUTEX_PIN);
}

void ledmatrix_test(void) {
    ledmatrix_top.full();
    ledmatrix_bottom.show(smallheart);

    int i;
    for (i = 0; i < 15; i ++) {
        ledmatrix_top.drop_one_layer();
        delay(1000);
    }
}