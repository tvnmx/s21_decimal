#include "helpers.h"

void s21_add_with_rounding(s21_decimal value_1, s21_decimal value_2, int *error, s21_decimal *result) {
    s21_decimal int_part_1 = {{0, 0, 0, 0}}, int_part_2 = {{0, 0, 0, 0}}, sum_fr_part = {{0, 0, 0, 0}}, int_part_from_sum_fr_part = {{0, 0, 0, 0}};
    s21_decimal fr_part_1 = s21_get_fr_part(value_1);
    s21_decimal fr_part_2 = s21_get_fr_part(value_2);
    s21_truncate(value_1, &int_part_1);
    s21_truncate(value_2, &int_part_2);
    if (s21_get_sign(value_1) == s21_get_sign(value_2)) {
        s21_add_with_equal_signs(error, &sum_fr_part, fr_part_1, fr_part_2);
    } else {
        s21_add_with_diff_signs(error, &sum_fr_part, fr_part_1, fr_part_2);
    }
    s21_decimal fr_part_for_sum_fr_part = s21_get_fr_part(sum_fr_part);
    s21_truncate(sum_fr_part, &int_part_from_sum_fr_part);
    if (s21_get_sign(value_1) == s21_get_sign(int_part_from_sum_fr_part)) {
        s21_add_with_equal_signs(error, &value_1, value_1, int_part_from_sum_fr_part);
    } else {
        s21_add_with_diff_signs(error, &value_1, value_1, int_part_from_sum_fr_part);
    }

    if (s21_get_sign(value_1) == s21_get_sign(value_2)) {
        s21_add_with_equal_signs(error, result, int_part_1, int_part_2);
    } else {
        s21_add_with_diff_signs(error, result, int_part_1, int_part_2);
    }
    if (!(*error) && (s21_is_greater(fr_part_for_sum_fr_part, (s21_decimal){{5, 0, 0, 1 << 16}}) || (s21_is_equal(fr_part_for_sum_fr_part, (s21_decimal){{5, 0, 0, 1 << 16}}) && result->bits[0] % 2 == 1))) {
        if (s21_get_sign(*result)) {
            s21_add_with_equal_signs(error, result, *result, (s21_decimal){{1, 0, 0, 1 << 31}});
        } else {
            s21_add_with_equal_signs(error, result, *result, (s21_decimal) {{1, 0, 0, 0}});
        }
    }
}

s21_decimal s21_get_fr_part(s21_decimal value) {
    s21_decimal result = {{0, 0, 0, 0}};
    uint32_t scale = s21_get_scale(value);
    if (scale) {
        s21_decimal int_part = {{0, 0, 0, 0}};
        s21_truncate(value, &int_part);
        s21_sub(value, int_part, &result);
    }
    s21_set_scale(&result, scale);
    return result;
}

void s21_add_with_equal_signs(int *error, s21_decimal *result,
                              s21_decimal value_1, s21_decimal value_2) {
    uint64_t carry = 0;
  uint32_t sign1 = s21_get_sign(value_1);
    uint32_t max_scale = s21_get_scale(value_1) > s21_get_scale(value_2) ? s21_get_scale(value_1) : s21_get_scale(value_2);
  for (int i = 0; i < 3; i++) {
    uint64_t sum = (uint64_t)value_1.bits[i] + value_2.bits[i] + carry;
    result->bits[i] = (uint32_t)(sum & 0xFFFFFFFF);
    carry = sum >> 32;
  }
  if (carry && !max_scale) {
    *error = sign1 == 0 ? 1 : 2;
  }
  if (sign1) {
    s21_set_sign(result, 1);
  }
    s21_set_scale(result, max_scale);
}

void s21_add_with_diff_signs(int *error, s21_decimal *result,
                             s21_decimal value_1, s21_decimal value_2) {
  s21_decimal abs_value_1 = value_1;
  s21_decimal abs_value_2 = value_2;

  abs_value_1.bits[3] &= 0x7FFFFFFF;
  abs_value_2.bits[3] &= 0x7FFFFFFF;

  if (s21_is_greater_or_equal(abs_value_1, abs_value_2)) {
    value_2.bits[3] ^= 0x80000000;
    *error = s21_sub(value_1, value_2, result);
  } else {
    value_1.bits[3] ^= 0x80000000;
    *error = s21_sub(value_2, value_1, result);
    result->bits[3] ^= 0x80000000;
  }
    uint32_t max_scale = s21_get_scale(value_1) > s21_get_scale(value_2) ? s21_get_scale(value_1) : s21_get_scale(value_2);
    s21_set_scale(result, max_scale);
}

void s21_sub_with_equal_signs(s21_decimal *result, s21_decimal value_1,
                              s21_decimal value_2) {
  int borrow = 0;
  if (s21_is_greater_or_equal(value_1, value_2)) {
    for (int i = 0; i < 3; i++) {
      int64_t diff = (int64_t)value_1.bits[i] - value_2.bits[i] - borrow;
      if (diff < 0) {
        borrow = 1;
        diff += (1LL << 32);
      } else {
        borrow = 0;
      }
      result->bits[i] = (uint32_t)diff;
    }
  } else {
    for (int i = 0; i < 3; i++) {
      int64_t diff = (int64_t)value_2.bits[i] - value_1.bits[i] - borrow;
      if (diff < 0) {
        borrow = 1;
        diff += (1LL << 32);
      } else {
        borrow = 0;
      }
      result->bits[i] = (uint32_t)diff;
    }
    result->bits[3] = 0x80000000;
  }
}

void s21_sub_with_diff_signs(int *error, s21_decimal *result,
                             s21_decimal value_1, s21_decimal value_2) {
    s21_decimal abs_value_1 = value_1;
  s21_decimal abs_value_2 = value_2;

  abs_value_1.bits[3] &= 0x7FFFFFFF;
  abs_value_2.bits[3] &= 0x7FFFFFFF;
  if (s21_is_greater_or_equal(abs_value_1, abs_value_2)) {
    value_2.bits[3] ^= 0x80000000;
    *error = s21_add(value_1, value_2, result);
  } else {
    value_1.bits[3] ^= 0x80000000;
    *error = s21_add(value_2, value_1, result);
    result->bits[3] ^= 0x80000000;
  }
  s21_set_sign(result, 1);
}

int s21_get_bit(s21_decimal value, int index) {
  uint32_t bit_position = index % 32;
  uint32_t word_index = index / 32;
  return (value.bits[word_index] >> bit_position) & 1;
}

void s21_set_bit(s21_decimal *value, int index, int bit) {
  int bit_position = index % 32;
  int word_index = index / 32;
  if (bit)
    value->bits[word_index] |= (1U << bit_position);
  else
    value->bits[word_index] &= ~(1U << bit_position);
}

int s21_div_support(s21_decimal *remainder, s21_decimal divisible,
                    s21_decimal divisor, s21_decimal *quotient) {
  *remainder = (s21_decimal){{0, 0, 0, 0}};
  *quotient = (s21_decimal){{0, 0, 0, 0}};

  for (int i = 95; i >= 0; i--) {
    s21_shift_left(remainder);
    if (s21_get_bit(divisible, i)) {
      s21_set_bit(remainder, 0, 1);
    }

    if (s21_is_greater_or_equal(*remainder, divisor)) {
      s21_sub(*remainder, divisor, remainder);
      s21_set_bit(quotient, i, 1);
    }
  }
  return 0;
}


// отрицательные числа ???
void div_bank_round(s21_decimal *quotient, s21_decimal *fractional, int *fractional_scale, s21_decimal *divisor, s21_decimal remainder) {
    s21_decimal zero = {{0, 0, 0, 0}};
    s21_decimal one = {{1, 0, 0, 0}};
    s21_decimal two = {{2, 0, 0, 0}};
    s21_decimal five = {{5, 0, 0, 0}};
    uint64_t chislo = (uint64_t) quotient->bits[0] + (uint64_t) fractional->bits[0];
    s21_decimal rem_for_round = remainder, scaled_int = *quotient;
    int error;
    if (chislo <= UINT32_MAX) {
        for (int i = 0; i < *fractional_scale; i++) {
            s21_multiply_by_10(&scaled_int);
        }
        s21_add_with_equal_signs(&error, quotient, scaled_int, *fractional);
    } else {
        (*fractional_scale)--;
    }
    if ((*fractional_scale == 28 && !s21_is_equal(remainder, zero)) || chislo > UINT32_MAX) {
        if (s21_is_equal(remainder, zero) && (s21_is_greater(*fractional, five) ||
                                              (s21_is_equal(*fractional, five) && quotient->bits[0] % 2 == 1))) {
            s21_add_with_equal_signs(&error, quotient, *quotient, one);
        } else {
            s21_mul(rem_for_round, two, &rem_for_round);
            if (s21_is_greater(rem_for_round, *divisor) ||
                (s21_is_equal(rem_for_round, *divisor) && quotient->bits[0] % 2 == 1)) {
                s21_add_with_equal_signs(&error, quotient, *quotient, one);
            }
        }
    }
}

void s21_print_decimal(s21_decimal dec) {
    for (int i = 3; i >= 0; i--) {
        for (int j = 31; j >= 0; j--) {
            printf("%u", (dec.bits[i] >> j) & 1);
            if (j % 4 == 0) printf(" ");
        }
        printf("\n");
    }
    printf("\n");
    for (int i = 3; i >= 0; i--) {
        printf("bits[%d] = 0x%08X\n", i, dec.bits[i]);
    }
    printf("\n");
}

void s21_shift_left(s21_decimal *val) {
    uint32_t carry = 0;
    for (int i = 0; i < 3; i++) {
        uint64_t temp = ((uint64_t)val->bits[i]) << 1 | carry;
        val->bits[i] = (uint32_t)temp;
        carry = (uint32_t)(temp >> 32);
    }
}

void s21_equalize_scales(s21_decimal *value_1, s21_decimal *value_2,
                         int *error) {
  uint32_t scale1 = s21_get_scale(*value_1);
  uint32_t scale2 = s21_get_scale(*value_2);
  while (scale1 != scale2 && !(*error)) {
    if (scale1 < scale2) {
      if (s21_multiply_by_10(value_1)) {
        *error = 1;
      } else {
          scale1++;
      }
    } else {
      if (s21_multiply_by_10(value_2)) {
        *error = 1;
      } else {
          scale2++;
      }
    }
  }
  s21_set_scale(value_1, scale1);
  s21_set_scale(value_2, scale2);
}

uint32_t s21_get_sign(s21_decimal value) {
  decimal_bit3 db3;
  db3.i = value.bits[3];
  return db3.parts.sign;
}

uint32_t s21_get_scale(s21_decimal value) {
  decimal_bit3 db3;
  db3.i = value.bits[3];
  return db3.parts.scale;
}

int s21_multiply_by_10(s21_decimal *value) {
    s21_decimal tmp;
    tmp.bits[0] = (uint32_t)value->bits[0];
    tmp.bits[1] = (uint32_t)value->bits[1];
    tmp.bits[2] = (uint32_t)value->bits[2];
  uint64_t carry = 0;
  int res = 0;
  for (int i = 0; i < 3; i++) {
    uint64_t ans = (uint64_t)tmp.bits[i] * 10 + carry;

    if (ans > 0xFFFFFFFF) {
      carry = ans >> 32;
      tmp.bits[i] = (uint32_t)(ans & 0xFFFFFFFF);
    } else {
      carry = 0;
      tmp.bits[i] = (uint32_t)ans;
    }
  }
  if (carry != 0) {
    res = 1;
  } else {
      value->bits[0] = (uint32_t)tmp.bits[0];
      value->bits[1] = (uint32_t)tmp.bits[1];
      value->bits[2] = (uint32_t)tmp.bits[2];
  }

  return res;
}

void s21_decimal_zero(s21_decimal *dst) {
  for (int i = 0; i < 4; i++) {
    dst->bits[i] = 0;
  }
}

void s21_set_sign(s21_decimal *dst, uint32_t sign) {
  uint32_t sign_bit = (uint32_t)1 << 31;

  if (sign) {
    dst->bits[3] |= sign_bit;
  } else {
    dst->bits[3] &= ~sign_bit;
  }
}

void s21_set_scale(s21_decimal *decimal, uint32_t scale) {
  decimal_bit3 db3;
  db3.i = decimal->bits[3];
  db3.parts.scale = scale;
  decimal->bits[3] = db3.i;
}

void s21_dec_assignment(s21_decimal value, s21_decimal *result) {
  result->bits[0] = value.bits[0];
  result->bits[1] = value.bits[1];
  result->bits[2] = value.bits[2];
  result->bits[3] = value.bits[3];
}

bool s21_is_valid_decimal(s21_decimal value) {
  bool result = true;
  decimal_bit3 db3;
  db3.i = value.bits[3];
  if (db3.parts.empty2 != 0 || db3.parts.empty1 != 0 || db3.parts.scale > 28 ||
      db3.parts.sign > 1) {
    result = false;
  }
  return result;
}

bool s21_are_all_bits_zero(s21_decimal value) {
  bool result = false;
  if (value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0 &&
      value.bits[3] == 0) {
    result = true;
  }
  return result;
}

s21_decimal s21_trim_trailing_zeros(s21_decimal value) {
  s21_decimal result = value;
  uint32_t scale = s21_get_scale(value);
  if (scale > 0 && s21_is_valid_decimal(value)) {
    s21_decimal ost = {.bits = {0, 0, 0, 0}};
    s21_decimal tmp = value;
    tmp.bits[3] = 0;
    s21_decimal DECIMAL_TEN = {.bits = {10, 0, 0, 0}};
    while (scale > 0 && s21_are_all_bits_zero(ost)) {
      s21_div(tmp, DECIMAL_TEN, &tmp);
      s21_ostatok(tmp, &ost);
      if (s21_are_all_bits_zero(ost)) {
        --scale;
        result = tmp;
      }
    }
  }
  result.bits[3] = value.bits[3];
  s21_set_scale(&result, scale);
  return result;
}

void s21_div_by_10(uint32_t power, s21_decimal value, s21_decimal *result) {
  int num = value.bits[0] % (uint32_t)pow(10, power);
  s21_decimal acc;
  s21_from_int_to_decimal(num, &acc);
  s21_sub(value, acc, result);
  s21_decimal ten = {.bits = {10, 0, 0, 0}};
  for (uint32_t i = 0; i < power; i++) {
    s21_div(*result, ten, result);
  }
}

void s21_ostatok(s21_decimal value, s21_decimal *result) {
  uint32_t power = s21_get_scale(value);
  int num = value.bits[0] % (uint32_t)pow(10, power);
  s21_from_int_to_decimal(num, result);
}