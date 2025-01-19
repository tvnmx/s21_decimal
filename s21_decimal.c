#include "s21_decimal.h"

char *s21_dec_desc_to_char(s21_decimal value) {
    char result[32] = {0};
    int desc = value.bits[3];
    int i = 32;
    while (desc > 0) {
        result[i] = (desc % 10) + '0';
        desc /= 10;
        i--;
    }
    return result;
}

void s21_dec_assignment(s21_decimal value, s21_decimal *result) {
    result->bits[0] = value.bits[0];
    result->bits[1] = value.bits[1];
    result->bits[2] = value.bits[2];
    result->bits[3] = value.bits[3];
}

void s21_abs(s21_decimal value, s21_decimal *result) {
    s21_dec_assignment(value, result);
    if (result->bits[3] % 10 == 1) {
        result->bits[3]--;
    }
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    if (value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0 && value_2.bits[3] != 0) {
        return 3;
    }
}

int s21_is_less(s21_decimal a, s21_decimal b) {

}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
    return s21_is_less(a, b) || s21_is_equal(a, b);
}

int s21_is_greater(s21_decimal a, s21_decimal b) {

}

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
    return s21_is_greater(a, b) || s21_is_equal(a, b);
}

int s21_is_equal(s21_decimal a, s21_decimal b) {
    if ((a.bits[0] == b.bits[0] && a.bits[1] == b.bits[1] && a.bits[2] == b.bits[2] && a.bits[3] == b.bits[3])
    || (a.bits[0] == 0 && b.bits[0] == 0 && a.bits[1] == 0 && b.bits[1] == 0 && a.bits[2] == 0 && b.bits[2] == 0
    && (a.bits[3] == 0 || a.bits[3] == 1) && (b.bits[3] == 0 || b.bits[3] == 1))) {
        return 1;
    } else {
        return 0;
    }
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
    return !s21_is_equal(a, b);
}

int s21_from_int_to_decimal(int sc, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);
