#include <check.h>
#include <stdlib.h>
#include "s21_decimal.h"
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

START_TEST(test_s21_add_overflow) {
    s21_decimal a = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0}};
    s21_decimal b = {{UINT32_MAX, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_add(a, b, &result);

    ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_add_negative) {
    s21_decimal a = {{2, 0, 0, 0}};
    s21_decimal b = {{3, 0, 0, 0}};
    s21_decimal result = {{0, 0, 0, 0}};

    int res = s21_add(a, b, &result);

    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(result.bits[0], 5);
}
END_TEST

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
    ck_assert_int_eq((uint32_t) result.bits[3] & (1 << 31), (uint32_t) (1 << 31));
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

Suite *s21_decimal_suite(void) {
    Suite *s = suite_create("s21_decimal");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_s21_add_basic);
    tcase_add_test(tc_core, test_s21_add_overflow);
    tcase_add_test(tc_core, test_s21_add_negative);
    tcase_add_test(tc_core, test_s21_sub_basic);
    tcase_add_test(tc_core, test_s21_sub_underflow);
    tcase_add_test(tc_core, test_s21_sub_negative_result);
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

    suite_add_tcase(s, tc_core);
    return s;
}

//int main(void) {
//    int number_failed;
//    Suite *s = s21_decimal_suite();
//    SRunner *sr = srunner_create(s);
//
//    srunner_run_all(sr, CK_NORMAL);
//    number_failed = srunner_ntests_failed(sr);
//    srunner_free(sr);
//
//    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
//}

