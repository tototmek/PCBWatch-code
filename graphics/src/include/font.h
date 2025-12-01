#ifndef FONT_H
#define FONT_H

#include <cstdint>

typedef struct Font font_t;

struct Font {
    uint8_t nChars;
    uint8_t charWidth;
    uint8_t charHeight;
    uint8_t charSpacing;
    uint16_t charDatalen;
    const uint8_t* data;
};

#endif // FONT_H