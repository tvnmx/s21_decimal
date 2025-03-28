#include "helpers.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal zero = {{0, 0, 0, 0}};
  *result = zero;
  int error = 0;
  int not_equal_scales = 0;
  s21_equalize_scales(&value_1, &value_2, &not_equal_scales);
    s21_decimal min_value, max_value;
    // модуль поставить
    if (s21_is_less(value_1, value_2)) {
      min_value = value_1;
      max_value = value_2;
  } else {
      min_value = value_2;
        max_value = value_1;
  }
  if (not_equal_scales) {
      // надо добавить целую часть второго числа
      s21_decimal fr_part = s21_get_fr_part(min_value);
      if (s21_is_greater(fr_part, (s21_decimal){{5, 0, 0, 1 << 16}}) || (s21_is_equal(fr_part, (s21_decimal){{5, 0, 5, 1 << 16}}) && max_value.bits[0] % 2 == 1)) {
          s21_print_decimal(max_value);
          if (s21_get_sign(value_1) == 1) {
              s21_add_with_equal_signs(&error, result, max_value, (s21_decimal){{1, 0, 0, 1 << 31}});
          } else {
              s21_add_with_equal_signs(&error, result, max_value, (s21_decimal) {{1, 0, 0, 0}});
          }
      } else {
          result->bits[0] = max_value.bits[0];
          result->bits[1] = max_value.bits[1];
          result->bits[2] = max_value.bits[2];
          result->bits[3] = max_value.bits[3];
      }
  } else {
      if (s21_get_sign(value_1) == s21_get_sign(value_2)) {
          s21_add_with_equal_signs(&error, result, value_1, value_2);
      } else {
          s21_add_with_diff_signs(&error, result, value_1, value_2);
      }
  }
  if (s21_get_sign(value_1) == 1 &&
      s21_get_sign(value_1) == s21_get_sign(value_2)) {
    s21_set_sign(result, 1);
  }
  return error;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal zero = {{0, 0, 0, 0}};
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
  *result = (s21_decimal){{0, 0, 0, 0}};
  uint32_t scale1 = s21_get_scale(value_1);
  uint32_t scale2 = s21_get_scale(value_2);
  uint32_t res_scale = (scale1 + scale2 > 28) ? 28 : (scale1 + scale2);
  s21_set_scale(result, res_scale);
  s21_set_sign(result, s21_get_sign(value_1) != s21_get_sign(value_2));

  uint64_t temp[6] = {0, 0, 0, 0, 0, 0};

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      uint64_t mul = (uint64_t)value_1.bits[i] * (uint64_t)value_2.bits[j];
      temp[i + j] += mul;
    }
  }

  for (int k = 0; k < 5; k++) {
    uint64_t carry = temp[k] >> 32;
    temp[k] &= 0xFFFFFFFF;
    temp[k + 1] += carry;
  }
  int overflow = (temp[3] || temp[4] || temp[5]) ? 1 : 0;

  result->bits[0] = (uint32_t)temp[0];
  result->bits[1] = (uint32_t)temp[1];
  result->bits[2] = (uint32_t)temp[2];

  return overflow;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int error = 0;
    if ((value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0)) {
        error = 3;
    } else {
        s21_decimal divisible = value_1, divisor = value_2;
        s21_decimal quotient = {{0, 0, 0, 0}}, remainder = {{0, 0, 0, 0}};
        s21_equalize_scales(&divisible, &divisor, &error);

        uint32_t initial_scale = s21_get_scale(divisible) - s21_get_scale(divisor);

        divisible.bits[3] = 0;
        divisor.bits[3] = 0;

        s21_div_support(&remainder, divisible, divisor, &quotient);
        int fractional_scale = 0;

        s21_decimal fractional = {{0, 0, 0, 0}};
        s21_decimal digit = {{0, 0, 0, 0}};
        s21_decimal temp = {{0, 0, 0, 0}};

        while ((remainder.bits[0] != 0 || remainder.bits[1] != 0 ||
                remainder.bits[2] != 0) && fractional_scale < 28) {
            s21_multiply_by_10(&remainder);
            s21_div_support(&temp, remainder, divisor, &digit);
            remainder.bits[0] = temp.bits[0];
            remainder.bits[1] = temp.bits[1];
            remainder.bits[2] = temp.bits[2];
            s21_multiply_by_10(&fractional);
            s21_add(fractional, digit, &fractional);
            fractional_scale++;
        }
        div_bank_round(&quotient, &fractional, &fractional_scale, &divisor, remainder);
        uint32_t final_scale = initial_scale + fractional_scale > 28 ? 28 : initial_scale + fractional_scale;
        s21_set_scale(&quotient, final_scale);
        s21_set_sign(&quotient,
                     ((value_1.bits[3] >> 31) ^ (value_2.bits[3] >> 31)) & 1);
        *result = quotient;
    }
    return error;
}

int s21_is_less(s21_decimal a, s21_decimal b) { return s21_is_greater(b, a); }

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

  if ((tmp1.bits[0] == tmp2.bits[0] && tmp1.bits[1] == tmp2.bits[1] &&
       tmp1.bits[2] == tmp2.bits[2] && tmp1.bits[3] == tmp2.bits[3]) ||
      (tmp1.bits[0] == 0 && tmp1.bits[1] == 0 && tmp1.bits[2] == 0 &&
       tmp2.bits[0] == 0 && tmp2.bits[1] == 0 && tmp2.bits[2] == 0)) {
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
  if (isnan(src) || isinf(src) ||
      fabs((double)src) > 7.9228162514264337593543950335e28 ||
      fabs((double)src) < 1e-28) {
    flag = 1;
  }
  if (flag == 0) {
    int sign = src < 0 ? 1 : 0;
    src = (float)fabs((double)src);
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
    int integer_part = (int)round((double)src);
    dst->bits[0] = integer_part;
    dst->bits[3] |= (scale << 16);
    s21_set_sign(dst, sign);
  }
  return flag;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int res = !s21_is_valid_decimal(src);
  if (res == 0) {
    if (src.bits[3] >> 16 != 0) {
      uint32_t scale = (src.bits[3] >> 16) & 0xFF;
      uint32_t integer_part = src.bits[0];
      while (scale > 0) {
        integer_part /= 10;
        scale--;
      }
      src.bits[0] = integer_part;
    }
    *dst = (int)src.bits[0];
    if (s21_get_sign(src)) {
      *dst = -*dst;
    }
  }
  return res;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int res = !s21_is_valid_decimal(src);
  if (res == 0) {
    float result = 0;
    result += (float)src.bits[0];
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
    if (sign == 1 && !(fr_part.bits[0] == 0 && fr_part.bits[1] == 0 &&
                       fr_part.bits[2] == 0)) {
      s21_decimal one = {.bits = {1, 0, 0, 0}};
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
    s21_from_float_to_decimal((float)0.5, &fractional1);
    if (s21_is_greater_or_equal(fractional_part, fractional1) == 1) {
      if (sign == 0) {
        s21_decimal one = {.bits = {1, 0, 0, 0}};
        s21_add(*result, one, result);
      } else {
        s21_decimal one = {.bits = {1, 0, 0, 0}};
        s21_sub(*result, one, result);
      }
    }
  }
  return res;
}

int main() {
    s21_decimal a = {{0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    s21_decimal b = {{0, 0, 5, 1 << 16}};
    s21_decimal result = {{0, 0, 0, 0}};
    int res = s21_add(a, b, &result);
    s21_print_decimal(result);
}