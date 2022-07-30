#include <Arduino.h>
#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/demo.ino"
#include <M5StickCPlus.h>
#include "include/user.h"

#line 4 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/demo.ino"
void setup();
#line 9 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/demo.ino"
void loop();
#line 4 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/demo.ino"
void setup() {
    M5.begin();
    User_Setup();
}

void loop() {
    User_Loop();
}
