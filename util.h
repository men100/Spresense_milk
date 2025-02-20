#ifndef __MILK_UTIL_H__
#define __MILK_UTIL_H__

#include <SDHCI.h>
#include <Adafruit_ILI9341.h>
#include "define.h"

// 液晶ディスプレイの指定の場所に文字列を表示する
void drawStringCenter(Adafruit_ILI9341& tft, const char* str, int x, int y, int w, int h, int color, int scale = 1);

// 液晶ディスプレイの下部に文字列を表示する
void putStringOnLcd(Adafruit_ILI9341& tft, String str, int color);

// 液晶ディスプレイに LINE_THICKNESS の太さの四角形を描画する
void drawBox(Adafruit_ILI9341& tft, uint16_t* imgBuf, int offset_x, int offset_y, int width, int height, int thickness, int color);

// 指定した行と列から x と y の示す p を返す
float get_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y);

// 赤ちゃん画像を描画。isBaby == false だと、ただの丸
void drawBabyGraphic(Adafruit_ILI9341& tft, SDClass &sd, bool isBaby);

#endif // __MILK_UTIL_H__