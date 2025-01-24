#include "util.h"

void drawStringCenter(Adafruit_ILI9341& tft, const char* str, int x, int y, int w, int h, int color, int scale = 1) {
  int len = strlen(str);
  tft.setTextSize(scale);
  int strWidth = len * TEXT_SIZE_BASE_WIDTH * scale;
  int strHeight = TEXT_SIZE_BASE_HEIGHT * scale;
  int sx = (w - strWidth)/2;
  if (sx < 0) {
    sx = 0;
  }
  int sy = (h - strHeight)/2;
  if (sy < 0) {
    sy = 0;
  }
  tft.setCursor(x + sx, y + sy);
  tft.setTextColor(color);
  tft.print(str);
}

void putStringOnLcd(Adafruit_ILI9341& tft, String str, int color) {
  int len = str.length();
  tft.fillRect(0, 224, 320, 240, ILI9341_BLACK);
  tft.setTextSize(1);
  int sx = 160 - len/2*12;
  if (sx < 0) sx = 0;
  tft.setCursor(sx, 225);
  tft.setTextColor(color);
  tft.println(str);
}

void drawBox(Adafruit_ILI9341& tft, uint16_t* imgBuf, int offset_x, int offset_y, int width, int height, int thickness, int color) {
  /* Draw target line */
  for (int x = offset_x; x < offset_x+width; ++x) {
    for (int n = 0; n < thickness; ++n) {
      *(imgBuf + tft.width()*(offset_y+n) + x)          = color;
      *(imgBuf + tft.width()*(offset_y+height-1-n) + x) = color;
    }
  }
  for (int y = offset_y; y < offset_y+height; ++y) {
    for (int n = 0; n < thickness; ++n) {
      *(imgBuf + tft.width()*y + offset_x+n)           = color;
      *(imgBuf + tft.width()*y + offset_x + width-1-n) = color;
    }
  } 
}
