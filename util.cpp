#include "util.h"

#define BABY_GRA_OFFSET_X 82
#define BABY_GRA_OFFSET_Y 82
#define BABY_GRA_WIDTH    156
#define BABY_GRA_HEIGHT   155

// 赤ちゃん画像バッファ
uint16_t babyGraBuffer[BABY_GRA_WIDTH * BABY_GRA_HEIGHT];

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

float get_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y) {
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= cols) x = cols - 1;
    if (y >= rows) y = rows - 1;
    return p[y * cols + x];
}

void drawBabyGraphic(Adafruit_ILI9341& tft, SDClass &sd, bool isBaby) {
  char path[32];
  File myFile;

  if (isBaby) {
    sprintf(path, "GRA/baby.raw");
  } else {
    sprintf(path, "GRA/no_baby.raw");    
  }
  myFile = sd.open(path, FILE_READ);

  // Verify file open
  if (!myFile) {
    printf("File open error(%s)\n", path);
    exit(1);
  }

  // ファイルのサイズを取得
  int fileSize = myFile.size();

  // ファイルサイズとバッファのサイズが一致しなかったらエラー
  if (fileSize != sizeof(babyGraBuffer)) {
    printf("fileSize doesn't match buffer size!(fileSize=%d)\n", fileSize);
    exit(1);
  }
    
  int bytesRead = myFile.read(babyGraBuffer, fileSize);
  // 読み込んだサイズとファイルサイズが一致しなかったらエラー
  if (bytesRead != fileSize) {
    printf("bytesRead doesn't match fileSizse!(bytesRead=%d)\n", bytesRead);
    exit(1);
  }
  myFile.close();

  tft.drawRGBBitmap(BABY_GRA_OFFSET_X, BABY_GRA_OFFSET_Y, babyGraBuffer, BABY_GRA_WIDTH, BABY_GRA_HEIGHT);
}