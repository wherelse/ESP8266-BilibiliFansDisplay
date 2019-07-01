/*
*   B站粉丝数获取相关代码感谢B站UP主 flyAkari 会飞的阿卡林
*   Design by wherelse
*/

#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Ticker.h>

#define LED_PIN 3
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define BRIGHTNESS 30//0-255

// Params for width and height
// 设置点阵屏的宽度和高度
// -----------Y---Width---------->
//  | <------------32------------
//  |
// Height 8---------------------->
//  | X
//  ∨ -------------------------->
const uint8_t kMatrixWidth = 32;
const uint8_t kMatrixHeight = 8;
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define LED_PIN 3
CRGB led_arry[kMatrixHeight][kMatrixWidth]; //存储灯颜色状态数组
CRGB leds[NUM_LEDS];                        //存储灯颜色状态数组
// CRGB *const leds(leds_plus_safety_pixel + 1);
// WIFI相关信息配置及初始化

//5*3 ASCII 字库
const unsigned int ascii_5_3_font[] =
    {
        0b000000000000000, // sp
        0b010010010000010, // !
        0b101101000000000, // "
        0b101111101111101, // #
        0b010111010111010, // $
        0b100001010100001, // %
        0b010001110101010, // &
        0b010010000000000, // '
        0b010100100100010, // (
        0b010001001001010, // )
        0b000010101010000, // *
        0b000010111010000, // +
        0b000000000010010, // ,
        0b000000111000000, // -
        0b000000000010000, // .
        0b000001010100000, // /
        0b111101101101111, // 0
        0b110010010010111, // 1
        0b111001111100111, // 2
        0b111001011001111, // 3
        0b101101111001001, // 4
        0b111100111001111, // 5
        0b111100111101111, // 6
        0b111001001001001, // 7
        0b111101111101111, // 8
        0b111101111001111, // 9
        0b000010000010000, // :
        0b000010000010010, // ;
        0b001010100010001, // <
        0b000111000111000, // =
        0b100010001010100, // >
        0b111001010000010, // ?
        0b111101101100111, // @
        0b010101111101101, // A
        0b110101110101111, // B
        0b111100100100111, // C
        0b110101101101110, // D
        0b111100111100111, // E
        0b111100111100100, // F
        0b111100101101111, // G
        0b101101111101101, // H
        0b111010010010111, // I
        0b111010010010110, // J
        0b100111100110101, // K
        0b100100100100111, // L
        0b101111111101101, // M
        0b111101101101101, // N
        0b010101101101010, // O
        0b111101111100100, // P
        0b111101111001001, // Q
        0b100111100100100, // R
        0b111100010001111, // S
        0b111010010010010, // T
        0b101101101101111, // U
        0b101101101101010, // V
        0b101101111111101, // W
        0b101101010101101, // X
        0b101101101010010, // Y
        0b111001010100111, // Z
        0b110100100100110, // [
        0b000100010001000, // 55
        0b011001001001011, // ]
        0b010101000000000, // ^
        0b000000000000111, // _
        0b010001000000000, // '
        0b111101111101101, // a
        0b100100111101111, // b
        0b111101100100111, // c
        0b001001111101111, // d
        0b111101111100111, // e
        0b111100110100100, // f
        0b111101111001011, // g
        0b100100111101101, // h
        0b010000010010010, // i
        0b010000010010110, // j
        0b100101110101101, // k
        0b010010010010011, // l
        0b000101111101101, // m
        0b000111101101101, // n
        0b000010101101010, // o
        0b000111101111100, // p
        0b000111101111001, // q
        0b000100110100100, // r
        0b011100010001110, // s
        0b010111010010011, // t
        0b000101101101111, // u
        0b000101101101010, // v
        0b000101101111101, // w
        0b000101010101000, // x
        0b101101111001111, // y
        0b000111010100111, // z
        0b001010110010001, // {
        0b010010010010010, //|
        0b100010011010100, // }
        0b000001111100000  //~

};

CRGB bilibili_bmp[80] =
    {
        CRGB::Black, CRGB::Black, CRGB::DeepSkyBlue, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::DeepSkyBlue, CRGB::Black, CRGB::Black,
        CRGB::Black, CRGB::Black, CRGB::Black, CRGB::DeepSkyBlue, CRGB::Black, CRGB::Black, CRGB::DeepSkyBlue, CRGB::Black, CRGB::Black, CRGB::Black,
        CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue,
        CRGB::DeepSkyBlue, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::DeepSkyBlue,
        CRGB::DeepSkyBlue, CRGB::White, CRGB::DeepSkyBlue, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::DeepSkyBlue, CRGB::White, CRGB::DeepSkyBlue,
        CRGB::DeepSkyBlue, CRGB::White, CRGB::White, CRGB::White, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::White, CRGB::White, CRGB::White, CRGB::DeepSkyBlue,
        CRGB::DeepSkyBlue, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::DeepSkyBlue,
        CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue, CRGB::DeepSkyBlue};

//---------------修改此处""内的信息--------------------
const char *ssid = "*****";     //WiFi名
const char *password = "******"; //WiFi密码
String biliuid = "4969724";        //bilibili UID
//----------------------------------------------------
//bilibili数据调取配置
DynamicJsonDocument jsonBuffer(200);
WiFiClient client;

//NTP时钟初始化，设置UTC时区为东八区
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "cn.pool.ntp.org");
// 定时存储时间的相关变量
char days = 0, hours = 0, minutes = 0, seconds = 0; //定义周几，小时，分钟，秒

//定时器初始化
Ticker TimeGet;
Ticker FansGet;
Ticker TimeCal;
Ticker PageChange;
bool TimeGet_Flag = false; //定义获取状态标志位
bool FansGet_Flag = false;
u8 PageChange_Flag = 0;

long fans = 0;
/*------------------------------------------
                函数声明
------------------------------------------*/
void TimeUpdate(void);
void FansUpdate(void);
void TimeGet_Callback(void);
void FansGet_Callback(void);
void FillScreen(void);
void ScreenClear(void);
void Screen_DrawPoint(u8 x, u8 y, CRGB color);
void Screen_DrawLine(u8 x, u8 y, u8 lens, CRGB color, u8 modes);
void Screen_ShowChar(u8 x, u8 y, u8 chr, CRGB color);
void Screen_ShowNum(u8 x, u8 y, u32 num, u8 len, CRGB color);
u32 mypow(u8 m, u8 n);
void LedBuffer_Refresh(void);
void Screen_ShowString(u8 x, u8 y, u8 *chr, CRGB color);
void Time_Cal(void);
void ShowTime(void);
void DisplayFans(void);
void ShowBMP(u8 x0, u8 x1, u8 y0, u8 y1, CRGB color[]);
/*------------------------------------------
                程序初始化
------------------------------------------*/
void setup()
{
  Serial.begin(9600); //初始化串口波特率9600
  /*****WiFi连接初始化*****/
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(2000);
    Serial.print("Wifi Connect failed");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
  timeClient.setTimeOffset(28800);
  TimeGet.attach(100, TimeGet_Callback); //定时器初始化，1秒获取一次时间
  FansGet.attach(500, FansGet_Callback); //定时器初始化，5秒获取一次粉丝数
  TimeCal.attach(1, Time_Cal);
  PageChange.attach(30, PageChange_Callback);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);
  TimeUpdate();
  FansUpdate();
}

void loop()
{
  if (TimeGet_Flag == true)
    TimeUpdate();
  if (FansGet_Flag == true)
    FansUpdate();
  
  if(PageChange_Flag == 0)
    {
      ShowTime();
    }
    else
    {
      DisplayFans();
    }
  FillScreen();
  LedBuffer_Refresh();
  //ScreenClear();
}

//LED缓冲区刷新
void LedBuffer_Refresh(void)
{
  for (u8 i = 0; i < 8; i++)
    for (u8 j = 0; j < 32; j++)
      led_arry[i][j] = CRGB::Black;
}

//画点函数
void Screen_DrawPoint(u8 x, u8 y, CRGB color)
{
  if (x >= kMatrixHeight || y >= kMatrixWidth)
    return; //超出范围了.
  led_arry[x][y] = color;
}

//mode=0,沿x轴划线，mode=1，沿y轴划线
void Screen_DrawLine(u8 x, u8 y, u8 lens, CRGB color, u8 modes)
{
  if (x > kMatrixHeight || y > kMatrixWidth)
    return; //超出范围了.
  if (modes == 0)
  {
    for (u8 i = x; i < x + lens; i++)
      Screen_DrawPoint(i, y, color);
  }
  else if (modes == 1)
  {
    for (u8 i = y; i < y + lens; i++)
      Screen_DrawPoint(x, i, color);
  }
}

//显示字符函数
void Screen_ShowChar(u8 x, u8 y, u8 chr, CRGB color)
{
  chr = chr - ' '; //得到偏移后的值
  u32 temp = ascii_5_3_font[chr];
  for (u8 i = 5; i > 0; i--)
  {
    for (u8 j = 3; j > 0; j--)
    {
      if (temp & 0x0001)
      {
        Screen_DrawPoint(x + i - 1, y + j - 1, color);
      }
      else
        Screen_DrawPoint(x + i - 1, y + j - 1, CRGB::Black);
      temp >>= 1;
    }
  }
}

//显示字符串函数
void Screen_ShowString(u8 x, u8 y, u8 *chr, CRGB color)
{
  unsigned char j = 0;
  while (chr[j] != '\0')
  {
    Screen_ShowChar(x, y, chr[j], color);
    y += 4;
    if (y > 32)
    {
      y = 0;
    }
    j++;
  }
}

//显示数字函数
void Screen_ShowNum(u8 x, u8 y, u32 num, u8 len, CRGB color)
{
  u8 temp;
  u8 enshow = 0;
  for (u8 t = 0; t < len; t++)
  {
    temp = (num / mypow(10, len - t - 1)) % 10;
    if (enshow == 0 && t < (len - 1))
    {
      if (temp == 0)
      {
        Screen_ShowChar(x, y + 4 * t, ' ', color);
        continue;
      }
      else
        enshow = 1;
    }
    Screen_ShowChar(x, y + 4 * t, temp + '0', color);
  }
}

//m^n函数
u32 mypow(u8 m, u8 n)
{
  u32 result = 1;
  while (n--)
    result *= m;
  return result;
}
/*显示缓存数组数据位置调整  

  0 1 2------->31              0 1 2------->31
  32---------->63              63<----------32
  64---------->95              64---------->95
  96--------->127   ---->      127<---------96
  128-------->159   ---->      128-------->159
  160-------->191              191<--------160
  192-------->223              192-------->223
  224-------->255              255<--------224
*/
void FillScreen(void)
{
  CRGB *p;
  p = (CRGB *)led_arry;
  char n = 0;
  for (int i = 0; i < 32; i++)
  {
    leds[i] = p[i];
  }
  for (int i = 32; i < 64; i++)
  {
    leds[i] = p[63 - n];
    n++;
  }
  for (int i = 64; i < 96; i++)
  {
    leds[i] = p[i];
  }
  n = 0;
  for (int i = 96; i < 128; i++)
  {
    leds[i] = p[127 - n];
    n++;
  }
  for (int i = 128; i < 160; i++)
  {
    leds[i] = p[i];
  }
  n = 0;
  for (int i = 160; i < 192; i++)
  {
    leds[i] = p[191 - n];
    n++;
  }
  for (int i = 192; i < 224; i++)
  {
    leds[i] = p[i];
  }
  n = 0;
  for (int i = 224; i < 256; i++)
  {
    leds[i] = p[255 - n];
    n++;
  }
  FastLED.show();
}

//清屏函数
void ScreenClear(void)
{
  for (int i = 0; i < 256; i++)
  {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

//定时器回调函数
void TimeGet_Callback(void)
{
  TimeGet_Flag = true;
}

//定时器回调函数
void FansGet_Callback(void)
{
  FansGet_Flag = true;
}

//定时器回调函数
void PageChange_Callback(void)
{
  PageChange_Flag = ~PageChange_Flag;
}

//时间更新函数，从NTP服务器获取当前时间，并赋值给本地变量
void TimeUpdate(void)
{
  TimeGet_Flag = false;
  if (WiFi.status() == WL_CONNECTED)
  {
    timeClient.update();
    days = timeClient.getDay();
    Serial.print(timeClient.getDay());
    hours = timeClient.getHours();
    minutes = timeClient.getMinutes();
    seconds = timeClient.getSeconds();
  }
}

//粉丝数更新函数，从Bilibili获取粉丝数信息
void FansUpdate(void)
{
  FansGet_Flag = false;
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin("http://api.bilibili.com/x/relation/stat?vmid=" + biliuid); //API
    auto httpCode = http.GET();
    //  Serial.println(httpCode);
    if (httpCode > 0)
    {
      String resBuff = http.getString();
      DeserializationError error = deserializeJson(jsonBuffer, resBuff);
      if (error)
      {
        Serial.println("json error");
      }
      JsonObject root = jsonBuffer.as<JsonObject>();
      long code = root["code"];
      //  Serial.println(code);
      fans = root["data"]["follower"];
      Serial.println(fans);
    }
  }
}

void Time_Cal(void)
{
  seconds++;
  if (seconds >= 60)
  {
    minutes++;
    seconds = 0;
  }
  if (minutes >= 60)
  {
    hours++;
    minutes = 0;
  }
  if (hours > 24)
  {
    days++;
    hours = 0;
  }
}

void ShowTime(void)
{
  static char hue;
  hue++;
  if (hue > 255)
    hue = 0;

  Screen_ShowNum(1, 2, hours, 2, CRGB::White);
  Screen_ShowChar(1, 9, ':', CRGB::Gray);
  if (minutes > 9)
    Screen_ShowNum(1, 12, minutes, 2, CRGB::White);
  else
  {
    Screen_ShowNum(1, 12, 0, 1, CRGB::White);
    Screen_ShowNum(1, 16, minutes, 1, CRGB::White);
  }
  Screen_ShowChar(1, 19, ':', CRGB::Gray);
  if (seconds > 9)
    Screen_ShowNum(1, 22, seconds, 2, CRGB::White);
  else
  {
    Screen_ShowNum(1, 22, 0, 1, CRGB::White);
    Screen_ShowNum(1, 26, seconds, 1, CRGB::White);
  }

  Screen_DrawLine(7, 2, 3, CRGB::Gray, 1);
  Screen_DrawLine(7, 6, 3, CRGB::Gray, 1);
  Screen_DrawLine(7, 10, 3, CRGB::Gray, 1);
  Screen_DrawLine(7, 14, 3, CRGB::Gray, 1);
  Screen_DrawLine(7, 18, 3, CRGB::Gray, 1);
  Screen_DrawLine(7, 22, 3, CRGB::Gray, 1);
  Screen_DrawLine(7, 26, 3, CRGB::Gray, 1);
  switch (days)
  {
  case 1:
    Screen_DrawLine(7, 2, 3, CRGB::Cyan, 1);
    break;

  case 2:
    Screen_DrawLine(7, 6, 3, CRGB::Cyan, 1);
    break;

  case 3:
    Screen_DrawLine(7, 10, 3, CRGB::Cyan, 1);
    break;

  case 4:
    Screen_DrawLine(7, 14, 3, CRGB::Cyan, 1);
    break;

  case 5:
    Screen_DrawLine(7, 18, 3, CRGB::Cyan, 1);
    break;

  case 6:
    Screen_DrawLine(7, 22, 3, CRGB::Cyan, 1);
    break;

  case 7:
    Screen_DrawLine(7, 26, 3, CRGB::Cyan, 1);
    break;

  default:
    break;
  }
}

void DisplayFans(void)
{
  long temp;
  u8 t = 1;
  temp = fans;
  while (temp > 10)
  {
    temp /= 10;
    t++;
  }
  ShowBMP(0,7,0,9,bilibili_bmp);
  Screen_ShowNum(2, 12, fans, t, CRGB::White);
}

void ShowBMP(u8 x0, u8 x1, u8 y0, u8 y1, CRGB color[])
{
  u8 x, y;
  u8 j = 0;
  for (x = x0; x <= x1; x++)
  {
    for (y = y0; y <= y1; y++)
    {
      led_arry[x][y] = color[j++];
    }
  }
}