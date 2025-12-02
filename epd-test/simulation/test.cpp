#include <iostream>
#include "simulator.h"
#include "../../graphics/output/bitmaps/bitmap_logo_64x64.h"

void EPD_SetFrameMemory(const uint8_t* image_buffer, int x, int y, int image_width, int image_height);
#define EPD_WIDTH kNumPixels
#define EPD_HEIGHT kNumPixels

int main()
{
    Simulator sim;
    Simulator_Init(&sim);
    EPD_SetFrameMemory(BITMAP_LOGO_64X64.data, 14, 12, BITMAP_LOGO_64X64.width, BITMAP_LOGO_64X64.height);

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
  if (x_end % 8 != 7) {
    true_end = (x_end & 0xF8) + 7;
  }
  std::cout<<"x_start="<<int(x)<<", true_start="<<int(true_start)<<std::endl;
  std::cout<<"x_end="<<int(x_end)<<", true_end="<<int(true_end)<<std::endl;

  int endbits = true_end - x_end;
//   image_width &= 0xF8;
//   if (x + image_width >= EPD_WIDTH) {
//     x_end = EPD_WIDTH - 1;
//   } else {
//     x_end = x + image_width - 1;
//   }
//   if (y + image_height >= EPD_HEIGHT) {
//     y_end = EPD_HEIGHT - 1;
//   } else {
//     y_end = y + image_height - 1;
//   }
  EPD_SetMemoryArea(true_start, y, true_end, y_end);
  EPD_SetMemoryPointer(true_start, y);
  EPD_SendCommand(WRITE_RAM);
  /* send the image data */
  int bit_idx = 0;
  uint8_t newbyte;
  int databit_idx = 0;
  for (int j = 0; j < image_height; j++) {
    newbyte = 0;
    bit_idx = startbits;
    for (int i = 0; i < image_width; ++i) {
        // newbyte |= image_buffer[databyte_idx] & (1 << (7 - databit_idx++));
        // newbyte |= 1 <<(7-bit_idx++);
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