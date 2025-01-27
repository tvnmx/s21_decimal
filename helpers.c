#include "helpers.h"


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

void s21_set_sign(s21_decimal *dst, int sign) {
    if (sign < 0) {
        dst->bits[3] |= (1 << 31);
    }
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

int s21_is_valid_decimal(s21_decimal value) {
    bool result = true;
    decimal_bit3 db3;
    db3.i = value.bits[3];
    if (db3.parts.empty2 != 0 || db3.parts.empty1 != 0 || db3.parts.power > 28 || db3.parts.sign > 1) {
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
    decimal_bit3 db3;
    db3.i = value.bits[3];
    uint32_t power = db3.parts.power;
    if (power > 0 && s21_is_valid_decimal(value)) {
        s21_decimal ost = { .bits = {0, 0, 0, 0}};
        s21_decimal tmp = value;
        tmp.bits[3] = 0;

        while (power > 0 && s21_are_all_bits_zero(ost)) {
            tmp = s21_int128_binary_division(tmp, s21_int128_get_ten_pow(1), &ost);
            if (s21_are_all_bits_zero(ost)) {
                --power;
                result = tmp;
            }
        }
    }
    return result;
}