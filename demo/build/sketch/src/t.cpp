#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/src/t.cpp"
#include <stdlib.h>
#include <stdio.h>

#define LEDMATRIX_START_LAYER 8

int main() {
    int _axis = 16 - LEDMATRIX_START_LAYER;
    int _add_to = 5 - (LEDMATRIX_START_LAYER - 7) / 2;
    int i, _total;

    for (i = 8; i >= _add_to; i--) {
        _total += i * (i*2 - 1);
        if (i <= _axis - 1) {
            _total -= 2*i * (_axis - i);
        }
    }

    printf("%d\n", _total);
    return 0;
}
