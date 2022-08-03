
#include <M5StickCPlus.h>

/* After M5StickC is started or reset
  the program in the setUp () function will be run, and this part will only be run once.
  在 M5StickCPlus 启动或者复位后，即会开始执行setup()函数中的程序，该部分只会执行一次。 */
void setup(){
  // Initialize the M5StickCPlus object.  初始化 M5StickCPlus 对象
  M5.begin();

  // LCD display.  LCd显示
  M5.Lcd.print("Hello World");
}

/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly
在setup()函数中的程序执行完后，会接着执行loop()函数中的程序
loop()函数是一个死循环，其中的程序会不断的重复运行 */
void loop() {
}
