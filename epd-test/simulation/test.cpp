#include <iostream>
#include "simulator.h"
#include "../../graphics/output/bitmaps/bitmap_battery.h"
#include "../../graphics/output/bitmaps/bitmap_colon.h"
#include "../../graphics/output/bitmaps/bitmap_comma.h"
#include "../../graphics/output/fonts/font_big.h"
#include "../../graphics/output/fonts/font_small_bold.h"
#include "../../graphics/output/fonts/font_small_letters.h"

void EPD_SetFrameMemory(const uint8_t* image_buffer, int x, int y, int image_width, int image_height);
void EPD_DrawFont(int x, int y, font_t font, int char_idx, int seq_n=0);
void EPD_DrawBitmap(int x, int y, bitmap_t bitmap);
void EPD_DrawText(int x, int y, font_t font, char* text, int textLen=-1);

#define EPD_WIDTH kNumPixels
#define EPD_HEIGHT kNumPixels

char poniedzalek[10] = {10, 9, 8, 5, 4, 3, 15, 17, -1};
char wtorek[10] = {14, 13, 9, 11, 4, 6, -1};
char sroda[10] = {18, 11, 9, 3, 0, -1};
char czwartek[10] = {2, 15, 14, 0, 11, 13, 4, 6, -1};
char piątek[10] = {10, 5, 16, 13, 4, 6, -1};
char sobota[10] = {12, 9, 1, 9, 13, 0, -1};
char niedziela[10] = {8, 5, 4, 3, 15, 5, 4, 7, 0, -1};
char daysOfWeekStrs[7][10] = {
  {10, 9, 8, 5, 4, 3, 15, 17, -1},
  {14, 13, 9, 11, 4, 6, -1},
  {18, 11, 9, 3, 0, -1},
  {2, 15, 14, 0, 11, 13, 4, 6, -1},
  {10, 5, 16, 13, 4, 6, -1},
  {12, 9, 1, 9, 13, 0, -1},
  {8, 5, 4, 3, 15, 5, 4, 7, 0, -1}
};

#define TIME_Y 64
#define DATE_Y 110
#define VOLTAGE_X 148
#define VOLTAGE_Y 153

uint8_t hours = 21;
uint8_t minutes = 37;
uint8_t dayOfWeek = 3;
uint8_t day = 3;
uint8_t month = 12;
uint16_t year = 2025;
uint8_t voltsTimesTen = 27;

void IncrementTime() {
  if (++minutes > 59) {
    minutes = 0;
    if (++hours > 23) {
      hours = 0;
    }
  }
}

void IncrementDate() {
  if (day % 5 == 0) {
    if (++dayOfWeek > 6) {
      dayOfWeek = 0;
    }
  }
  if (++day > 30) {
    day = 1;
    if (++month > 12) {
      month = 1;
      ++year;
    }
  }
}

void IncrementVoltage() {
  if (++voltsTimesTen > 35) {
    voltsTimesTen = 17;
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

void DrawVoltage() {
    EPD_DrawFont(VOLTAGE_X, VOLTAGE_Y, FONT_SMALL_BOLD, voltsTimesTen/10, 0);
    EPD_DrawFont(VOLTAGE_X+8, VOLTAGE_Y, FONT_SMALL_BOLD, voltsTimesTen%10, 1);
}

void DrawDayOfWeek() {
    EPD_DrawText(16, DATE_Y + FONT_SMALL_BOLD.charHeight + 10, FONT_SMALL_LETTERS, daysOfWeekStrs[dayOfWeek]);
}

int main()
{
    Simulator sim;
    Simulator_Init(&sim);
    EPD_DrawBitmap(96, TIME_Y, BITMAP_COLON);
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, 10, 2);
    EPD_DrawFont(16, DATE_Y, FONT_SMALL_BOLD, 10, 5);
    EPD_DrawBitmap(VOLTAGE_X+17, VOLTAGE_Y+FONT_SMALL_BOLD.charHeight-2, BITMAP_COMMA);
    EPD_DrawFont(VOLTAGE_X+40, VOLTAGE_Y+FONT_SMALL_BOLD.charHeight-FONT_SMALL_LETTERS.charHeight, FONT_SMALL_LETTERS, 19);
    EPD_DrawBitmap(147, 174, BITMAP_BATTERY);
    DrawTime();
    DrawDate();
    DrawVoltage();
    DrawDayOfWeek();
    uint8_t i = 0;
    while(true) {
        if (++i > 10) {
          i = 0;
          IncrementTime();
          IncrementDate();
          IncrementVoltage();
          DrawDate();
          DrawVoltage();
          DrawDayOfWeek();
          DrawTime();
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




/*
Draws a sequence of characters as one action, to avoid characters occluding each other
*/
void EPD_DrawText(int x, int y, font_t font, char* text, int textLen) {
  int x_end;
  int y_end;
  if (textLen == -1) { // Automatic string end detection, read all chars until -1
    for (textLen = 0; text[textLen] != -1; ++textLen){}
    textLen++;
  }
  int image_width = textLen * font.charWidth + (textLen - 1) * font.charSpacing;
  int image_height = font.charHeight;
  if (image_width > EPD_WIDTH) {
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
  int total_databit_idx = 0;
  for (int j = 0; j < image_height; j++) {
    newbyte = 0;
    bit_idx = startbits;
    for (int c = 0; c < textLen; ++c) {
      databit_idx = total_databit_idx;
      for (int i = 0; i < font.charWidth; ++i) {
          newbyte |= GetNthBit(font.data + text[c]*font.charDatalen, databit_idx++) << (7-bit_idx++);
          if (bit_idx >= 8) {
              bit_idx = 0;
              EPD_SendData(newbyte);
              newbyte = 0;
          }
      }
      for (int pad = 0; pad < font.charSpacing; ++pad) {
        ++bit_idx;
          if (bit_idx >= 8) {
              bit_idx = 0;
              EPD_SendData(newbyte);
              newbyte = 0;
          }

      }
    }
    if (bit_idx != 0) {
      EPD_SendData(newbyte); // send the byte padded with zeros if needed
    }
    total_databit_idx = databit_idx;
  }
}