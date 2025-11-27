#include "epd_draw.h"


void EPD_ClearMemory(uint8_t value) {
    EPD_SetMemoryArea(0, 0, EPD_WIDTH, EPD_HEIGHT);
    EPD_SetMemoryPointer(0, 0);
    EPD_SendCommand(WRITE_RAM);

    for (int i = 0; i < EPD_WIDTH*EPD_HEIGHT/8; ++i) {
        EPD_SendData(value);
    }
}

