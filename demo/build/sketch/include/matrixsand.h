#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/include/matrixsand.h"
#ifndef _MATRIXSAND_H_
#define _MATRIXSAND_H_

#include <vector>
#include <tuple>

namespace matrix_sand {

class MatrxiSand {
    public:
    MatrxiSand(int width, int height, bool enable=false): _width(width), _height(height), upside_down_enable(enable) {
        this->_grains.assign(width * height, false);
    }

    bool __getitem__(int value) {
        return this->_grains[value];
    }

    bool __getitem__(int _x, int _y) {
        return this->_grains[_x + this->_width * _y];
    }

    void __setitem__(int value, bool key) {
        this->_grains[value] = key;
    }

    void __setitem__(int _x, int _y, bool key) {
        this->_grains[_x + this->_width * _y] = key;
    }
    
    void clear(void) {
        this->_grains.assign(this->_width * this->_height, false);
    }

    std::tuple<int, int> _side_count(bool upside_down=false) {
        int left = 0, right = 0;
        int x, y;
        for (x = 0; x < this->_width; x++) {
            for (y = 0; y < this->_height; y++) {
                if (x != y and this->__getitem__(x, y)) {
                    if (x > y) {
                        right += 1;
                    }
                    else {
                        left += 1;
                    }
                }
            }
        }
        if (upside_down) {
            return std::tuple<int, int>(right ,left);
        }
        else {
            return std::tuple<int, int>(left, right);
        }
    }

    bool iterate(float ax, float ay, float az) {
        int ix = 0, iy = 0;
        int x, y, newx, newy;
        int left, right;
        float ratio;
        std::vector<bool> new_grains;
        bool updated, moved;

        if (abs(az) > abs(ax) and abs(az) > abs(ay)) {
            return false;
        }
        
        if (abs(ax) > 0.01) {
            ratio = abs(ay / ax);
            if (ratio < 2.414) {
                ix = (ax > 0) ? 1 : -1;
            }
            if (ratio > 0.414) {
                iy = (ay > 0) ? 1 : -1;
            }
        }
        else {
            iy = (ay > 0) ? 1 : -1;
        }

        new_grains = this->_grains;
        updated = false;
        for (x = 0; x < this->_width; x++) {
            for (y = 0; y < this->_height; y++) {
                if (this->__getitem__(x, y)) {
                    moved = false;
                    newx = x + ix;
                    newy = y + iy;

                    newx = max(min(this->_width-1, newx), 0);
                    newy = max(min(this->_height-1, newy), 0);
                    
                    if (x != newx or y != newy) {
                        moved = true;
                        if (new_grains[newx + this->_width * newy]) {
                            if (not new_grains[x + this->_width * newy] and \
                                not new_grains[newx + this->_width * y]) {
                                std::tuple<int, int>(left, right) = this->_side_count(this->upside_down_enable and ax < 0 and ay < 0);
                                if (left >= right) {
                                    newy = y;
                                }
                                else if (right > left) {
                                    newx = x;
                                }
                            }
                            else if (not new_grains[x + this->_width * newy]) {
                                newx = x;
                            }
                            else if (not new_grains[newx + this->_width * y]) {
                                newy = y;
                            }
                            else {
                                moved = false;
                            }
                        }
                    }
                    if (moved) {
                        new_grains[x + this->_width * y] = false;
                        new_grains[newx + this-> _width * newy] = true;
                        updated = true;
                    }
                }
            }
        }
        if (updated) {
            this->_grains = new_grains;
        }

        return updated;
    }

    private:
    int _width;
    int _height;
    bool upside_down_enable;
    std::vector<bool> _grains;
};

}

#endif