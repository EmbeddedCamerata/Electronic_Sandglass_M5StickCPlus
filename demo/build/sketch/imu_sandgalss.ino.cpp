#include <Arduino.h>
#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/imu_sandglass/imu_sandgalss.ino"
#include <M5StickCPlus.h>
#include "include/imu_sandglass.h"

#line 4 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/imu_sandglass/imu_sandgalss.ino"
void setup();
#line 9 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/imu_sandglass/imu_sandgalss.ino"
void loop();
#line 4 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/imu_sandglass/imu_sandgalss.ino"
void setup() {
    M5.begin();
    Imu_Sandglass_Setup();
}

void loop() {
    
}
