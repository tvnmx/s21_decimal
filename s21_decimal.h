#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    uint32_t bits[4];
} s21_decimal;

typedef union decimal_bit3 {
    uint32_t i;
    struct {
        uint32_t empty2 : 16;
        uint32_t power : 8;
        uint32_t empty1 : 7;
        uint32_t sign : 1;
    } parts;
} decimal_bit3;

#define MAX_DEC {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0};
#define MIN_DEC {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000};

s21_decimal s21_int128_binary_division(s21_decimal decimal1, s21_decimal decimal2, s21_decimal *ost);
s21_decimal s21_int128_get_ten_pow(int pow);

void s21_decimal_zero(s21_decimal *dst);
void s21_set_sign(s21_decimal *dst, int sign);
s21_decimal s21_trim_trailing_zeros(s21_decimal value);
void s21_print_decimal(s21_decimal dec);
void s21_abs(s21_decimal value, s21_decimal *result);

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_is_less(s21_decimal a, s21_decimal b);
int s21_is_less_or_equal(s21_decimal a, s21_decimal b);
int s21_is_greater(s21_decimal a, s21_decimal b);
int s21_is_greater_or_equal(s21_decimal a, s21_decimal b);
int s21_is_equal(s21_decimal a, s21_decimal b);
int s21_is_not_equal (s21_decimal a, s21_decimal b);

int s21_from_int_to_decimal (int sc, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);
#endif

