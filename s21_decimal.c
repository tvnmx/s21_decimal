#include "helpers.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    s21_decimal zero = {0, 0, 0, 0};
    *result = zero;
    int error = 0;
    s21_equalize_scales(&value_1, &value_2, &error);

    if (s21_get_sign(value_1) == s21_get_sign(value_2)) {
        s21_add_with_equal_signs(&error, result, value_1, value_2);
    } else {
        s21_add_with_diff_signs(&error, result, value_1, value_2);
    }
    if (s21_get_sign(value_1) == 1 && s21_get_sign(value_1)  == s21_get_sign(value_2) ) {
        s21_set_sign(result, 1);
    }
    return error;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    s21_decimal zero = {0, 0, 0, 0};
    *result = zero;
    int error = 0;
    uint32_t sign1 = s21_get_sign(value_1);
    uint32_t sign2 = s21_get_sign(value_2);

    s21_equalize_scales(&value_1, &value_2, &error);

    if (sign1 == sign2) {
        s21_sub_with_equal_signs(result, value_1, value_2);
    } else {
        s21_sub_with_diff_signs(&error, result, value_1, value_2);
    }
    return error;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int res = 0;
    *result = (s21_decimal){{0, 0, 0, 0}};
    uint32_t scale1 = s21_get_scale(value_1);
    uint32_t scale2 = s21_get_scale(value_2);
    uint32_t final_scale = scale1 + scale2;

    if (final_scale > 28) {
        final_scale = 28;
    }

    s21_set_scale(result, final_scale);
    s21_set_sign(result, s21_get_sign(value_1) != s21_get_sign(value_2));
    uint32_t carry = 0;

    for (int i = 0; i <= 2; i++) {
        uint32_t local_carry = 0;
        for (int j = 0; j <= 2; j++) {
            if (i + j <= 2) {
                uint32_t tmp = s21_mul_bit(value_1.bits[i], value_2.bits[j], &local_carry);
                uint64_t sum = (uint64_t)result->bits[i + j] + tmp + carry;
                result->bits[i + j] = (uint32_t)(sum & 0xFFFFFFFF);
                carry = (uint32_t)(sum >> 32);
            }
        }

        if (i + 1 <= 2) {
            result->bits[i + 1] += carry;
            carry = 0;
        }
    }
    return res;
}


int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int error = 0;
    if ((value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0)) {
        return 3;
    }

    s21_decimal divisible = value_1;
    s21_decimal divisor = value_2;
    s21_decimal quotient = {. bits = {0, 0, 0, 0}};
    s21_decimal remainder = {. bits = {0, 0, 0, 0}};
    s21_equalize_scales(&divisible, &divisor, &error);
    uint32_t final_scale = s21_get_scale(divisible) - s21_get_scale(divisor);
    divisible.bits[3] = 0;
    divisor.bits[3] = 0;

    div_support(&remainder, divisible, divisor, &quotient);

    s21_decimal quotient2 = {. bits = {0, 0, 0, 0}};
    int cnt = 1;
    int scale = 0;
    while ((remainder.bits[0] != 0 || remainder.bits[1] != 0 || remainder.bits[2] != 0) && scale < 28) {
        cnt++;
        s21_multiply_by_10(&remainder);

        s21_decimal digit = {. bits = {0, 0, 0, 0}};

        div_support(&digit, remainder, divisor, &quotient2);
        quotient2.bits[0] |= (digit.bits[0] & 1);
        s21_multiply_by_10(&quotient);
        s21_add(quotient, quotient2, &quotient);
        scale++;
        remainder.bits[0] = digit.bits[0];
        remainder.bits[1] = digit.bits[1];
        remainder.bits[2] = digit.bits[2];
    }

    final_scale = (uint32_t) fmax(scale, final_scale);
    if (final_scale > 28) final_scale = 28;
    s21_set_scale(&quotient, final_scale);
    s21_set_sign(&quotient, ((value_1.bits[3] >> 31) ^ (value_2.bits[3] >> 31)) & 1);
    *result = quotient;
    return error;
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
    s21_equalize_scales(&a, &b, &result);
    s21_decimal tmp1 = a;
    s21_decimal tmp2 = b;
    uint32_t sign_a = s21_get_sign(tmp1);
    uint32_t sign_b = s21_get_sign(tmp2);
    if (sign_a < sign_b) {
        result = 1;
    } else if (sign_a == sign_b) {
        for (int i = 2; i >= 0 && !flag; i--) {
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
    int result = 0;
    s21_decimal tmp1 = s21_trim_trailing_zeros(a);
    s21_decimal tmp2 = s21_trim_trailing_zeros(b);

    if ((tmp1.bits[0] == tmp2.bits[0]
         && tmp1.bits[1] == tmp2.bits[1]
         && tmp1.bits[2] == tmp2.bits[2]
         && tmp1.bits[3] == tmp2.bits[3]) ||
        (tmp1.bits[0] == 0 && tmp1.bits[1] == 0
         && tmp1.bits[2] == 0 && tmp2.bits[0] == 0
         && tmp2.bits[1] == 0 && tmp2.bits[2] == 0)) {
        result = 1;
    }
    return result;
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
    return !s21_is_equal(a, b);
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
    s21_decimal_zero(dst);
    if (src < 0) {
        s21_set_sign(dst, 1);
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
        int sign = src < 0 ? 1 : 0;
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
        while ((int)src % 10 == 0) {
            src /= 10;
            scale--;
        }
        int integer_part = (int) round((double) src);
        dst->bits[0] = integer_part;
        dst->bits[3] |= (scale << 16);
        s21_set_sign(dst, sign);
    }
    return flag;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
    int res = !s21_is_valid_decimal(src);
    if (res == 0) {
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
            if (s21_get_sign(src)) {
                *dst = -*dst;
            }
        }
        res = flag;
    }
    return res;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
    int res = !s21_is_valid_decimal(src);
    if (res == 0) {
        float result = 0;
        result += (float) src.bits[0];
        uint32_t scale = (src.bits[3] >> 16) & 0xFF;
        while (scale > 0) {
            result /= 10;
            scale--;
        }
        if (s21_get_sign(src)) {
            result = -result;
        }
        *dst = result;
    }
    return res;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
    int res = !s21_is_valid_decimal(value);
    uint32_t sign = s21_get_sign(value);
    if (res == 0) {
        s21_decimal_zero(result);
        uint32_t scale = s21_get_scale(value);
        if (scale == 0) {
            *result = value;
        } else {
            s21_decimal temp = value;
            temp.bits[3] = 0;
            s21_decimal integer_part;
            s21_div_by_10(scale, temp, &integer_part);
            temp.bits[0] = integer_part.bits[0];
            temp.bits[1] = integer_part.bits[1];
            temp.bits[2] = integer_part.bits[2];
            *result = temp;
            s21_set_sign(result, sign);
        }
    }
    return res;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
    int res = !(s21_is_valid_decimal(value));
    if (res == 0) {
        s21_dec_assignment(value, result);
        s21_set_sign(result, !(s21_get_sign(value)));
    }
    return res;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
    int res = !s21_is_valid_decimal(value);
    if (res == 0) {
        uint32_t sign = s21_get_sign(value);
        s21_decimal fr_part;
        s21_ostatok(value, &fr_part);
        s21_truncate(value, result);
        if (sign == 1 && !(fr_part.bits[0] == 0 && fr_part.bits[1] == 0 && fr_part.bits[2] == 0) ) {
            s21_decimal one = { .bits = {1, 0, 0, 0} };
            s21_sub(*result, one, result);
        }
    }
    return res;
}

int s21_round(s21_decimal value, s21_decimal *result) {
    int res = !s21_is_valid_decimal(value);
    if (res == 0) {
        uint32_t sign = s21_get_sign(value);
        s21_decimal fractional_part;
        s21_ostatok(value, &fractional_part);
        s21_set_scale(&fractional_part, s21_get_scale(value));
        s21_truncate(value, result);
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

//int main() {
//    s21_decimal a = {{12345678, 0, 0, 0}};  // 1.2345678
//    s21_decimal b = {{9876543, 0, 0, 0}};   // 0.9876543
//
//    s21_set_scale(&a, 7);
//    s21_set_scale(&b, 7);
//
//    s21_decimal result;
//    int res_code = s21_mul(a, b, &result);
//
//    s21_decimal expected = {{2793065010, 28389, 0, 0}};
//    s21_set_scale(&expected, 14);
//    s21_print_decimal(result);
//    s21_print_decimal(expected);
//    printf("%d\n", res_code); // 0
//
//    return 0;
//}