#include <check.h>
#include <stdlib.h>
#include "s21_decimal.h"
#include "helpers.h"
#include "helpers.c"
#include "s21_decimal.c"

START_TEST(test_s21_add_basic) {
        s21_decimal a = {{3333333330, 0, 0, 0}};
        s21_decimal b = {{3, 0, 0, 0}};
        s21_decimal result = {{0, 0, 0, 0}};

        int res = s21_add(a, b, &result);

        ck_assert_int_eq(res, 0);
        ck_assert_int_eq(result.bits[0], 3333333333);
}
END_TEST

START_TEST(test_s21_add_positive) {
    s21_decimal a = {{1, 0, 0, 0}};
    s21_decimal b = {{2, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_add(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 3);
}
END_TEST

START_TEST(test_s21_add_negative) {
    s21_decimal a = {{5, 0, 0, 1 << 31}};
    s21_decimal b = {{10, 0, 0, 1 << 31}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_add(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 15);
    ck_assert_int_eq(result.bits[3] >> 31, 1);
}
END_TEST

START_TEST(test_s21_add_opposite_numbers) {
    s21_decimal a = {{123456789, 0, 0, 0}};
    s21_decimal b = {{123456789, 0, 0, 0x80000000}};
    s21_decimal result = {{1, 1, 1, 1}};

    int res = s21_add(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 0);
    ck_assert_int_eq(result.bits[1], 0);
    ck_assert_int_eq(result.bits[2], 0);
    ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_add_with_zero) {
    s21_decimal a = {{42, 0, 0, 0}};
    s21_decimal b = {{0, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_add(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 42);
}
END_TEST

START_TEST(test_s21_add_negative_overflow) {
    s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
    s21_decimal b = {{1, 0, 0, 0x80000000}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_add(a, b, &result);

    ck_assert_int_eq(res, 2);
}
END_TEST

START_TEST(test_s21_add_complex_case) {
    s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}}; // -максимальное число
    s21_decimal b = {{1, 0, 0, 0x80000000}}; // -1
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_add(a, b, &result);

    ck_assert_int_eq(res, 2); // Ожидаем ошибку переполнения
}
END_TEST

// SUB

START_TEST(test_s21_sub_basic) {
    s21_decimal a = {{5, 0, 0, 0}};
    s21_decimal b = {{3, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_sub(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 2);
}
END_TEST

START_TEST(test_s21_sub_underflow) {
    s21_decimal a = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 1<<31}};
    s21_decimal b = {{1, 0, 0, 1}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_sub(a, b, &result);

    ck_assert_int_eq(res, 2);
}
END_TEST

START_TEST(test_s21_sub_negative_result) {
    s21_decimal a = {{3, 0, 0, 0}};
    s21_decimal b = {{5, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_sub(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 2);
    ck_assert_int_eq(result.bits[3] >> 31, 1);
}
END_TEST

//

START_TEST(test_s21_sub_large_numbers) {
    s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    s21_decimal b = {{1, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_sub(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 0xFFFFFFFE);
    ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
    ck_assert_int_eq(result.bits[2], 0xFFFFFFFF);
}
END_TEST

START_TEST(test_s21_sub_same_numbers) {
    s21_decimal a = {{123456789, 0, 0, 0}};
    s21_decimal b = {{123456789, 0, 0, 0}};
    s21_decimal result = {{1, 1, 1, 1}};

    int res = s21_sub(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 0);
    ck_assert_int_eq(result.bits[1], 0);
    ck_assert_int_eq(result.bits[2], 0);
    ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_sub_negative_zero) {
    s21_decimal a = {{5, 0, 0, 0}};
    s21_decimal b = {{5, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0x80000000}};

    int res = s21_sub(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 0);
    ck_assert_int_eq(result.bits[1], 0);
    ck_assert_int_eq(result.bits[2], 0);
    ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_sub_overflow_negative) {
    s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
    s21_decimal b = {{1, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_sub(a, b, &result);

    ck_assert_int_eq(res, 2);
}
END_TEST

START_TEST(test_s21_sub_carry_propagation) {
    s21_decimal a = {{0, 0, 1, 0}};
    s21_decimal b = {{1, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_sub(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);
    ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
    ck_assert_int_eq(result.bits[2], 0);
}
END_TEST

START_TEST(test_s21_mul_bit_no_carry) {
        uint32_t carry = 0;
        uint32_t result = s21_mul_bit(2, 3, &carry);
        ck_assert_int_eq(result, 6);
        ck_assert_int_eq(carry, 0);
}
END_TEST

START_TEST(test_s21_mul_bit_with_carry) {
    uint32_t carry = 1;
    uint32_t result = s21_mul_bit(0xFFFFFFFF, 2, &carry);
    ck_assert_int_eq(result, 0xFFFFFFFF);
    ck_assert_int_eq(carry, 1);
}
END_TEST

START_TEST(test_s21_mul_bit_overflow) {
    uint32_t carry = 0;
    uint32_t result = s21_mul_bit(0xFFFFFFFF, 0xFFFFFFFF, &carry);
    ck_assert_int_eq(result, 1);
    ck_assert_int_eq(carry, 0xFFFFFFFE);
}
END_TEST

START_TEST(test_s21_mul_bit_large_numbers) {
    uint32_t carry = 0;
    uint32_t result = s21_mul_bit(100000, 100000, &carry);
    ck_assert_int_eq(result, 1410065408);
    ck_assert_int_eq(carry, 2);
}
END_TEST


START_TEST(test_s21_equalize_scales_same_scale) {
    s21_decimal value_1 = {{123, 0, 0, (2 << 16)}};
    s21_decimal value_2 = {{456, 0, 0, (2 << 16)}};
    int error = 0;

    s21_equalize_scales(&value_1, &value_2, &error);

    ck_assert_int_eq(s21_get_scale(value_1), 2);
    ck_assert_int_eq(s21_get_scale(value_2), 2);
    ck_assert_int_eq(error, 0);
}
END_TEST

START_TEST(test_s21_equalize_scales_increase_scale_1) {
    s21_decimal value_1 = {{123, 0, 0, (2 << 16)}};
    s21_decimal value_2 = {{456, 0, 0, (3 << 16)}};
    int error = 0;

    s21_equalize_scales(&value_1, &value_2, &error);

    ck_assert_int_eq(s21_get_scale(value_1), 3);
    ck_assert_int_eq(s21_get_scale(value_2), 3);
    ck_assert_int_eq(error, 0);
}
END_TEST

START_TEST(test_s21_equalize_scales_increase_scale_2) {
    s21_decimal value_1 = {{123, 0, 0, (3 << 16)}};
    s21_decimal value_2 = {{456, 0, 0, (2 << 16)}};
    int error = 0;

    s21_equalize_scales(&value_1, &value_2, &error);

    ck_assert_int_eq(s21_get_scale(value_1), 3);
    ck_assert_int_eq(s21_get_scale(value_2), 3);
    ck_assert_int_eq(error, 0);
}
END_TEST

START_TEST(test_s21_equalize_scales_multiply_overflow) {
    s21_decimal value_1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, (2 << 16)}};
    s21_decimal value_2 = {{456, 0, 0, (3 << 16)}};
    int error = 0;

    s21_equalize_scales(&value_1, &value_2, &error);

    ck_assert_int_eq(error, 1);
}
END_TEST

START_TEST(test_s21_shift_left_basic) {
    s21_decimal value = {{1, 0, 0, 0}};
    s21_shift_left(&value);
    ck_assert_int_eq(value.bits[0], 2);
    ck_assert_int_eq(value.bits[1], 0);
    ck_assert_int_eq(value.bits[2], 0);
}
END_TEST

START_TEST(test_s21_shift_left_carry) {
    s21_decimal value = {{0x80000000, 0, 0, 0}};
    s21_shift_left(&value);
    ck_assert_int_eq(value.bits[0], 0);
    ck_assert_int_eq(value.bits[1], 1);
    ck_assert_int_eq(value.bits[2], 0);
}
END_TEST


START_TEST(test_s21_shift_left_multi_bit) {
    s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
    s21_shift_left(&value);
    ck_assert_int_eq(value.bits[0], 0xFFFFFFFE);
    ck_assert_int_eq(value.bits[1], 0xFFFFFFFF);
    ck_assert_int_eq(value.bits[2], 1);
}
END_TEST


START_TEST(test_s21_shift_left_top_bit) {
    s21_decimal value = {{0, 0, 0x80000000, 0}};
    s21_shift_left(&value);
    ck_assert_int_eq(value.bits[0], 0);
    ck_assert_int_eq(value.bits[1], 0);
    ck_assert_int_eq(value.bits[2], 0);
}
END_TEST

// Тест 5: Сдвиг нуля (должен остаться нулём)
START_TEST(test_s21_shift_left_zero) {
    s21_decimal value = {{0, 0, 0, 0}};
    s21_shift_left(&value);
    ck_assert_int_eq(value.bits[0], 0);
    ck_assert_int_eq(value.bits[1], 0);
    ck_assert_int_eq(value.bits[2], 0);
}
END_TEST

START_TEST(test_div_support_basic) {
    s21_decimal remainder = {{0, 0, 0, 0}};
    s21_decimal divisible = {{100, 0, 0, 0}};
    s21_decimal divisor = {{10, 0, 0, 0}};
    s21_decimal quotient = {{0, 0, 0, 0}};

    int res = div_support(&remainder, divisible, divisor, &quotient);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(quotient.bits[0], 10);
    ck_assert_int_eq(remainder.bits[0], 0);
}
END_TEST

START_TEST(test_div_support_with_remainder) {
    s21_decimal remainder = {{0, 0, 0, 0}};
    s21_decimal divisible = {{105, 0, 0, 0}};
    s21_decimal divisor = {{10, 0, 0, 0}};
    s21_decimal quotient = {{0, 0, 0, 0}};

    int res = div_support(&remainder, divisible, divisor, &quotient);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(quotient.bits[0], 10);
    ck_assert_int_eq(remainder.bits[0], 5);
}
END_TEST

START_TEST(test_div_support_large_number) {
    s21_decimal remainder = {{0, 0, 0, 0}};
    s21_decimal divisible = {{1000000, 0, 0, 0}};
    s21_decimal divisor = {{1000, 0, 0, 0}};
    s21_decimal quotient = {{0, 0, 0, 0}};

    int res = div_support(&remainder, divisible, divisor, &quotient);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(quotient.bits[0], 1000);
    ck_assert_int_eq(remainder.bits[0], 0);
}
END_TEST

START_TEST(test_div_support_zero_divisible) {
    s21_decimal remainder = {{0, 0, 0, 0}};
    s21_decimal divisible = {{0, 0, 0, 0}};
    s21_decimal divisor = {{123, 0, 0, 0}};
    s21_decimal quotient = {{123, 0, 0, 0}};

    int res = div_support(&remainder, divisible, divisor, &quotient);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(quotient.bits[0], 0);
    ck_assert_int_eq(remainder.bits[0], 0);
}
END_TEST

START_TEST(test_div_support_divide_by_one) {
    s21_decimal remainder = {{0, 0, 0, 0}};
    s21_decimal divisible = {{98765, 0, 0, 0}};  // 98765
    s21_decimal divisor = {{1, 0, 0, 0}};        // 1
    s21_decimal quotient = {{0, 0, 0, 0}};

    int res = div_support(&remainder, divisible, divisor, &quotient);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(quotient.bits[0], 98765);
    ck_assert_int_eq(remainder.bits[0], 0);
}
END_TEST



START_TEST(test_s21_div_by_10_basic) {
    s21_decimal a = {{1000, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    s21_div_by_10(1, a, &result);

    ck_assert_int_eq(result.bits[0], 100);
}
END_TEST

START_TEST(test_s21_div_by_10_multiple) {
    s21_decimal a = {{1000, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    s21_div_by_10(2, a, &result);

    ck_assert_int_eq(result.bits[0], 10);
}
END_TEST

START_TEST(test_s21_div_by_10_rounding) {
    s21_decimal a = {{105, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    s21_div_by_10(1, a, &result);

    ck_assert_int_eq(result.bits[0], 10);
}
END_TEST

START_TEST(test_s21_div_by_10_negative) {
    s21_decimal a = {{100, 0, 0, 0x80000000}};
    s21_decimal result = {{0, 0, 0, 0}};

    s21_div_by_10(1, a, &result);

    ck_assert_int_eq(result.bits[0], 10);
    ck_assert_int_eq(result.bits[3], 0x80000000);
}
END_TEST

START_TEST(test_s21_div_by_10_zero) {
    s21_decimal a = {{0, 0, 0, 0}};
    s21_decimal result = {{123, 0, 0, 0}};

    s21_div_by_10(3, a, &result);

    ck_assert_int_eq(result.bits[0], 0);
}
END_TEST

START_TEST(test_s21_div_by_10_large_number) {
    s21_decimal a = {{1000000, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    s21_div_by_10(3, a, &result);

    ck_assert_int_eq(result.bits[0], 1000);
}
END_TEST

START_TEST(test_s21_add_overflow) {
    s21_decimal a = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0}};
    s21_decimal b = {{UINT32_MAX, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_add(a, b, &result);

    ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_mul_basic) {
    s21_decimal a = {{2, 0, 0, 0}};
    s21_decimal b = {{3, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_mul(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 6);
}
END_TEST

START_TEST(test_s21_mul_overflow) {
    s21_decimal a = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, }};
    s21_decimal b = {{2, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_mul(a, b, &result);

    ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_mul_negative_result) {
    s21_decimal a = {{2, 0, 0, 0}};
    s21_decimal b = {{3, 0, 0, (1 << 31)}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_mul(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 6);
    ck_assert_int_eq((uint32_t) result.bits[3] & (1 << 31), (uint32_t) (1 << 31));
}
END_TEST

START_TEST(test_s21_div_basic) {
    s21_decimal a = {{6, 0, 0, 0}};
    s21_decimal b = {{2, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_div(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 3);
}
END_TEST

START_TEST(test_s21_div_by_zero) {
    s21_decimal a = {{5, 0, 0, 0}};
    s21_decimal b = {{0, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_div(a, b, &result);

    ck_assert_int_eq(res, 3);
}
END_TEST

START_TEST(test_s21_div_negative_result) {
    s21_decimal a = {{6, 0, 0, 0}};
    s21_decimal b = {{2, 0, 0, 1 << 31}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_div(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 3);
    ck_assert_int_eq((uint32_t) result.bits[3] & (1 << 31), (uint32_t) (1 << 31));
}
END_TEST

START_TEST(test_s21_is_less_basic) {
    s21_decimal a = {{1, 0, 0, 0}};
    s21_decimal b = {{2, 0, 0, 0}};

    int res = s21_is_less(a, b);

    ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_is_less_equal) {
    s21_decimal a = {{1, 0, 0, 0}};
    s21_decimal b = {{1, 0, 0, 0}};

    int res = s21_is_less_or_equal(a, b);

    ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_is_greater) {
    s21_decimal a = {{3, 0, 0, 0}};
    s21_decimal b = {{2, 0, 0, 0}};

    int res = s21_is_greater(a, b);

    ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_is_greater_or_equal) {
    s21_decimal a = {{3, 0, 0, 0}};
    s21_decimal b = {{2, 0, 0, 0}};

    int res = s21_is_greater_or_equal(a, b);

    ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_is_equal) {
    s21_decimal a = {{5, 0, 0, 0}};
    s21_decimal b = {{5, 0, 0, 0}};

    int res = s21_is_equal(a, b);

    ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_is_not_equal) {
    s21_decimal a = {{5, 0, 0, 0}};
    s21_decimal b = {{3, 0, 0, 0}};

    int res = s21_is_not_equal(a, b);

    ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_floor) {
    s21_decimal a = {{314, 0, 0, (2 << 16)}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_floor(a, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 3);
}
END_TEST

START_TEST(test_s21_round) {
    s21_decimal a = {{314, 0, 0, (2 << 16)}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_round(a, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 3);
}
END_TEST

START_TEST(test_s21_truncate) {
    s21_decimal a = {{314, 0, 0, (2 << 16)}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_truncate(a, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 3);
}
END_TEST

START_TEST(test_s21_negate) {
    s21_decimal a = {{314, 0, 0, (3 << 16)}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_negate(a, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 314);
    ck_assert_int_eq((uint32_t) result.bits[3] & (1 << 31), (uint32_t) (1 << 31));
}
END_TEST

START_TEST(test_s21_from_int_to_decimal) {
    int a = 123456789;
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_from_int_to_decimal(a, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 123456789);
}
END_TEST

START_TEST(test_s21_from_float_to_decimal) {
    float a = 123.456f;
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_from_float_to_decimal(a, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 123456);
    ck_assert_int_eq(result.bits[3] >> 16, 3);
}
END_TEST

START_TEST(test_s21_from_decimal_to_int) {
    s21_decimal a = {{123456789, 0, 0, 0}};
    int result = 0;

    int res = s21_from_decimal_to_int(a, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result, 123456789);
}
END_TEST

START_TEST(test_s21_from_decimal_to_float) {
    s21_decimal a = {{123456789, 0, 0, 0}};
    float result = 0.0f;

    int res = s21_from_decimal_to_float(a, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_float_eq(result, 123456789.0f);
}
END_TEST

START_TEST(test_s21_div_fraction) {
    s21_decimal a = {{1, 0, 0, 0}};
    s21_decimal b = {{2, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_div(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 5);
    ck_assert_int_eq(s21_get_scale(result), 1);
}
END_TEST

START_TEST(test_s21_div_large_numbers) {
    s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    s21_decimal b = {{2, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_div(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);
    ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
    ck_assert_int_eq(result.bits[2], 0x7FFFFFFF);
}
END_TEST

START_TEST(test_s21_div_negative) {
    s21_decimal a = {{100, 0, 0, 0x80000000}};
    s21_decimal b = {{10, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_div(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 10);
    ck_assert_int_eq(result.bits[3], 0x80000000);
}
END_TEST

START_TEST(test_s21_div_two_negatives) {
    s21_decimal a = {{100, 0, 0, 0x80000000}};
    s21_decimal b = {{10, 0, 0, 0x80000000}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_div(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 10);
    ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_div_scale_adjustment) {
    s21_decimal a = {{12345, 0, 0, (3 << 16)}};
    s21_decimal b = {{100, 0, 0, (2 << 16)}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_div(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 12345);
    ck_assert_int_eq(s21_get_scale(result), 3);
}
END_TEST

START_TEST(test_s21_multiply_by_10_basic) {
    s21_decimal value = {{5, 0, 0, 0}};
    int res = s21_multiply_by_10(&value);
    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(value.bits[0], 50);
}
END_TEST

START_TEST(test_s21_multiply_by_10_large_number) {
    s21_decimal value = {{0x20000000, 0, 0, 0}};
    int res = s21_multiply_by_10(&value);
    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(value.bits[0], 0x40000000);
}
END_TEST

START_TEST(test_s21_multiply_by_10_overflow) {
    s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    int res = s21_multiply_by_10(&value);
    ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_multiply_by_10_middle_overflow) {
    s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
    int res = s21_multiply_by_10(&value);
    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(value.bits[2], 9);
}
END_TEST

START_TEST(test_s21_mul_carry_propagation) {
    s21_decimal a = {{0, 0, 0xFFFFFFFF, 0}};
    s21_decimal b = {{0, 0, 0xFFFFFFFF, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_mul(a, b, &result);

    ck_assert_int_eq(res, 1);
}
END_TEST

Suite *s21_decimal_suite(void) {
    Suite *s = suite_create("s21_decimal");
    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_s21_mul_carry_propagation);
    tcase_add_test(tc_core, test_s21_mul_bit_no_carry);
    tcase_add_test(tc_core, test_s21_mul_bit_with_carry);
    tcase_add_test(tc_core, test_s21_mul_bit_overflow);
    tcase_add_test(tc_core, test_s21_mul_bit_large_numbers);
    tcase_add_test(tc_core,test_s21_equalize_scales_same_scale);
    tcase_add_test(tc_core,test_s21_equalize_scales_increase_scale_1);
    tcase_add_test(tc_core,test_s21_equalize_scales_increase_scale_2);
    tcase_add_test(tc_core,test_s21_equalize_scales_multiply_overflow);

    tcase_add_test(tc_core,test_s21_shift_left_zero);
    tcase_add_test(tc_core, test_s21_shift_left_basic);
    tcase_add_test(tc_core, test_s21_shift_left_carry);
    tcase_add_test(tc_core, test_s21_shift_left_multi_bit);
    tcase_add_test(tc_core, test_s21_shift_left_top_bit);

    tcase_add_test(tc_core, test_div_support_basic);
    tcase_add_test(tc_core, test_div_support_with_remainder);
    tcase_add_test(tc_core, test_div_support_large_number);
    tcase_add_test(tc_core, test_div_support_zero_divisible);
    tcase_add_test(tc_core, test_div_support_divide_by_one);
    tcase_add_test(tc_core, test_s21_div_by_10_basic);
    tcase_add_test(tc_core, test_s21_div_by_10_rounding);
    tcase_add_test(tc_core, test_s21_div_by_10_negative);
    tcase_add_test(tc_core, test_s21_div_by_10_zero);
    tcase_add_test(tc_core, test_s21_div_by_10_multiple);

    tcase_add_test(tc_core, test_s21_add_basic);
    tcase_add_test(tc_core, test_s21_add_positive);
    tcase_add_test(tc_core, test_s21_add_negative);
    tcase_add_test(tc_core, test_s21_add_opposite_numbers);
    tcase_add_test(tc_core, test_s21_add_overflow);
    tcase_add_test(tc_core, test_s21_add_with_zero);
    tcase_add_test(tc_core, test_s21_add_negative_overflow);
    tcase_add_test(tc_core, test_s21_add_complex_case);
    tcase_add_test(tc_core, test_s21_sub_basic);
    tcase_add_test(tc_core, test_s21_sub_underflow);
    tcase_add_test(tc_core, test_s21_sub_negative_result);
    tcase_add_test(tc_core, test_s21_sub_large_numbers);
    tcase_add_test(tc_core, test_s21_sub_same_numbers);
    tcase_add_test(tc_core, test_s21_sub_negative_zero);
    tcase_add_test(tc_core, test_s21_sub_overflow_negative);
    tcase_add_test(tc_core, test_s21_sub_carry_propagation);

    tcase_add_test(tc_core, test_s21_mul_basic);
    tcase_add_test(tc_core, test_s21_mul_overflow);
    tcase_add_test(tc_core, test_s21_mul_negative_result);
    tcase_add_test(tc_core, test_s21_div_basic);
    tcase_add_test(tc_core, test_s21_div_by_zero);
    tcase_add_test(tc_core, test_s21_div_negative_result);
    tcase_add_test(tc_core, test_s21_is_less_basic);
    tcase_add_test(tc_core, test_s21_is_less_equal);
    tcase_add_test(tc_core, test_s21_is_greater);
    tcase_add_test(tc_core, test_s21_is_greater_or_equal);
    tcase_add_test(tc_core, test_s21_is_equal);
    tcase_add_test(tc_core, test_s21_is_not_equal);
    tcase_add_test(tc_core, test_s21_floor);
    tcase_add_test(tc_core, test_s21_round);
    tcase_add_test(tc_core, test_s21_truncate);
    tcase_add_test(tc_core, test_s21_negate);
    tcase_add_test(tc_core, test_s21_from_int_to_decimal);
    tcase_add_test(tc_core, test_s21_from_float_to_decimal);
    tcase_add_test(tc_core, test_s21_from_decimal_to_int);
    tcase_add_test(tc_core, test_s21_from_decimal_to_float);

    tcase_add_test(tc_core, test_s21_div_fraction);
    tcase_add_test(tc_core, test_s21_div_large_numbers);
    tcase_add_test(tc_core, test_s21_div_negative);
    tcase_add_test(tc_core, test_s21_div_two_negatives);
    tcase_add_test(tc_core, test_s21_div_scale_adjustment);

    tcase_add_test(tc_core, test_s21_multiply_by_10_basic);
    tcase_add_test(tc_core, test_s21_multiply_by_10_large_number);
    tcase_add_test(tc_core, test_s21_multiply_by_10_overflow);
    tcase_add_test(tc_core, test_s21_multiply_by_10_middle_overflow);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void) {
    int number_failed;
    Suite *s = s21_decimal_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

