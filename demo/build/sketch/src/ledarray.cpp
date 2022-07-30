#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/src/ledarray.cpp"
#include "../include/ledarray.h"

void ledarray_init(void)
{
    pinMode(LEDARRAY_D, OUTPUT);
    pinMode(LEDARRAY_RCLK, OUTPUT);
    pinMode(LEDARRAY_SRCLK, OUTPUT);
    pinMode(MUTEX_PIN, ANALOG);
}

void show_on_ledarray()
{

}

void scan_row(uint8_t _row, uint8_t _col)
{
    int i;
    digitalWrite(LEDARRAY_RCLK, LOW);
    digitalWrite(LEDARRAY_SRCLK, LOW);

    for (i = 0; i < 16; i++)
    {
        if (i <= 7)
        {
            digitalWrite(LEDARRAY_D, _row >> 7);
            _row <<= 1;
        }
        else
        {
            digitalWrite(LEDARRAY_D, _col >> 7);
            _col <<= 1;
        }
        digitalWrite(LEDARRAY_SRCLK, HIGH);
        delay(5);
        digitalWrite(LEDARRAY_SRCLK, LOW);
    }
    digitalWrite(LEDARRAY_RCLK, HIGH);
    delay(5);
    digitalWrite(LEDARRAY_RCLK, LOW);
}