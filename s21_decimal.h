#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int bits[4];
} s21_decimal;

#define MAX_DEC = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0};
#define MIN_DEC = {0x1, 0x0, 0x0, 0x1C0000};

char *s21_dec_desc_to_char(s21_decimal value);
void s21_dec_assignment(s21_decimal value, s21_decimal *result);
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
