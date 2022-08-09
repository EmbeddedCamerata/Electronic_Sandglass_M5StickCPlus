#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/include/ledmatrix.h"
#ifndef _LEDMATRIX_H_
#define _LEDMATRIX_H_

#include "softspi.h"

#define _COLUMNS 8
#define _ROWS    8

using softspi::SoftSpi;

class LedMatrix: public SoftSpi {
    public:
    LedMatrix(uint8_t latchPin, uint8_t dataPin, uint8_t clockPin, bool auto_write=true): 
        SoftSpi(latchPin, dataPin, clockPin, auto_write) {
            this->begin();
        };

    void pixel(int x, int y, bool filled=true);
    bool pixel(int x, int y);
    void shift(int x, int y, bool rotate=false);

    uint8_t _get_line(int x) const {
        return this->_buffer[x];
    }

    void shift_right(bool rotate=false) {
        this->shift(1, 0, rotate);
    }

    void shift_left(bool rotate=false) {
        this->shift(-1, 0, rotate);
    }

    void shift_up(bool rotate=false) {
        this->shift(0, 1, rotate);
    }

    void shift_down(bool rotate=false) {
        this->shift(10, -1, rotate);
    }

    bool __getitem__(int _x, int _y) {
        return this->_get_pixel(_x, _y);
    }

    void __setitem__(int _x, int _y, bool value) {
        this->pixel(_x, _y, value);
    }
    
    private:
    bool _auto_write;
    int columns = _COLUMNS;
    int rows = _ROWS;
};

#endif