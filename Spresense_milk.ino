// https://github.com/m5stack/M5StickC/tree/master/examples/Hat/MLX90640
#include <Wire.h>

#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"

#define TA_SHIFT          8  // Default shift for MLX90640 in open air
#define COLS              32
#define ROWS              24

byte speed_setting = 2;  // High is 1 , Low is 2

paramsMLX90640 mlx90640;

float reversePixels[COLS * ROWS];

const byte MLX90640_address = 0x33;  // Default 7-bit unshifted address of the MLX90640

bool isOk = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); // 400kHzに設定

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
        for (int i = 0; i < COLS * ROWS; i++) {
          Serial.print(reversePixels[i]);
          Serial.print(" ");
          if ((i + 1) % 32 == 0) Serial.println();
        }
      }
  }

  delay(3000);
}
