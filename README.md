# Electronic Sandglass of M5StickC Plus

## ⚙️ 依赖

1. [M5StickC Plus SDK](https://github.com/m5stack/M5StickC-Plus)，通过Github或Arduino下载库
2. Arduino或VSCode

   👉 本工程基于VSCode与Arduino开发，在此环境下，工程可直接编译通过。在Arduino IDE下，可能存在头文件路径的问题，未验证。
3. 两块LED灯板、杜邦线若干

## 📄 文件结构

├─ demo，独立的电子沙漏倒计时工程

│	├─ build，编译时的构建文件

│	│	└─ ……

│	├─ include，工程头文件

│	│	├─ bsp_timer.h，硬件定时器相关

│	│	├─ ledmatrix.h，控制LED灯板的方法

│	│	├─ matrixsand.h，矩阵沙粒的动态计算方法

│	│	├─ matrixsand.py

│	│	├─ nonblock_delay.h，非阻塞延迟函数

│	│	├─ sandglass.h，顶层电子沙漏与倒计时逻辑的方法

│	│	├─ softspi.h，软SPI底层

│	│	└─ user.h，用户逻辑相关

│	├─ src，工程源文件

│	│	├─ bsp_timer.cpp

│	│	├─ ledmatrix.cpp

│	│	├─ nonblock_delay.cpp

│	│	├─ sandglass.cpp

│	│	└─ user.cpp

│	└─ demo.ino

├─ imu_sandglass，独立的重力电子沙漏工程（仅沙漏，无倒计时部分逻辑）

│	├─ include

│	│	└─ ……

│	├─ src

│	│	└─ ……

│	└─ imu_sandglass.ino

└─ StickC_Plus_20200616.pdf，M5StickC Plus 电路原理图

## ⛓️ 硬件结构

两块LED矩阵上有两片74595，可以使用软SPI方式驱动灯板。限于M5引出的IO口有限（且下方的IO口用杜邦线引出十分不牢固），两路SPI总线(data、clock、latch)可以共用数据与Latch线，而不共用时钟线，通过面包板连接。

左下角LED灯板为沙漏⌛的上方，其沙粒从右下角流至第二块灯板的左上角。

`<img width="600" src="./img/IMG_20220812_103655.jpg" alt="硬件结构图"></br>`

具体连接关系：

| 引脚 |           含义           | 引脚编号 |
| :--: | :----------------------: | :------: |
| G26 |         数据信号         |    26    |
| G25 |        时钟信号1        |    25    |
| G33 |        时钟信号2        |    33    |
|  G0  |        Latch信号        |    0    |
| G36 | 复用IO，需要设置浮空输入 |    36    |

```c++
#define MUTEX_PIN	GPIO_NUM_36
gpio_pulldown_dis(MUTEX_PIN);
gpio_pullup_dis(MUTEX_PIN);
```

## ⚒️ 工程

工程代码讲解及效果参见：

1. [CSDN工程详解：基于M5StickC Plus的电子沙漏(LCD+软SPI+Arduino/C++)](https://blog.csdn.net/weixin_46422143/article/details/126303016)
2. B站：工程演示](https://www.bilibili.com/video/BV1ed4y1o7bK)
3. 重力电子沙漏部分未作展示

## 📚 参考

1. [M5StickC API](https://docs.m5stack.com/zh_CN/api/stickc/system_m5stickc)
2. 软SPI：[bxparks/AceSPI](https://github.com/bxparks/AceSPI)
3. LED矩阵：[adafruit/Adafruit_CircuitPython_HT16K33](https://github.com/adafruit/Adafruit_CircuitPython_HT16K33)，用C++实现
4. 矩阵沙粒随重力动态效果计算：[adafruit/Adafruit_Learning_System_Guides](https://github.com/adafruit/Adafruit_Learning_System_Guides/blob/main/CircuitPython_LED_Sand_Hourglass/matrixsand.py)，用C++实现
5. 重力电子沙漏：[LED Matrix Hourglass Created by Ruiz Brothers](https://cdn-learn.adafruit.com/downloads/pdf/led-matrix-hourglass.pdf?timestamp=1594177461)，用C++实现
