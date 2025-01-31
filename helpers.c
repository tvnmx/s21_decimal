#include "helpers.h"

void s21_shift_left(s21_decimal *value) {
    unsigned int carry = 0;
    for (int i = 0; i < 3; i++) {
        unsigned int next_carry = (value->bits[i] >> 31) & 1;
        value->bits[i] = (value->bits[i] << 1) | carry;
        carry = next_carry;
    }
}

void s21_equalize_scales(s21_decimal value_1, s21_decimal value_2, int *error) {
    uint32_t scale1 = s21_get_scale(value_1);
    uint32_t scale2 = s21_get_scale(value_2);
    while (scale1 != scale2) {
        if (scale1 < scale2) {
            if (s21_multiply_by_10(&value_1)) {
                *error = 1;
            }
            scale1++;
        } else {
            if (s21_multiply_by_10(&value_2)) {
                *error = 1;
            }
            scale2++;
        }
    }
}

uint32_t s21_mul_bit(uint32_t first_bit, uint32_t second_bit, uint32_t *carry) {
    uint64_t big_bit = first_bit * second_bit;
    big_bit += *carry;
    uint32_t res_bit = (uint32_t) big_bit;
    *carry = (uint32_t) (big_bit >> 32);
    return res_bit;
}

uint32_t s21_get_sign(s21_decimal value) {
    decimal_bit3 db3;
    db3.i = value.bits[3];
    return db3.parts.sign;
}

uint32_t s21_get_scale(s21_decimal value) {
    decimal_bit3 db3;
    db3.i = value.bits[3];
    return db3.parts.scale;
}

int s21_multiply_by_10(s21_decimal *value) {
    uint64_t carry = 0;
    int res = 0;
    for (int i = 0; i < 3; i++) {
        uint64_t ans = (uint64_t)value->bits[i] * 10 + carry;
        value->bits[i] = (uint32_t)(ans & 0xFFFFFFFF);
        carry = ans >> 32;
    }
    if (carry != 0) {
        res = 1;
    }
    return res;
}

void s21_decimal_zero(s21_decimal *dst) {
    for (int i = 0; i < 4; i++) {
        dst->bits[i] = 0;
    }
}

void s21_set_sign(s21_decimal *dst, uint32_t sign) {
    if (sign) {
        dst->bits[3] |= (1 << 31);
    } else {
        dst->bits[3] &= ~(1 << 31);
    }
}

void s21_set_scale(s21_decimal *decimal, uint32_t scale) {
    decimal_bit3 db3;
    db3.i = decimal->bits[3];
    db3.parts.scale = scale;
    decimal->bits[3] = db3.i;
}

void s21_print_decimal(s21_decimal dec) {
    for (int i = 3; i >= 0; i--) {
        for (int j = 31; j >= 0; j--) {
            printf("%d", (dec.bits[i] >> j) & 1);
            if (j % 4 == 0) printf(" ");
        }
        printf("\n");
    }
}

void s21_dec_assignment(s21_decimal value, s21_decimal *result) {
    result->bits[0] = value.bits[0];
    result->bits[1] = value.bits[1];
    result->bits[2] = value.bits[2];
    result->bits[3] = value.bits[3];
}

void s21_abs(s21_decimal value, s21_decimal *result) {
    s21_dec_assignment(value, result);
    if (result->bits[3] & (1 << 31)) {
        result->bits[3] &= ~(1 << 31);
    }
}

bool s21_is_valid_decimal(s21_decimal value) {
    bool result = true;
    decimal_bit3 db3;
    db3.i = value.bits[3];
    if (db3.parts.empty2 != 0 || db3.parts.empty1 != 0 || db3.parts.scale > 28 || db3.parts.sign > 1) {
        result = false;
    }
    return result;
}

bool s21_are_all_bits_zero(s21_decimal value) {
    bool result = false;
    if (value.bits[0] == 0 && value.bits[1] == 0
        && value.bits[2] == 0 && value.bits[3] == 0) {
        result = true;
    }
    return result;
}

s21_decimal s21_trim_trailing_zeros(s21_decimal value) {
    s21_decimal result = value;
    uint32_t scale = s21_get_scale(value);
    if (scale > 0 && s21_is_valid_decimal(value)) {
        s21_decimal ost = { .bits = {0, 0, 0, 0}};
        s21_decimal tmp = value;
        tmp.bits[3] = 0;
        s21_decimal DECIMAL_TEN = { .bits = {10, 0, 0, 0} };
        while (scale > 0 && s21_are_all_bits_zero(ost)) {
            s21_div(tmp, DECIMAL_TEN, &tmp);
            s21_truncate(tmp, &ost);
            s21_sub(tmp, ost, &ost);
            if (s21_are_all_bits_zero(ost)) {
                --scale;
                result = tmp;
            }
        }
    }
    return result;
}