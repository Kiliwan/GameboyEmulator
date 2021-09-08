#include "alu.h"

// See alu.h
flag_bit_t get_flag(flags_t flags, flag_bit_t flag)
{
    if((flag == FLAG_Z || flag == FLAG_N || flag == FLAG_H || flag == FLAG_C)) {
        return flags & flag;
    } else {
        return 0;
    }
}

// See alu.h
void set_flag(flags_t* flags, flag_bit_t flag)
{
    if(flags!=NULL) {
        switch(flag) {
        case FLAG_Z:
            bit_set(flags, 7);
            break;
        case FLAG_N:
            bit_set(flags, 6);
            break;
        case FLAG_H:
            bit_set(flags, 5);
            break;
        case FLAG_C:
            bit_set(flags, 4);
            break;
        default:
            // Do nothing
            break;
        }
    }
}

// See alu.h
int alu_add8(alu_output_t* result, uint8_t x, uint8_t y, bit_t c0)
{

    M_REQUIRE_NON_NULL(result);

    uint8_t temp = lsb4(x) + lsb4(y) + c0;
    uint8_t temp2 = msb4(x) + msb4(y) + msb4(temp);

    uint8_t res = merge4(temp, temp2);

    int carry[9];
    carry[0] = 0;

    for(int i = 1; i < 9; ++i) {
        int c = (i == 1 && c0 == 1) ? 1 : 0;
        carry[i] = bit_get(bit_get(x, i-1) + bit_get(y, i -1) + carry[i - 1] + c, 1);
    }

    result->value = res;

    update_flags(result, carry, 0);

    return ERR_NONE;
}

// See alu.h
int alu_sub8(alu_output_t* result, uint8_t x, uint8_t y, bit_t b0)
{

    M_REQUIRE_NON_NULL(result);

    uint8_t temp = lsb4(x) - lsb4(y) - b0;
    uint8_t temp2 = msb4(x) - msb4(y) + msb4(temp);

    int res = merge4(temp, temp2);

    set_N(&result->flags); // operation is substration, set N flag

    int carry[9];
    carry[0] = 0;

    for(int i = 1; i < 9; ++i) {
        int c = (i == 1 && b0 == 1) ? 1 : 0;
        carry[i] = bit_get(bit_get(x, i-1) - bit_get(y, i -1) - carry[i - 1] - c, 1);
    }

    result->value = res;
    update_flags(result, carry, temp2);

    return ERR_NONE;
}

// See alu.h
int alu_add16_low(alu_output_t* result, uint16_t x, uint16_t y)
{

    M_REQUIRE_NON_NULL(result);

    alu_add8(result, lsb8(x), lsb8(y), 0);

    int carry = (get_C(result->flags) != 0) ? 1 : 0; // extract carry
    uint16_t msb = msb8(x) + msb8(y) + carry;

    result->value = merge8(result->value, msb);

    alu_add16_update(result);

    return ERR_NONE;
}

// See alu.h
int alu_add16_high(alu_output_t* result, uint16_t x, uint16_t y)
{

    M_REQUIRE_NON_NULL(result);

    uint16_t lsb = lsb8(x) + lsb8(y);
    alu_add8(result, msb8(x), msb8(y), bit_get(msb8(lsb), 0));

    result->value = merge8(lsb, result->value);

    alu_add16_update(result);

    return ERR_NONE;

}

// See alu.h
void alu_add16_update(alu_output_t* result)
{

    int zero = (result->value == 0) ? FLAG_Z : 0; // set flag if result is 0
    result->flags = get_C(result->flags) + get_H(result->flags) + zero; // combine flags
}

// See alu.h
int alu_shift(alu_output_t* result, uint8_t x, rot_dir_t dir)
{

    M_REQUIRE_NON_NULL(result);

    if(dir == RIGHT) {
        if(bit_get(x, 0) == 1)
            set_C(&result->flags); // if carry, set C flag
        x = x >> 1; // shift right
    } else if(dir == LEFT) {
        if(bit_get(x, 7) == 1)
            set_C(&result->flags); // if carry, set C flag
        x = x << 1; // shift left
    } else {
        return ERR_BAD_PARAMETER;
    }

    if(x == 0)
        set_Z(&result->flags); // if zero, set Z flag

    result->value = x;

    return ERR_NONE;
}

// See alu.h
int alu_shiftR_A(alu_output_t* result, uint8_t x)
{

    M_REQUIRE_NON_NULL(result);

    result->flags = 0; // reset flags

    if(bit_get(x, 0) == 1) {
        set_C(&result->flags); // if carry, set C flag
    }

    if(bit_get(x, 7) == 1) {
        x = x >> 1;
        bit_set(&x, 7); // because arithmetic shift
    } else {
        x = x >> 1;
    }

    if(x == 0)
        set_Z(&result->flags); // if zero, set Z flag

    result->value = x;

    return ERR_NONE;
}

// See alu.h
int alu_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir)
{

    M_REQUIRE_NON_NULL(result);

    if (dir != LEFT && dir != RIGHT) {
        return ERR_BAD_PARAMETER;
    }

    if((dir == LEFT && bit_get(x, 7) == 1) || (dir == RIGHT && bit_get(x, 0) == 1)) {
        set_C(&result->flags); // if carry, set C flag
    }

    bit_rotate(&x, dir, 1);
    result->value = x;

    if(result->value == 0)
        set_Z(&result->flags); // if zero, set Z flag

    return ERR_NONE;

}

// See alu.h
int alu_carry_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir, flags_t flags)
{

    M_REQUIRE_NON_NULL(result);

    if (dir != LEFT && dir != RIGHT) {
        return ERR_BAD_PARAMETER;
    }

    bit_t perte = (dir==LEFT) ? bit_get(x, 7) : bit_get(x, 0); // extract lost bit

    alu_shift(result, x, dir);

    result->flags = 0; // reset flags

    int carry = (get_C(flags) == FLAG_C) ? 1 : 0; // extract carry

    if(dir == RIGHT) {
        carry = carry << 7;
    }


    result->value += carry;

    if(result->value == 0)
        set_Z(&result->flags); // if zero, set Z flag
    if(perte == 1)
        set_C(&result->flags); // if carry, set C flag

    return ERR_NONE;
}

// See alu.h
void update_flags(alu_output_t* result, int carry[], int temp)
{

    if(result->value == 0) {
        set_Z(&result->flags); // if zero, set Z flag
    }
    if(carry[4] != 0) {
        set_H(&result->flags); // if half-carry, set H flag
    }
    if(carry[8] != 0 || temp == 0x0F) {
        set_C(&result->flags); // if carry, set C flag
    }

}
