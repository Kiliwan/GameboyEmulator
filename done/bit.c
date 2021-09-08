#include "bit.h"

// See bit.h
uint8_t lsb4(uint8_t value)
{
    return value & 0xF;
}

// See bit.h
uint8_t lsb8(uint16_t value)
{
    return value & 0xFF;
}

// See bit.h
uint8_t msb4(uint8_t value)
{
    return value >> 4;
}

// See bit.h
uint8_t msb8(uint16_t value)
{
    return (value & 0xFF00) >> 8;
}

// See bit.h
uint8_t merge4(uint8_t v1, uint8_t v2)
{
    return (v2 & 0xF) << 4 | (v1 & 0xF);
}

// See bit.h
uint16_t merge8(uint8_t v1, uint8_t v2)
{
    return v2 << 8 | v1;
}

// See bit.h
bit_t bit_get(uint8_t value, int index)
{
    return (value >> CLAMP07(index)) & 1;
}

// See bit.h
void bit_set(uint8_t* value, int index)
{
    if(value!=NULL) {
        *value |= (1 << CLAMP07(index));
    }
}

// See bit.h
void bit_unset(uint8_t* value, int index)
{
    if(value!=NULL) {
        *value &= ~(1 << CLAMP07(index));
    }
}

// See bit.h
void bit_rotate(uint8_t* value, rot_dir_t dir, int d)
{
    if(value!=NULL) {
        int index = CLAMP07(d);
        uint8_t keep = 0; // bits to keep

        switch(dir) {
        case LEFT:
            keep = *value >> (8-index);
            *value = (*value << index) | keep;
            break;
        case RIGHT:
            keep = *value << (8-index);
            *value = (*value >> index) | keep;
            break;
        default:
            // Do nothing
            break;
        }
    }
}

// See bit.h
void bit_edit(uint8_t* value, int index, uint8_t v)
{
    if(v==0) {
        bit_unset(value, index);
    } else {
        bit_set(value, index);
    }
}
