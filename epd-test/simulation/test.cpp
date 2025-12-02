#include <iostream>
#include "simulator.h"
#include "../../graphics/output/bitmaps/bitmap_battery.h"
#include "../../graphics/output/fonts/font_big.h"
#include "../../graphics/output/fonts/font_small.h"

void EPD_SetFrameMemory(const uint8_t* image_buffer, int x, int y, int image_width, int image_height);
void EPD_DrawFont(int x, int y, font_t font, int char_idx, int seq_n=0);

#define EPD_WIDTH kNumPixels
#define EPD_HEIGHT kNumPixels

int main()
{
    Simulator sim;
    Simulator_Init(&sim);
    EPD_SetFrameMemory(BITMAP_BATTERY.data, 150, 174, BITMAP_BATTERY.width, BITMAP_BATTERY.height);
    EPD_DrawFont(43, 43, FONT_BIG, 2, 0);
    EPD_DrawFont(43, 43, FONT_BIG, 1, 1);
    EPD_DrawFont(43, 43, FONT_BIG, 3, 2);
    EPD_DrawFont(43, 43, FONT_BIG, 7, 3);
    EPD_DrawFont(120, 150, FONT_SMALL, 7, 3);
    while(true) {
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
  int true_end = x_end;

  true_end = ((x_end + 1) & 0xF8) + 8;
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
    EPD_SendData(newbyte);
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