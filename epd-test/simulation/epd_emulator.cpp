#include "epd_emulator.h"

uint8_t epdDisplayData[kNumPixels][kNumBytes] = {0};

EPD epd;

void EPD_SetMemoryArea(uint8_t x, uint8_t y, uint8_t x_end, uint8_t y_end) {
    epd.memoryAreaX = x >> 3;
    epd.memoryAreaY = y;
    epd.memoryAreaXEnd = x_end >> 3;
    epd.memoryAreaYEnd = y_end;
}

void EPD_SetMemoryPointer(uint8_t x, uint8_t y) {
    epd.memoryPointerX = x >> 3;
    epd.memoryPointerY = y;
}

void EPD_SendCommand(uint8_t command) {}

void EPD_SendData(uint8_t data) {
    epdDisplayData[epd.memoryPointerY][epd.memoryPointerX++] = data;
    if (epd.memoryPointerX > epd.memoryAreaXEnd) {
        epd.memoryPointerX = epd.memoryAreaX;
        ++epd.memoryPointerY;
    }
}