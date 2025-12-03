#ifndef BITMAP_COLON_H
#define BITMAP_COLON_H

#ifndef BITMAP_H
#define BITMAP_H

#include <cstdint>

typedef struct Bitmap bitmap_t;

struct Bitmap {
    uint8_t width;
    uint8_t height;
    uint16_t datalen;
    const uint8_t * data;
};

#endif // BITMAP_H

static const uint8_t BITMAP_COLON_DATA[18] = {
0x0,0x0,0x0,0x0,0x0,0x3c,0x3c,0x3c,0x3c,0x0,0x0,0x0,0x0,0x0,0x3c,0x3c,0x3c,0x3c
};

const bitmap_t BITMAP_COLON {
    .width = 8,
    .height = 18,
    .datalen = 18,
    .data = BITMAP_COLON_DATA,
};

#endif // {c_bitmap_name}_H
