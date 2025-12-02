#ifndef EPD_EMULATOR_H
#define EPD_EMULATOR_H

#define WRITE_RAM 0x24

#include <cstdint>

constexpr int kNumPixels = 200;
constexpr int kNumBytes = kNumPixels / 8;

extern uint8_t epdDisplayData[kNumPixels][kNumBytes];

struct EPD {
    uint8_t memoryAreaX = 0;
    uint8_t memoryAreaY = 0;
    uint8_t memoryAreaXEnd = 0;
    uint8_t memoryAreaYEnd = 0;
    uint8_t memoryPointerX = 0;
    uint8_t memoryPointerY = 0;
};

void EPD_SetMemoryArea(uint8_t x, uint8_t y, uint8_t x_end, uint8_t y_end);
void EPD_SetMemoryPointer(uint8_t x, uint8_t y);
void EPD_SetCommand(uint8_t command);
void EPD_SendData(uint8_t data);

extern EPD epd;

#endif // EPD_EMULATOR_H