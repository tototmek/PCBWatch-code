#include <Arduino.h>
#include "epd.h"
#include "epdpaint.h"
#include "../../graphics/output/bitmaps/bitmap_logo_64x64.h"


#define BLACK 0x00
#define WHITE 0xff

char time_string[] = {'0', '0', ':', '0', '0', '\0'};
unsigned long time_start_ms;
unsigned long time_now_s;

uint8_t error = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  if (EPD_Init() != 0) {
    Serial.print("e-Paper init failed\n");
    error = 1;
    return;
  }    

  EPD_ClearFrameMemory(BLACK);
  EPD_SetFrameMemory(BITMAP_LOGO_64X64, 64, 64, BITMAP_LOGO_64X64_WIDTH, BITMAP_LOGO_64X64_HEIGHT);
  EPD_UpdateFull();

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (i == 1 and j == 1) continue;
        delay(1000);
        EPD_SetFrameMemory(BITMAP_LOGO_64X64, 64*i, 64*j, BITMAP_LOGO_64X64_WIDTH, BITMAP_LOGO_64X64_HEIGHT);
        EPD_UpdateFull();
    }
  }


  EPD_Sleep();
}



void loop() {
    if (error) {
        Serial.println("Init error");
    }
    else {
        Serial.println("OK");
    }
    delay(1500);
}
