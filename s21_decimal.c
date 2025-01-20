#include <math.h>
#include "s21_decimal.h"

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

bool s21_is_valid_decimal(s21_decimal value) {
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
    uint32_t sign = db3.parts.sign;
    if (power > 0 && s21_is_valid_decimal(value)) {
        s21_decimal ost = {0, 0, 0, 0};
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

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {

}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {

}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {

}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    decimal_bit3 db3;
    db3.i = value_2.bits[3];
    if (value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0 && db3.parts.power != 0) {
        return 3;
    }
}

int s21_is_less(s21_decimal a, s21_decimal b) {
    return s21_is_greater(b, a);
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
    return !s21_is_greater_or_equal(a, b) || s21_is_equal(a, b);
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
    bool flag = 0;
    int result = 0;
    s21_decimal tmp1 = s21_trim_trailing_zeros(a);
    s21_decimal tmp2 = s21_trim_trailing_zeros(b);
    decimal_bit3 db31;
    db31.i = tmp1.bits[3];
    uint32_t sign1 = db31.parts.sign;
    decimal_bit3 db32;
    db32.i = tmp2.bits[3];
    uint32_t sign2 = db32.parts.sign;
    if (sign1 < sign2) {
        result = 1;
    } else if (sign1 == sign2) {
        //TODO
    } else {
        //TODO
    }
    return result;
}

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
    return s21_is_greater(a, b) || s21_is_equal(a, b);
}

int s21_is_equal(s21_decimal a, s21_decimal b) {
    bool result = false;
    s21_decimal tmp1 = s21_trim_trailing_zeros(a);
    s21_decimal tmp2 = s21_trim_trailing_zeros(b);

    if ((tmp1.bits[0] == tmp2.bits[0]
         && tmp1.bits[1] == tmp2.bits[1]
         && tmp1.bits[2] == tmp2.bits[2]
         && tmp1.bits[3] == tmp2.bits[3]) ||
        (tmp1.bits[0] == 0 && tmp1.bits[1] == 0
         && tmp1.bits[2] == 0 && tmp2.bits[0] == 0
         && tmp2.bits[1] == 0 && tmp2.bits[2] == 0)) {
        result = true;
    }
    return result;
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
    return !s21_is_equal(a, b);
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
    s21_decimal_zero(dst);
    if (src < 0) {
        s21_set_sign(dst, -1);
        src = -src;
    }
    dst->bits[0] = src;
    return 0;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
    int flag = 0;
    s21_decimal_zero(dst);
    if (isnan(src) || isinf(src) || fabs(src) > 7.9228162514264337593543950335e28 || fabs(src) < 1e-28) {
        flag = 1;
    }
    if (flag == 0) {
        int sign = src < 0 ? -1 : 1;
        src = fabs(src);
        int scale = 0;
        while (src >= 1e7) {
            src /= 10;
            scale++;
        }
        while (src < 1e6 && scale < 28) {
            src *= 10;
            scale++;
        }
        int integer_part = (int)round(src);
        dst->bits[0] = integer_part;
        dst->bits[3] |= (scale << 16);
        s21_set_sign(dst, sign);
    }
    return flag;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
    int flag = 0;
    if (src.bits[3] >> 16 != 0) {
        int scale = (src.bits[3] >> 16) & 0xFF;
        int integer_part = src.bits[0];
        while (scale > 0 && flag == 0) {
            if (integer_part % 10 != 0) {
                flag = 1;
            }
            integer_part /= 10;
            scale--;
        }
        if (flag == 0) {
            src.bits[0] = integer_part;
        }
    }
    if (flag == 0) {
        *dst = src.bits[0];
        if (src.bits[3] & (1 << 31)) {
            *dst = -*dst;
        }
    }
    return flag;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
    float result = 0.0;
    result += src.bits[0];
    int scale = (src.bits[3] >> 16) & 0xFF;
    while (scale > 0) {
        result /= 10;
        scale--;
    }
    if (src.bits[3] & (1 << 31)) {
        result = -result;
    }
    *dst = result;
    return 0;
}

int s21_floor(s21_decimal value, s21_decimal *result);

int s21_round(s21_decimal value, s21_decimal *result);

int s21_truncate(s21_decimal value, s21_decimal *result);

int s21_negate(s21_decimal value, s21_decimal *result);

