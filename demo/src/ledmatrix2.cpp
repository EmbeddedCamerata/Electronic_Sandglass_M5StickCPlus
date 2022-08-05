#include "../include/ledmatrix2.h"

/*
    Shift pixels by x and y
    :param int x: The x coordinate of the pixel
    :param int y: The y coordinate of the pixel
    :param bool rotate: Rotate the shifted pixels to the left side (default=False)
*/
void LedMatrix2::shift(int x, int y, bool rotate=false) {
    int i, row;
    bool auto_write = this->auto_write;
    this->_auto_write = false;

    if (x > 0) {
        for (i = 0; i < x; i++) {
            for (row = 0; row < this->rows; row++) {
                
            }
        }
    }

    this->_auto_write = auto_write;
    if (_auto_write) {
        this->show();
    }
}