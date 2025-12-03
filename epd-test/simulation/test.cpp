#include <iostream>
#include "simulator.h"
#include "../../graphics/output/bitmaps/bitmap_battery.h"
#include "../../graphics/output/bitmaps/bitmap_colon.h"
#include "../../graphics/output/fonts/font_big.h"
#include "../../graphics/output/fonts/font_small_bold.h"

void EPD_SetFrameMemory(const uint8_t* image_buffer, int x, int y, int image_width, int image_height);
void EPD_DrawFont(int x, int y, font_t font, int char_idx, int seq_n=0);
void EPD_DrawBitmap(int x, int y, bitmap_t bitmap);

#define EPD_WIDTH kNumPixels
#define EPD_HEIGHT kNumPixels


#define TIME_Y 64
#define DATE_Y 110

uint8_t hours = 21;
uint8_t minutes = 37;
uint8_t dayOfWeek = 3;
uint8_t day = 3;
uint8_t month = 12;
uint16_t year = 2025;

void IncrementTime() {
  if (++minutes > 59) {
    minutes = 0;
    if (++hours > 23) {
      hours = 0;
    }
  }
}

void IncrementDate() {
  if (++day > 30) {
    day = 1;
    if (++month > 12) {
      month = 1;
      ++year;
    }
  }
}

void DrawTime() {
    EPD_DrawFont(16,  TIME_Y, FONT_BIG, hours/10,   0);
    EPD_DrawFont(57,  TIME_Y, FONT_BIG, hours%10,   0);
    EPD_DrawFont(106, TIME_Y, FONT_BIG, minutes/10, 0);
    EPD_DrawFont(147, TIME_Y, FONT_BIG, minutes%10, 0);
}

void DrawDate() {
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, day/10,        0);
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, day%10,        1);
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, month/10,      3);
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, month%10,      4);
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, year/1000,     6);
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, year%1000/100, 7);
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, year%100/10,   8);
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, year%10,       9);

}

int main()
{
    Simulator sim;
    Simulator_Init(&sim);
    EPD_SetFrameMemory(BITMAP_BATTERY.data, 150, 174, BITMAP_BATTERY.width, BITMAP_BATTERY.height);
    EPD_SetFrameMemory(BITMAP_BATTERY.data, 150, 174, BITMAP_BATTERY.width, BITMAP_BATTERY.height);
    EPD_DrawBitmap(150, 174, BITMAP_BATTERY);
    EPD_DrawBitmap(96, TIME_Y, BITMAP_COLON);
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, 10, 2);
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, 10, 5);
    EPD_DrawFont(120, 150, FONT_SMALL_BOLD, 7, 3);
    DrawTime();
    DrawDate();
    uint8_t i = 0;
    while(true) {
        if (++i > 10) {
          i = 0;
          IncrementTime();
          IncrementDate();
          DrawTime();
          DrawDate();
        }
        Simulator_Update(&sim);
        if (Simulator_CheckExit(&sim)) {
            break;
        }
        SDL_Delay(30);
    }

    Simulator_DeInit(&sim);
}

uint8_t GetNthBit(const uint8_t* buffer, int n) {
  return 0b1 & (buffer[n/8] >> (7 - (n % 8)));
}

void EPD_SetFrameMemory(
  const uint8_t* image_buffer,
  int x,
  int y,
  int image_width,
  int image_height
) {
  int x_end;
  int y_end;

  if (
    image_buffer == NULL ||
    x < 0 || image_width < 0 ||
    y < 0 || image_height < 0
  ) {
    return;
  }
  x_end = x + image_width - 1;
  y_end = y + image_height - 1;
  int true_start = x & 0xF8;
  int startbits = x - true_start;
  int true_end = x_end + 1;
  if (true_end % 8 != 0) {
    true_end = ((x_end + 1) & 0xF8) + 8;
  }
//   std::cout<<"x_start="<<int(x)<<", true_start="<<int(true_start)<<std::endl;
//   std::cout<<"x_end="<<int(x_end)<<", true_end="<<int(true_end)<<std::endl;

  int endbits = true_end - x_end;
  EPD_SetMemoryArea(true_start, y, true_end - 1, y_end);
  EPD_SetMemoryPointer(true_start, y);
  EPD_SendCommand(WRITE_RAM);
  int bit_idx = 0;
  uint8_t newbyte;
  int databit_idx = 0;
  for (int j = 0; j < image_height; j++) {
    newbyte = 0;
    bit_idx = startbits;
    for (int i = 0; i < image_width; ++i) {
        newbyte |= GetNthBit(image_buffer, databit_idx++) << (7-bit_idx++);
        if (bit_idx >= 8) {
            bit_idx = 0;
            EPD_SendData(newbyte);
            newbyte = 0;
        }
    }
    if (bit_idx != 0) {
      EPD_SendData(newbyte); // send the byte padded with zeros if needed
    }
  }
}



void EPD_DrawFont(int x, int y, font_t font, int char_idx, int seq_n) {
    EPD_SetFrameMemory(
            font.data + char_idx * font.charDatalen,
            x + seq_n * (font.charWidth + font.charSpacing),
            y,
            font.charWidth,
            font.charHeight
        );
}


void EPD_DrawBitmap(int x, int y, bitmap_t bitmap) {
    EPD_SetFrameMemory(bitmap.data, x, y, bitmap.width, bitmap.height);
}