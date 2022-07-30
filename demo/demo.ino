#include <M5StickCPlus.h>
#include "include/user.h"

void setup() {
    M5.begin();
    User_Setup();
}

void loop() {
    User_Loop();
}