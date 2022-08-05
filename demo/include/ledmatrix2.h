#ifndef _LEDMATRIX2_H_
#define _LEDMATRIX2_H_

#include "softspi.h"

#define _COLUMNS 8
#define _ROWS    8

using softspi::SoftSpi;

class LedMatrix2: public SoftSpi {
    public:

    void shift(int x, int y, bool rotate=false);
    
    private:
    bool _auto_write;
    int columns = _COLUMNS;
    int rows = _ROWS;
};

#endif