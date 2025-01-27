/*
  Use Adafruit-GFX-Library 1.5.6
  https://github.com/kzhioki/Adafruit-GFX-Library/archive/spresense.zip

  Use Adafruit-ILI9341 1.5.1
  https://github.com/kzhioki/Adafruit_ILI9341/archive/spresense.zip
*/
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include "define.h"
#include "util.h"
#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"

#define TA_SHIFT          8  // Default shift for MLX90640 in open air
#define COLS              32
#define ROWS              24
#define pixelsArraySize   (COLS * ROWS)

#define DRAW_PIXELS_OFFSET_X  272
#define DRAW_PIXELS_OFFSET_Y  16

bool isNeedDraw = false;

byte speed_setting = 1;  // High is 1, Low is 2

Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC_PIN, TFT_CS_PIN, TFT_RST_PIN);
paramsMLX90640 mlx90640;

float pixels[COLS * ROWS];
float reversePixels[COLS * ROWS];


#define get_pixels(x, y)   (pixels[y * COLS + x])

const byte MLX90640_address = 0x33;  // Default 7-bit unshifted address of the MLX90640

// 最大温度、最低温度、平均温度
float maxTemp = 0.0;
float minTemp = 255.0;
float avgTemp = 0.0;

// low range of the sensor (this will be blue on the screen)
float mintemp   = 24;   // For color mapping
float min_v     = 24;   // Value of current min temp
float min_cam_v = -40;  // Spec in datasheet

// high range of the sensor (this will be red on the screen)
float maxtemp   = 35;   // For color mapping
float max_v     = 35;   // Value of current max temp
float max_cam_v = 300;  // Spec in datasheet

// the colors we will be using
const uint16_t camColors[] = {
    0x480F, 0x400F, 0x400F, 0x400F, 0x4010, 0x3810, 0x3810, 0x3810, 0x3810,
    0x3010, 0x3010, 0x3010, 0x2810, 0x2810, 0x2810, 0x2810, 0x2010, 0x2010,
    0x2010, 0x1810, 0x1810, 0x1811, 0x1811, 0x1011, 0x1011, 0x1011, 0x0811,
    0x0811, 0x0811, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0031, 0x0031,
    0x0051, 0x0072, 0x0072, 0x0092, 0x00B2, 0x00B2, 0x00D2, 0x00F2, 0x00F2,
    0x0112, 0x0132, 0x0152, 0x0152, 0x0172, 0x0192, 0x0192, 0x01B2, 0x01D2,
    0x01F3, 0x01F3, 0x0213, 0x0233, 0x0253, 0x0253, 0x0273, 0x0293, 0x02B3,
    0x02D3, 0x02D3, 0x02F3, 0x0313, 0x0333, 0x0333, 0x0353, 0x0373, 0x0394,
    0x03B4, 0x03D4, 0x03D4, 0x03F4, 0x0414, 0x0434, 0x0454, 0x0474, 0x0474,
    0x0494, 0x04B4, 0x04D4, 0x04F4, 0x0514, 0x0534, 0x0534, 0x0554, 0x0554,
    0x0574, 0x0574, 0x0573, 0x0573, 0x0573, 0x0572, 0x0572, 0x0572, 0x0571,
    0x0591, 0x0591, 0x0590, 0x0590, 0x058F, 0x058F, 0x058F, 0x058E, 0x05AE,
    0x05AE, 0x05AD, 0x05AD, 0x05AD, 0x05AC, 0x05AC, 0x05AB, 0x05CB, 0x05CB,
    0x05CA, 0x05CA, 0x05CA, 0x05C9, 0x05C9, 0x05C8, 0x05E8, 0x05E8, 0x05E7,
    0x05E7, 0x05E6, 0x05E6, 0x05E6, 0x05E5, 0x05E5, 0x0604, 0x0604, 0x0604,
    0x0603, 0x0603, 0x0602, 0x0602, 0x0601, 0x0621, 0x0621, 0x0620, 0x0620,
    0x0620, 0x0620, 0x0E20, 0x0E20, 0x0E40, 0x1640, 0x1640, 0x1E40, 0x1E40,
    0x2640, 0x2640, 0x2E40, 0x2E60, 0x3660, 0x3660, 0x3E60, 0x3E60, 0x3E60,
    0x4660, 0x4660, 0x4E60, 0x4E80, 0x5680, 0x5680, 0x5E80, 0x5E80, 0x6680,
    0x6680, 0x6E80, 0x6EA0, 0x76A0, 0x76A0, 0x7EA0, 0x7EA0, 0x86A0, 0x86A0,
    0x8EA0, 0x8EC0, 0x96C0, 0x96C0, 0x9EC0, 0x9EC0, 0xA6C0, 0xAEC0, 0xAEC0,
    0xB6E0, 0xB6E0, 0xBEE0, 0xBEE0, 0xC6E0, 0xC6E0, 0xCEE0, 0xCEE0, 0xD6E0,
    0xD700, 0xDF00, 0xDEE0, 0xDEC0, 0xDEA0, 0xDE80, 0xDE80, 0xE660, 0xE640,
    0xE620, 0xE600, 0xE5E0, 0xE5C0, 0xE5A0, 0xE580, 0xE560, 0xE540, 0xE520,
    0xE500, 0xE4E0, 0xE4C0, 0xE4A0, 0xE480, 0xE460, 0xEC40, 0xEC20, 0xEC00,
    0xEBE0, 0xEBC0, 0xEBA0, 0xEB80, 0xEB60, 0xEB40, 0xEB20, 0xEB00, 0xEAE0,
    0xEAC0, 0xEAA0, 0xEA80, 0xEA60, 0xEA40, 0xF220, 0xF200, 0xF1E0, 0xF1C0,
    0xF1A0, 0xF180, 0xF160, 0xF140, 0xF100, 0xF0E0, 0xF0C0, 0xF0A0, 0xF080,
    0xF060, 0xF040, 0xF020, 0xF800,
};

void drawPixels(float *p, uint8_t rows, uint8_t cols) {
  int colorTemp;
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      float val = get_point(p, rows, cols, x, y);
      if (val >= maxtemp) {
        colorTemp = maxtemp;
      } else if (val <= mintemp) {
        colorTemp = mintemp;
      } else {
        colorTemp = val;
      }
      
      uint8_t colorIndex = map(colorTemp, mintemp, maxtemp, 0, 255);
      colorIndex = constrain(colorIndex, 0, 255);
      
      // ピクセルを描画
      tft.drawPixel(x + DRAW_PIXELS_OFFSET_X, y + DRAW_PIXELS_OFFSET_Y, camColors[colorIndex]);
    }
  }
}

void draw() {
  char temp[16];

  tft.fillRect(16, 16, 144, 48, ILI9341_WHITE);

  sprintf(temp, "max: %.2fC", maxTemp);
  if (39 <= maxTemp && maxTemp <= 40) {
    drawStringCenter(tft, temp, 16, 16, 8, 8, ILI9341_RED, 2);
  } else {
    drawStringCenter(tft, temp, 16, 16, 8, 8, ILI9341_BLACK, 2);
  }

  sprintf(temp, "min: %.2fC", minTemp);
  drawStringCenter(tft, temp, 16, 32, 8, 8, ILI9341_BLACK, 2);

  sprintf(temp, "avg: %.2fC", avgTemp);
  drawStringCenter(tft, temp, 16, 48, 8, 8, ILI9341_BLACK, 2);

  // show tmp image
//  drawPixels(pixels, ROWS, COLS);
  drawPixels(reversePixels, ROWS, COLS);
}

void setup() {
  Serial.begin(SERIAL_BAUDRATE);

  Wire.begin();
  Wire.setClock(I2C_CLOCK);

  tft.begin(SPI_CLOCK); 
  tft.setRotation(3);
  tft.fillRect(0, 0, 320, 240, ILI9341_WHITE);

  // Get device parameters - We only have to do this once
  int status;
  uint16_t eeMLX90640[832];  // 32 * 24 = 768

  status = MLX90640_DumpEE(MLX90640_address, eeMLX90640);
  if (status != 0) {
    Serial.println("Failed to load system parameters");
  }

  status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
  if (status != 0) {
    Serial.println("Parameter extraction failed");
  }

  // Setting MLX90640 device at slave address 0x33 to work with 2Hz refresh
  MLX90640_SetRefreshRate(0x33, 0x02);

  MLX90640_SetResolution(0x33, 0x03);
}

void loop() {
  uint16_t mlx90640Frame[834];
  long startTime, endTime;

  startTime = millis();

  // those fun get tmp array, 32*24, 5fps
  for (byte x = 0; x < speed_setting; x++) {
      int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
      if (status < 0) {
          Serial.print("GetFrame Error: ");
          Serial.println(status);
          delay(500); // エラー時は長めに待つ (ほぼ意味は無いが・・・)
          return;
      }

      float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
      float Ta  = MLX90640_GetTa(mlx90640Frame, &mlx90640);
      float tr  = Ta - TA_SHIFT;  // Reflected temperature based on the sensor
                                  // ambient temperature
      float emissivity = 0.95;
      MLX90640_CalculateTo(
          mlx90640Frame, &mlx90640, emissivity, tr,
          reversePixels);  // save pixels temp to array (pixels)
      int mode = MLX90640_GetCurMode(MLX90640_address);
      MLX90640_BadPixelsCorrection(mlx90640.brokenPixels, reversePixels, mode, &mlx90640);
  }

  /*
  // Reverse image (order of Integer array)
  for (int x = 0; x < pixelsArraySize; x++) {
      if (x % COLS == 0) {
          for (int j = 0 + x, k = (COLS - 1) + x; j < COLS + x; j++, k--) {
              pixels[j] = reversePixels[k];
          }
      }
  }
  */

  {
    maxTemp = 0.0;
    minTemp = 255.0;
    avgTemp = 0.0;

    // 最高温度、最低温度、平均温度の算出
    for (int i = 0; i < COLS * ROWS; i++) {
      if (maxTemp < reversePixels[i]) {
        maxTemp = reversePixels[i];
      }
      if (minTemp > reversePixels[i]) {
        minTemp = reversePixels[i];
      }
      avgTemp += reversePixels[i];
    }
    avgTemp /= COLS * ROWS;
  }

  draw();

  endTime = millis();

  {
    char temp[16];
    sprintf(temp, "time: %ld msec", endTime - startTime);
    Serial.println(temp);
  }

  delay(250);
}
