#include "helpers.h"


int div_support(s21_decimal *remainder, s21_decimal divisible, s21_decimal divisor, s21_decimal *quotient) {
    for (int i = 0; i < 96; i++) {
        s21_shift_left(remainder);
        (*remainder).bits[0] |= (divisible.bits[2] >> 31) & 1;
        s21_shift_left(&divisible);

        if (s21_is_greater_or_equal(*remainder, divisor)) {
            s21_sub(*remainder, divisor, remainder);
            (*quotient).bits[0] |= 1;
            (*quotient).bits[1] |= 1;
            (*quotient).bits[2] |= 1;
        }
        if (i != 95) s21_shift_left(&(*quotient));
    }
    return 0;
}

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
    uint64_t big_bit = (uint64_t) first_bit * (uint64_t) second_bit;
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

        if (ans > 0xFFFFFFFF) {
            carry = ans >> 32;
            value->bits[i] = (uint32_t)(ans & 0xFFFFFFFF);
        } else {
            carry = 0;
            value->bits[i] = (uint32_t)ans;
        }

    }
    if (carry != 0) {

        res = 1;
    }

    return res;
}

void s21_normalize(s21_decimal *a, s21_decimal *b) {
    int scale_a = s21_get_scale(*a);
    int scale_b = s21_get_scale(*b);

    while (scale_a < scale_b) {
        s21_multiply_by_10(a);
        scale_a++;
    }
    while (scale_b < scale_a) {
        s21_multiply_by_10(b);
        scale_b++;
    }
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

void s21_div_by_10(int power, s21_decimal value, s21_decimal *result) {
    for (int i = 0; i < power; i++) {
        int num = value.bits[0] % 10;
        s21_decimal acc;
        s21_from_int_to_decimal(num, &acc);
        if (s21_sub(value, acc, result) != 0) break;
        s21_decimal ten = { .bits = {10, 0, 0, 0} };
        if (s21_div(*result, ten, result) != 0) break;
    }
}