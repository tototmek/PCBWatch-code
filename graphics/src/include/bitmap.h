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