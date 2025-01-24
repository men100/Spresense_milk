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

byte speed_setting = 2;  // High is 1 , Low is 2

Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC_PIN, TFT_CS_PIN, TFT_RST_PIN);
paramsMLX90640 mlx90640;

float reversePixels[COLS * ROWS];

const byte MLX90640_address = 0x33;  // Default 7-bit unshifted address of the MLX90640

bool isOk = false;

// 最大温度、最低温度、平均温度
float maxTemp = 0.0;
float minTemp = 255.0;
float avgTemp = 0.0;

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

  // those fun get tmp array, 32*24, 5fps
  for (byte x = 0; x < speed_setting; x++) {
      int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
      if (status < 0) {
          Serial.print("GetFrame Error: ");
          Serial.println(status);
          isOk = false;
      } else {
        Serial.println("Success");
        isOk = true;
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
      MLX90640_BadPixelsCorrection(mlx90640.brokenPixels, reversePixels, mode,
                                    &mlx90640);

      if (isOk) {
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
          /*
          Serial.print(reversePixels[i]);
          Serial.print(" ");
          if ((i + 1) % 32 == 0) Serial.println();
          */
        }
        avgTemp /= COLS * ROWS;

        {
          char temp[16];

          tft.fillRect(0, 0, 320, 240, ILI9341_WHITE);

          sprintf(temp, "max: %.2fC", maxTemp);
          drawStringCenter(tft, temp, 0, 0, 8, 8, ILI9341_RED, 2);

          sprintf(temp, "min: %.2fC", minTemp);
          drawStringCenter(tft, temp, 0, 16, 8, 8, ILI9341_RED, 2);

          sprintf(temp, "avg: %.2fC", avgTemp);
          drawStringCenter(tft, temp, 0, 32, 8, 8, ILI9341_RED, 2);
        }
      }
  }

  delay(3000);
}
