#ifndef __FONTS_H
#define __FONTS_H

#include <stdint-gcc.h>

typedef struct
{
    const uint8_t *table;
    uint16_t Width;
    uint16_t Height;
} FontType;

extern FontType Font24;
extern FontType Font16;

#endif /* __FONTS_H */
