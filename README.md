# ESP8266-BilibiliFansDisplay

#### B站粉丝数显示时钟

## 硬件
* NODEMCU
* 8x32 WS2812B点阵屏
* 外壳 （Design with Solidworks , export as .dwg）
## 软件
- Arduino IDE
使用Arduino IDE开发Nodemcu，驱动RGB点阵，通过wifi获取B站粉丝数数据，并定时更新。
通过NTP服务器校时的网络时钟，暂未使用实时时钟芯片。

