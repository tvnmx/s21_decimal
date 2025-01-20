#include "s21_decimal.h"

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

s21_decimal s21_remove_trailing_zeros(s21_decimal value) {

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
    return !s21_is_greater_or_equal(a, b);
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
    return !s21_is_greater_or_equal(a, b) || s21_is_equal(a, b);
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
    bool flag = 0;
    for (int i = 0; i < 3 && flag == 0; i++) {
        if (a.bits[i] > b.bits[i]) {
            flag = 1;

        }
    }
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

int s21_from_int_to_decimal(int sc, s21_decimal *dst);

int s21_from_float_to_decimal(float src, s21_decimal *dst);

int s21_from_decimal_to_int(s21_decimal src, int *dst);

int s21_from_decimal_to_float(s21_decimal src, float *dst);

int s21_floor(s21_decimal value, s21_decimal *result);

int s21_round(s21_decimal value, s21_decimal *result);

int s21_truncate(s21_decimal value, s21_decimal *result);

int s21_negate(s21_decimal value, s21_decimal *result);

//s21_char_decimal s21_dec_to_char(s21_decimal value) {
//    s21_char_decimal result = {'0'};
//    for (int i = 0; i < 4; i++) {
//        for (int j = 31; j >= 0; j--) {
//            result.bits[i][j] = (value.bits[i] & 1) + '0';
//            value.bits[i] >>= 1;
//        }
//    }
//    return result;
//}

//char *s21_dec_desc_to_char(s21_decimal value) {
//    char result[32] = {'0'};
//    for (int j = 31; j >= 0; j--) {
//        result[j] = (value.bits[3] & 1) + '0';
//        value.bits[3] >>= 1;
//    }
//    return result;
//}

//int main() {
//    s21_decimal num1 = {28, 32, 1, 3496834};
//    s21_decimal num2 = {28, 33, 1, 0};
//    printf("%d", s21_is_valid_decimal(num1));
//    printf("%d", s21_is_valid_decimal(num2));
//}