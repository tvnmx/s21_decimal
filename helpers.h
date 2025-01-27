#ifndef S21_DECIMAL_HELPERS_H
#define S21_DECIMAL_HELPERS_H

#include "s21_decimal.h"

uint32_t get_sign(s21_decimal value);
uint32_t get_scale(s21_decimal value);
int s21_multiply_by_10(s21_decimal *value);
s21_decimal s21_int128_binary_division(s21_decimal decimal1, s21_decimal decimal2, s21_decimal *ost);
s21_decimal s21_int128_get_ten_pow(int pow);
void s21_decimal_zero(s21_decimal *dst);
void s21_set_sign(s21_decimal *dst, int sign);
void s21_print_decimal(s21_decimal dec);
void s21_dec_assignment(s21_decimal value, s21_decimal *result);
void s21_abs(s21_decimal value, s21_decimal *result);
int s21_is_valid_decimal(s21_decimal value);
bool s21_are_all_bits_zero(s21_decimal value);
s21_decimal s21_trim_trailing_zeros(s21_decimal value);

#endif
