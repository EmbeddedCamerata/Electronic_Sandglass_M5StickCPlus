#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/src/ledmatrix2.cpp"
#include "../include/ledmatrix2.h"

void LedMatrix2::pixel(int x, int y, bool filled) {
    if (x < 0 or x > 7) {
        return;
    }

    if (y < 0 or y > 7) {
        return;
    }
    this->_pixel(x, y, filled);
}

bool LedMatrix2::pixel(int x, int y) {
    assert(x >= 0 and x <= 7);
    assert(y >= 0 and y <= 7);

    return this->_get_pixel(x, y);
}

/*
    Shift pixels by x and y
    :param int x: The x coordinate of the pixel
    :param int y: The y coordinate of the pixel
    :param bool rotate: Rotate the shifted pixels to the left side (default=False)
*/
void LedMatrix2::shift(int x, int y, bool rotate) {
    int i, row, col;
    bool auto_write = this->auto_write;
    bool last_pixel;
    this->_auto_write = false;

    if (x > 0) {
        for (i = 0; i < x; i++) {
            for (row = 0; row < this->rows; row++) {
                last_pixel = (rotate) ? this->__getitem__(this->columns - 1, row) : 0;
                for (col = this->columns - 1; col > 0; col--) {
                    this->__setitem__(col, row, this->__getitem__(col - 1, row));
                }
                this->__setitem__(0, row, last_pixel);
            }
        }
    }
    else if (x < 0) {
        for (i = 0; i < -x; i++) {
            for (row = 0; row < this->rows; row++) {
                last_pixel = (rotate) ? this->__getitem__(0, row) : 0;
                for (col = 0; col < this->columns - 1; col++) {
                    this->__setitem__(col, row, this->__getitem__(col + 1, row));
                }
                this->__setitem__(this->columns - 1, row, last_pixel);
            }
        }
    }

    if (y > 0) {
        for (i = 0; i < y; i++) {
            for (col = 0; col < this->columns; col++) {
                last_pixel = (rotate) ? this->__getitem__(col, this->rows - 1) : 0;
                for (row = this->rows - 1; row > 0; row--) {
                    this->__setitem__(col, row, this->__getitem__(col, row - 1));
                }
                this->__setitem__(col, 0, last_pixel);
            }
        }
    }
    else if (y < 0) {
        for (i = 0; i < -y; i++) {
            for (col = 0; col < this->columns; col++) {
                last_pixel = (rotate) ? this->__getitem__(col, 0) : 0;
                for (row = 0; row < this->rows - 1; row++) {
                    this->__setitem__(col, row, this->__getitem__(col, row + 1));
                }
                this->__setitem__(col, this->rows - 1, last_pixel);
            }
        }
    }

    this->_auto_write = auto_write;
    if (_auto_write) {
        this->show();
    }
}