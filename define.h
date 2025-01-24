#ifndef __MILK_DEFINE_H__
#define __MILK_DEFINE_H__

// シリアルの Baudrate
#define SERIAL_BAUDRATE  (115200)

// I2C の Clock
#define I2C_CLOCK (400000)

// SPI (TFT) の Clock
#define SPI_CLOCK (40000000)

// ディスプレイ用 CS, RESET, DC ピン
#define TFT_CS_PIN  10
#define TFT_RST_PIN 8
#define TFT_DC_PIN  9

// タッチスクリーン用 CS ピン
#define TOUCH_SCREEN_CS_PIN 7

// Adafruit_ILI9341 ライブラリのデフォルトフォントの幅、高さ (pixel)
#define TEXT_SIZE_BASE_WIDTH  6
#define TEXT_SIZE_BASE_HEIGHT 7

// ポイント座標
typedef struct Point {
  int x;
  int y;
} Point;

// Rect 情報
typedef struct Rect {
  int x;
  int y;
  int w;
  int h;
} Rect;

#endif // __MILK_DEFINE_H__