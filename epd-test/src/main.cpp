#include <Arduino.h>
#include "epd.h"
#include "epdpaint.h"
#include "../../graphics/output/bitmaps/bitmap_logo_64x64.h"


#define BLACK 0
#define WHITE 1

unsigned char* frame_buffer = (unsigned char*)malloc(EPD_WIDTH * EPD_HEIGHT / 8);
char time_string[] = {'0', '0', ':', '0', '0', '\0'};
unsigned long time_start_ms;
unsigned long time_now_s;


EPD epd;

uint8_t error = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  if (EPD_Init(&epd, lut_full_update) != 0) {
    Serial.print("e-Paper init failed\n");
    error = 1;
    return;
  }    
  Paint paint;
  Paint_Init(&paint, frame_buffer, epd.width, epd.height);
  Paint_Clear(&paint, BLACK);
  
  /* Display the frame_buffer */
  EPD_SetFrameMemory(&epd, frame_buffer, 0, 0, 200, 200);
  EPD_UpdateDisplay(&epd);
  EPD_SetFrameMemory(&epd, BITMAP_BITMAP_LOGO_64X64, 64, 64, BITMAP_BITMAP_LOGO_64X64_WIDTH, BITMAP_BITMAP_LOGO_64X64_HEIGHT);
  EPD_UpdateDisplay(&epd);

  EPD_Sleep(&epd);
}



void loop() {
    if (error) {
        Serial.println("Init error");
    }
    else {
        Serial.println("OK");
    }
    delay(1000);
}
