#include "helpers.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    s21_decimal zero = {{0, 0, 0, 0}};
    *result = zero;
    int error = 0;
    int sign1 = (value_1.bits[3] & 0x80000000) != 0;
    uint32_t scale1 = (value_1.bits[3] >> 16) & 0xFF;
    uint32_t scale2 = (value_2.bits[3] >> 16) & 0xFF;
    while (scale1 < scale2) {
        if (s21_multiply_by_10(&value_1)) {
            error = 1;
        }
        scale1++;
    }
    while (scale2 < scale1) {
        if (s21_multiply_by_10(&value_2)) {
            error = 1;
        }
        scale2++;
    }
    uint64_t carry = 0;
    for (int i = 0; i < 3; i++) {
        uint64_t sum = (uint64_t)value_1.bits[i] + value_2.bits[i] + carry;
        result->bits[i] = (uint32_t)(sum & 0xFFFFFFFF);
        carry = sum >> 32;
    }
    if (carry) {
        error = sign1 == 0 ? 1 : 2;
    }
    result->bits[3] = (scale1 << 16);
    if (sign1) {
        result->bits[3] |= 0x80000000;
    }
    return error;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int res = 0;
    return res;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int res = 0;
    return res;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int res = 0;
    decimal_bit3 db3;
    db3.i = value_2.bits[3];
    if (value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0 && db3.parts.power != 0) {
        res = 3;
    }
    return res;
}

int s21_is_less(s21_decimal a, s21_decimal b) {
    return s21_is_greater(b, a);
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
    return !s21_is_greater_or_equal(a, b) || s21_is_equal(a, b);
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
    int result = 0;
    bool flag = 0;
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
        for (int i = 0; i < 3 && !flag; i++) {
            if (tmp1.bits[i] > tmp2.bits[i]) {
                result = 1;
                flag = 1;
            } else if (tmp2.bits[i] > tmp1.bits[i]) {
                result = 0;
                flag = 1;
            }
        }
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
    if (isnan(src) || isinf(src) || fabs((double) src) > 7.9228162514264337593543950335e28 || fabs((double) src) < 1e-28) {
        flag = 1;
    }
    if (flag == 0) {
        int sign = src < 0 ? -1 : 1;
        src = (float) fabs((double) src);
        int scale = 0;
        while (src >= 1e7) {
            src /= 10;
            scale++;
        }
        while (src < 1e6 && scale < 28) {
            src *= 10;
            scale++;
        }
        int integer_part = (int) round((double) src);
        dst->bits[0] = integer_part;
        dst->bits[3] |= (scale << 16);
        s21_set_sign(dst, sign);
    }
    return flag;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
    int res = s21_is_valid_decimal(src);
    if (res == 1) {
        int flag = 0;
        if (src.bits[3] >> 16 != 0) {
            uint32_t scale = (src.bits[3] >> 16) & 0xFF;
            uint32_t integer_part = src.bits[0];
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
            *dst = (int) src.bits[0];
            if (src.bits[3] & (1 << 31)) {
                *dst = -*dst;
            }
        }
        res = flag;
    }
    return res;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
    int res = s21_is_valid_decimal(src);
    if (res == 1) {
        float result = 0;
        result += (float) src.bits[0];
        uint32_t scale = (src.bits[3] >> 16) & 0xFF;
        while (scale > 0) {
            result /= 10;
            scale--;
        }
        if (src.bits[3] & (1 << 31)) {
            result = -result;
        }
        *dst = result;
    }
    return res;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
    int res = s21_is_valid_decimal(value);
    if (res == 1) {
        s21_decimal ten = { .bits = {10, 0, 0, 0} };
        uint32_t sign = (value.bits[3] >> 31) & 0x1;
        s21_truncate(value, result);
        s21_decimal fractional_part;
        s21_mul(*result, ten, &fractional_part);
        s21_sub(value, fractional_part, &fractional_part);
        if (sign == 1 && !(fractional_part.bits[0] == 0 && fractional_part.bits[1] == 0 && fractional_part.bits[2] == 0)) {
            s21_decimal one = { .bits = {1, 0, 0, 0} };
            s21_sub(*result, one, result);
        }
    }
    return res;
}

int s21_round(s21_decimal value, s21_decimal *result) {
    int res = s21_is_valid_decimal(value);
    if (res == 1) {
        s21_decimal ten = { .bits = {10, 0, 0, 0} };
        uint32_t sign = (value.bits[3] >> 31) & 0x1;
        s21_truncate(value, result);
        s21_decimal fractional_part;
        s21_mul(*result, ten, &fractional_part);
        s21_sub(value, fractional_part, &fractional_part);
        s21_decimal fractional1;
        s21_from_float_to_decimal((float) 0.5, &fractional1);
        if (s21_is_greater_or_equal(fractional_part, fractional1) == 1) {
            if (sign == 0) {
                s21_decimal one = { .bits = {1, 0, 0, 0} };
                s21_add(*result, one, result);
            } else {
                s21_decimal one = { .bits = {1, 0, 0, 0} };
                s21_sub(*result, one, result);
            }
        }
    }
    return res;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
    int res = s21_is_valid_decimal(value);
    if (res == 1) {
        s21_trim_trailing_zeros(value);
        s21_decimal ten = {.bits = {10, 0, 0, 0}};
        s21_decimal_zero(result);
        uint32_t scale = (value.bits[3] >> 16) & 0xFF;
        if (scale == 0) {
            *result = value;
        } else {
            s21_decimal integer_part;
            s21_decimal temp = value;
            temp.bits[3] &= ~(0xFF << 16);
            for (int i = 0; i < scale; ++i) {
                s21_div(temp, ten, &integer_part);
                temp = integer_part;
            }
            *result = temp;
        }
    }
    return res;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
    int res = s21_is_valid_decimal(value);
    if (res == 1) {
        s21_dec_assignment(value, result);
        s21_set_sign(result, 1);
    }
    return res;
}

