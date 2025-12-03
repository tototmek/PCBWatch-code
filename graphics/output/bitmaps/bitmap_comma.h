#ifndef BITMAP_COMMA_H
#define BITMAP_COMMA_H

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

static const uint8_t BITMAP_COMMA_DATA[1] = {
0xf0
};

const bitmap_t BITMAP_COMMA {
    .width = 2,
    .height = 2,
    .datalen = 1,
    .data = BITMAP_COMMA_DATA,
};

#endif // {c_bitmap_name}_H
