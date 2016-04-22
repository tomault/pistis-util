/**@file NumUtilTests.cpp
 *
 * Unit tests for the functions in pistis/util/NumItil.hpp.
 */

#include <pistis/util/NumUtil.hpp>
#include <pistis/exceptions/IllegalValueError.hpp>
#include <gtest/gtest.h>
#include <cmath>

using namespace pistis::exceptions;
using namespace pistis::util;

TEST(NumUtilTests, StrToInt64) {
  const char* const EMPTY_STRING= "";
  const char* const ALL_WHITESPACE= "    \t  \r\n\r\n   ";
  const char* const BASE10_VALUE= "123";
  const char* const BASE16_VALUE= "0x123";
  const char* const BASE8_VALUE= "0123";
  const char* const BASE16_VALUE_NO_PREFIX= "1AF";
  const char* const NEGATIVE_VALUE= "-123";
  const char* const PADDED_VALUE= "   123   ";
  const char* const VALUE_WITH_EXTRA_CHARS = "9876.123";
  const char* const NOT_AN_INTEGER = "  the quick brown fox jumped";
  const char* const OVERFLOWS = "1348742838729873492873472389239890823";
  const char* const UNDERFLOWS = "-1348742838729873492873472389239890823";
  const char* end= nullptr;
  NumConversionResult r;
  int64_t v= strToInt64(EMPTY_STRING, 0, &end, &r);

  EXPECT_EQ(v, 0);
  EXPECT_EQ(end, EMPTY_STRING);
  EXPECT_EQ(r, NumConversionResult::EMPTY_STRING);

  v= strToInt64(ALL_WHITESPACE, 0, &end, &r);
  EXPECT_EQ(v, 0);
  EXPECT_EQ(end, ALL_WHITESPACE+strlen(ALL_WHITESPACE));
  EXPECT_EQ(r, NumConversionResult::EMPTY_STRING);

  v= strToInt64(BASE10_VALUE, 0, &end, &r);
  EXPECT_EQ(v, 123);
  EXPECT_EQ(end, BASE10_VALUE+strlen(BASE10_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToInt64(BASE16_VALUE, 0, &end, &r);
  EXPECT_EQ(v, 291);
  EXPECT_EQ(end, BASE16_VALUE+strlen(BASE16_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToInt64(BASE8_VALUE, 0, &end, &r);
  EXPECT_EQ(v, 83);
  EXPECT_EQ(end, BASE8_VALUE+strlen(BASE8_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToInt64(BASE10_VALUE, 16, &end, &r);
  EXPECT_EQ(v, 291);
  EXPECT_EQ(end, BASE10_VALUE+strlen(BASE10_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToInt64(BASE8_VALUE, 10, &end, &r);
  EXPECT_EQ(v, 123);
  EXPECT_EQ(end, BASE8_VALUE+strlen(BASE8_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToInt64(BASE16_VALUE, 10, &end, &r);
  EXPECT_EQ(v, 0);
  EXPECT_EQ(end, BASE16_VALUE+1);  // Stops at the "x"
  EXPECT_EQ(r, NumConversionResult::OK);

  // Base=0 treats the input as base-10 if it has no prefix
  v= strToInt64(BASE16_VALUE_NO_PREFIX, 0, &end, &r);
  EXPECT_EQ(v, 1);
  EXPECT_EQ(end, BASE16_VALUE_NO_PREFIX+1);  // Stops at "A"
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToInt64(BASE16_VALUE_NO_PREFIX, 16, &end, &r);
  EXPECT_EQ(v, 431);
  EXPECT_EQ(end, BASE16_VALUE_NO_PREFIX+strlen(BASE16_VALUE_NO_PREFIX));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToInt64(NEGATIVE_VALUE, 0, &end, &r);
  EXPECT_EQ(v, -123);
  EXPECT_EQ(end, NEGATIVE_VALUE + strlen(NEGATIVE_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToInt64(PADDED_VALUE, 0, &end, &r);
  EXPECT_EQ(v, 123);
  EXPECT_EQ(end, PADDED_VALUE+6); // Stops at first trailing space
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToInt64(VALUE_WITH_EXTRA_CHARS, 0, &end, &r);
  EXPECT_EQ(v, 9876);
  EXPECT_EQ(end, VALUE_WITH_EXTRA_CHARS+4);  // Stops at "."
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToInt64(NOT_AN_INTEGER, 0, &end, &r);
  // Return value is undefined, so don't check it
  EXPECT_EQ(end, NOT_AN_INTEGER+2);  // Initial whitespace skipped
  EXPECT_EQ(r, NumConversionResult::NOT_AN_INTEGER);

  v= strToInt64(UNDERFLOWS, 0, &end, &r);
  EXPECT_EQ(v, INT64_MIN);
  EXPECT_EQ(end, UNDERFLOWS+strlen(UNDERFLOWS));
  EXPECT_EQ(r, NumConversionResult::UNDERFLOWED);

  v= strToInt64(OVERFLOWS, 0, &end, &r);
  EXPECT_EQ(v, INT64_MAX);
  EXPECT_EQ(end, OVERFLOWS+strlen(OVERFLOWS));
  EXPECT_EQ(r, NumConversionResult::OVERFLOWED);

  // Invalid bases - return value is undefined in these cases, so it is not
  // checked
  v= strToInt64(PADDED_VALUE, -1, &end, &r);
  EXPECT_EQ(end, PADDED_VALUE);
  EXPECT_EQ(r, NumConversionResult::INVALID_BASE);

  v= strToInt64(PADDED_VALUE, 1, &end, &r);
  EXPECT_EQ(end, PADDED_VALUE);
  EXPECT_EQ(r, NumConversionResult::INVALID_BASE);

  v= strToInt64(PADDED_VALUE, 37, &end, &r);
  EXPECT_EQ(end, PADDED_VALUE);
  EXPECT_EQ(r, NumConversionResult::INVALID_BASE);

  // Repeat some tests with p= resultCode= null to verify toInt64() doesn't
  // attmept to set these values when they are null
  v= strToInt64(ALL_WHITESPACE);
  EXPECT_EQ(v, 0);

  v= strToInt64(BASE10_VALUE);
  EXPECT_EQ(v, 123);

  v= strToInt64(PADDED_VALUE, -1, &end, &r);
  // Invalid base produces undefined result, so test passes if it doesn't
  // segfault.
}

TEST(NumUtilTests, StrToUInt64) {
  const char* const EMPTY_STRING= "";
  const char* const ALL_WHITESPACE= "    \t  \r\n\r\n   ";
  const char* const BASE10_VALUE= "123";
  const char* const BASE16_VALUE= "0x123";
  const char* const BASE8_VALUE= "0123";
  const char* const BASE16_VALUE_NO_PREFIX= "1AF";
  const char* const NEGATIVE_VALUE= "-123";
  const char* const PADDED_VALUE= "   123   ";
  const char* const VALUE_WITH_EXTRA_CHARS = "9876.123";
  const char* const NOT_AN_INTEGER = "  the quick brown fox jumped";
  const char* const OVERFLOWS = "1348742838729873492873472389239890823";
  const char* const UNDERFLOWS = "-1348742838729873492873472389239890823";
  const char* end= nullptr;
  NumConversionResult r;
  uint64_t v= strToUInt64(EMPTY_STRING, 0, &end, &r);

  EXPECT_EQ(v, 0);
  EXPECT_EQ(end, EMPTY_STRING);
  EXPECT_EQ(r, NumConversionResult::EMPTY_STRING);

  v= strToUInt64(ALL_WHITESPACE, 0, &end, &r);
  EXPECT_EQ(v, 0);
  EXPECT_EQ(end, ALL_WHITESPACE+strlen(ALL_WHITESPACE));
  EXPECT_EQ(r, NumConversionResult::EMPTY_STRING);

  v= strToUInt64(BASE10_VALUE, 0, &end, &r);
  EXPECT_EQ(v, 123);
  EXPECT_EQ(end, BASE10_VALUE+strlen(BASE10_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToUInt64(BASE16_VALUE, 0, &end, &r);
  EXPECT_EQ(v, 291);
  EXPECT_EQ(end, BASE16_VALUE+strlen(BASE16_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToUInt64(BASE8_VALUE, 0, &end, &r);
  EXPECT_EQ(v, 83);
  EXPECT_EQ(end, BASE8_VALUE+strlen(BASE8_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToUInt64(BASE10_VALUE, 16, &end, &r);
  EXPECT_EQ(v, 291);
  EXPECT_EQ(end, BASE10_VALUE+strlen(BASE10_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToUInt64(BASE8_VALUE, 10, &end, &r);
  EXPECT_EQ(v, 123);
  EXPECT_EQ(end, BASE8_VALUE+strlen(BASE8_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToUInt64(BASE16_VALUE, 10, &end, &r);
  EXPECT_EQ(v, 0);
  EXPECT_EQ(end, BASE16_VALUE+1);  // Stops at the "x"
  EXPECT_EQ(r, NumConversionResult::OK);

  // Base=0 treats the input as base-10 if it has no prefix
  v= strToUInt64(BASE16_VALUE_NO_PREFIX, 0, &end, &r);
  EXPECT_EQ(v, 1);
  EXPECT_EQ(end, BASE16_VALUE_NO_PREFIX+1);  // Stops at "A"
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToUInt64(BASE16_VALUE_NO_PREFIX, 16, &end, &r);
  EXPECT_EQ(v, 431);
  EXPECT_EQ(end, BASE16_VALUE_NO_PREFIX+strlen(BASE16_VALUE_NO_PREFIX));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToUInt64(NEGATIVE_VALUE, 0, &end, &r);
  EXPECT_EQ(v, 0);
  EXPECT_EQ(end, NEGATIVE_VALUE + strlen(NEGATIVE_VALUE));
  EXPECT_EQ(r, NumConversionResult::UNDERFLOWED);

  v= strToUInt64(PADDED_VALUE, 0, &end, &r);
  EXPECT_EQ(v, 123);
  EXPECT_EQ(end, PADDED_VALUE+6); // Stops at first trailing space
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToUInt64(VALUE_WITH_EXTRA_CHARS, 0, &end, &r);
  EXPECT_EQ(v, 9876);
  EXPECT_EQ(end, VALUE_WITH_EXTRA_CHARS+4);  // Stops at "."
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToUInt64(NOT_AN_INTEGER, 0, &end, &r);
  // Return value is undefined, so don't check it
  EXPECT_EQ(end, NOT_AN_INTEGER+2);  // Initial whitespace skipped
  EXPECT_EQ(r, NumConversionResult::NOT_AN_INTEGER);

  v= strToUInt64(UNDERFLOWS, 0, &end, &r);
  EXPECT_EQ(v, 0);
  EXPECT_EQ(end, UNDERFLOWS+strlen(UNDERFLOWS));
  EXPECT_EQ(r, NumConversionResult::UNDERFLOWED);

  v= strToUInt64(OVERFLOWS, 0, &end, &r);
  EXPECT_EQ(v, UINT64_MAX);
  EXPECT_EQ(end, OVERFLOWS+strlen(OVERFLOWS));
  EXPECT_EQ(r, NumConversionResult::OVERFLOWED);

  // Invalid bases - return value is undefined in these cases, so it is not
  // checked
  v= strToUInt64(PADDED_VALUE, -1, &end, &r);
  EXPECT_EQ(end, PADDED_VALUE);
  EXPECT_EQ(r, NumConversionResult::INVALID_BASE);

  v= strToUInt64(PADDED_VALUE, 1, &end, &r);
  EXPECT_EQ(end, PADDED_VALUE);
  EXPECT_EQ(r, NumConversionResult::INVALID_BASE);

  v= strToUInt64(PADDED_VALUE, 37, &end, &r);
  EXPECT_EQ(end, PADDED_VALUE);
  EXPECT_EQ(r, NumConversionResult::INVALID_BASE);

  // Repeat some tests with p= resultCode= null to verify toInt64() doesn't
  // attmept to set these values when they are null
  v= strToUInt64(ALL_WHITESPACE);
  EXPECT_EQ(v, 0);

  v= strToUInt64(BASE10_VALUE);
  EXPECT_EQ(v, 123);

  v= strToUInt64(PADDED_VALUE, -1, &end, &r);
  // Invalid base produces undefined result, so test passes if it doesn't
  // segfault.
}

TEST(NumUtilTests, StrToDouble) {
  const char* const EMPTY_STRING= "";
  const char* const ALL_WHITESPACE= "    \t  \r\n\r\n   ";
  const char* const BASE10_VALUE= "123.456";
  const char* const NEGATIVE_VALUE= "-123.456";
  const char* const PADDED_VALUE= "   987.65   ";
  const char* const VALUE_WITH_EXTRA_CHARS = "9876.123AZB";
  const char* const NOT_FLOATING_PT = "  the quick brown fox jumped";
  const char* const OVERFLOWS = "1E+10000";
  const char* const UNDERFLOWS = "1E-10000";
  const char* end= nullptr;
  NumConversionResult r;
  double v= strToDouble(EMPTY_STRING, &end, &r);

  EXPECT_EQ(v, 0.0);
  EXPECT_EQ(end, EMPTY_STRING);
  EXPECT_EQ(r, NumConversionResult::EMPTY_STRING);

  v= strToDouble(ALL_WHITESPACE, &end, &r);
  EXPECT_EQ(v, 0.0);
  EXPECT_EQ(end, ALL_WHITESPACE+strlen(ALL_WHITESPACE));
  EXPECT_EQ(r, NumConversionResult::EMPTY_STRING);

  v= strToDouble(BASE10_VALUE, &end, &r);
  EXPECT_NEAR(v, 123.456, 1e-10);
  EXPECT_EQ(end, BASE10_VALUE+strlen(BASE10_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToDouble(NEGATIVE_VALUE, &end, &r);
  EXPECT_NEAR(v, -123.456, 1e-10);
  EXPECT_EQ(end, NEGATIVE_VALUE + strlen(NEGATIVE_VALUE));
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToDouble(PADDED_VALUE, &end, &r);
  EXPECT_NEAR(v, 987.65, 1e-10);
  EXPECT_EQ(end, PADDED_VALUE+9); // Stops at first trailing space
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToDouble(VALUE_WITH_EXTRA_CHARS, &end, &r);
  EXPECT_NEAR(v, 9876.123, 1e-10);
  EXPECT_EQ(end, VALUE_WITH_EXTRA_CHARS+8);  // Stops at "A"
  EXPECT_EQ(r, NumConversionResult::OK);

  v= strToDouble(NOT_FLOATING_PT, &end, &r);
  // Return value is undefined, so don't check it
  EXPECT_EQ(end, NOT_FLOATING_PT+2);  // Initial whitespace skipped
  EXPECT_EQ(r, NumConversionResult::NOT_FLOATING_PT);

  v= strToDouble(UNDERFLOWS, &end, &r);
  EXPECT_EQ(v, 0);
  EXPECT_EQ(end, UNDERFLOWS+strlen(UNDERFLOWS));
  EXPECT_EQ(r, NumConversionResult::UNDERFLOWED);

  v= strToDouble(OVERFLOWS, &end, &r);
  EXPECT_EQ(v, HUGE_VAL);
  EXPECT_EQ(end, OVERFLOWS+strlen(OVERFLOWS));
  EXPECT_EQ(r, NumConversionResult::OVERFLOWED);

  // Repeat some tests with p= resultCode= null to verify toInt64() doesn't
  // attmept to set these values when they are null
  v= strToDouble(ALL_WHITESPACE);
  EXPECT_EQ(v, 0.0);

  v= strToDouble(BASE10_VALUE);
  EXPECT_NEAR(v, 123.456, 1e-10);
}

TEST(NumUtilTests, ToInt64Quietly) {
  const char* const EMPTY_STRING= "";
  const char* const ALL_WHITESPACE= "    \t  \r\n\r\n   ";
  const char* const BASE10_VALUE= "123";
  const char* const BASE16_VALUE= "0x123";
  const char* const BASE8_VALUE= "0123";
  const char* const BASE16_VALUE_NO_PREFIX= "1AF";
  const char* const NEGATIVE_VALUE= "-123";
  const char* const PADDED_VALUE= "   123   ";
  const char* const VALUE_WITH_EXTRA_CHARS = "9876.123";
  const char* const NOT_AN_INTEGER = "  the quick brown fox jumped";
  const char* const OVERFLOWS = "1348742838729873492873472389239890823";
  const char* const UNDERFLOWS = "-1348742838729873492873472389239890823";
  std::pair<int64_t, NumConversionResult> v= toInt64Quietly(EMPTY_STRING, 0);

  EXPECT_EQ(v.first, 0);
  EXPECT_EQ(v.second, NumConversionResult::EMPTY_STRING);

  v= toInt64Quietly(ALL_WHITESPACE, 0);
  EXPECT_EQ(v.first, 0);
  EXPECT_EQ(v.second, NumConversionResult::EMPTY_STRING);

  v= toInt64Quietly(BASE10_VALUE, 0);
  EXPECT_EQ(v.first, 123);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toInt64Quietly(BASE16_VALUE, 0);
  EXPECT_EQ(v.first, 291);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toInt64Quietly(BASE8_VALUE, 0);
  EXPECT_EQ(v.first, 83);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toInt64Quietly(BASE10_VALUE, 16);
  EXPECT_EQ(v.first, 291);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toInt64Quietly(BASE8_VALUE, 10);
  EXPECT_EQ(v.first, 123);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  // "0x123" is not a valid base-10 integer, so result is NOT_AN_INTEGER
  v= toInt64Quietly(BASE16_VALUE, 10);
  // v.first is undefined
  EXPECT_EQ(v.second, NumConversionResult::NOT_AN_INTEGER);

  // Base=0 treats the input as base-10 if it has no prefix.  "1AF" is not
  // a valid base-10 integer, so result is NOT_AN_INTEGER
  v= toInt64Quietly(BASE16_VALUE_NO_PREFIX, 0);
  // v.first is undefined
  EXPECT_EQ(v.second, NumConversionResult::NOT_AN_INTEGER);

  // However, "1AF" is a valid base-16 integer.
  v= toInt64Quietly(BASE16_VALUE_NO_PREFIX, 16);
  EXPECT_EQ(v.first, 431);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toInt64Quietly(NEGATIVE_VALUE, 0);
  EXPECT_EQ(v.first, -123);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toInt64Quietly(PADDED_VALUE, 0);
  EXPECT_EQ(v.first, 123);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  // Extra characters at the end render the number NOT_AN_INTEGER
  v= toInt64Quietly(VALUE_WITH_EXTRA_CHARS, 0);
  // v.first is undefined
  EXPECT_EQ(v.second, NumConversionResult::NOT_AN_INTEGER);

  v= toInt64Quietly(NOT_AN_INTEGER, 0);
  // v.first is undefined, so don't check it
  EXPECT_EQ(v.second, NumConversionResult::NOT_AN_INTEGER);

  v= toInt64Quietly(UNDERFLOWS, 0);
  EXPECT_EQ(v.first, INT64_MIN);
  EXPECT_EQ(v.second, NumConversionResult::UNDERFLOWED);

  v= toInt64Quietly(OVERFLOWS, 0);
  EXPECT_EQ(v.first, INT64_MAX);
  EXPECT_EQ(v.second, NumConversionResult::OVERFLOWED);

  // Invalid bases - v.first is undefined in these cases, so it is not
  // checked
  v= toInt64Quietly(PADDED_VALUE, -1);
  EXPECT_EQ(v.second, NumConversionResult::INVALID_BASE);

  v= toInt64Quietly(PADDED_VALUE, 1);
  EXPECT_EQ(v.second, NumConversionResult::INVALID_BASE);

  v= toInt64Quietly(PADDED_VALUE, 37);
  EXPECT_EQ(v.second, NumConversionResult::INVALID_BASE);
}

TEST(NumUtilTests, ToInt64) {
  const char* const EMPTY_STRING= "";
  const char* const ALL_WHITESPACE= "    \t  \r\n\r\n   ";
  const char* const BASE10_VALUE= "123";
  const char* const BASE16_VALUE= "0x123";
  const char* const BASE8_VALUE= "0123";
  const char* const BASE16_VALUE_NO_PREFIX= "1AF";
  const char* const NEGATIVE_VALUE= "-123";
  const char* const PADDED_VALUE= "   123   ";
  const char* const VALUE_WITH_EXTRA_CHARS = "9876.123";
  const char* const NOT_AN_INTEGER = "  the quick brown fox jumped";
  const char* const OVERFLOWS = "1348742838729873492873472389239890823";
  const char* const UNDERFLOWS = "-1348742838729873492873472389239890823";
  int64_t v;

  EXPECT_THROW(toInt64(EMPTY_STRING, 0), IllegalValueError);
  EXPECT_THROW(toInt64(ALL_WHITESPACE, 0), IllegalValueError);

  v= toInt64(BASE10_VALUE, 0);
  EXPECT_EQ(v, 123);

  v= toInt64(BASE16_VALUE, 0);
  EXPECT_EQ(v, 291);

  v= toInt64(BASE8_VALUE, 0);
  EXPECT_EQ(v, 83);

  v= toInt64(BASE10_VALUE, 16);
  EXPECT_EQ(v, 291);

  v= toInt64(BASE8_VALUE, 10);
  EXPECT_EQ(v, 123);

  // "0x123" is not a valid base-10 integer, so toInt64() throws
  EXPECT_THROW(toInt64(BASE16_VALUE, 10), IllegalValueError);

  // Base=0 treats the input as base-10 if it has no prefix.  "1AF" is not
  // a valid base-10 integer, so toInt64() throws
  EXPECT_THROW(toInt64(BASE16_VALUE_NO_PREFIX, 0), IllegalValueError);

  // However, "1AF" is a valid base-16 integer.
  v= toInt64(BASE16_VALUE_NO_PREFIX, 16);
  EXPECT_EQ(v, 431);

  v= toInt64(NEGATIVE_VALUE, 0);
  EXPECT_EQ(v, -123);

  v= toInt64(PADDED_VALUE, 0);
  EXPECT_EQ(v, 123);

  // Extra characters at the end render the number not an integer and
  // cause toInt64() to throw
  EXPECT_THROW(toInt64(VALUE_WITH_EXTRA_CHARS, 0), IllegalValueError);

  EXPECT_THROW(toInt64(NOT_AN_INTEGER, 0), IllegalValueError);

  // Overflow and underflow both produce IllegalValueErrors
  EXPECT_THROW(toInt64(UNDERFLOWS, 0), IllegalValueError);
  EXPECT_THROW(toInt64(OVERFLOWS, 0), IllegalValueError);

  // Invalid bases - produce an IllegalValueError for the second argument
  EXPECT_THROW(toInt64(PADDED_VALUE, -1), IllegalValueError);
  EXPECT_THROW(toInt64(PADDED_VALUE, 1), IllegalValueError);
  EXPECT_THROW(toInt64(PADDED_VALUE, 37), IllegalValueError);
}

TEST(NumUtilTests, ToUInt64Quietly) {
  const char* const EMPTY_STRING= "";
  const char* const ALL_WHITESPACE= "    \t  \r\n\r\n   ";
  const char* const BASE10_VALUE= "123";
  const char* const BASE16_VALUE= "0x123";
  const char* const BASE8_VALUE= "0123";
  const char* const BASE16_VALUE_NO_PREFIX= "1AF";
  const char* const NEGATIVE_VALUE= "-123";
  const char* const PADDED_VALUE= "   123   ";
  const char* const VALUE_WITH_EXTRA_CHARS = "9876.123";
  const char* const NOT_AN_INTEGER = "  the quick brown fox jumped";
  const char* const OVERFLOWS = "1348742838729873492873472389239890823";
  const char* const UNDERFLOWS = "-1348742838729873492873472389239890823";
  std::pair<uint64_t, NumConversionResult> v= toUInt64Quietly(EMPTY_STRING, 0);

  EXPECT_EQ(v.first, 0);
  EXPECT_EQ(v.second, NumConversionResult::EMPTY_STRING);

  v= toUInt64Quietly(ALL_WHITESPACE, 0);
  EXPECT_EQ(v.first, 0);
  EXPECT_EQ(v.second, NumConversionResult::EMPTY_STRING);

  v= toUInt64Quietly(BASE10_VALUE, 0);
  EXPECT_EQ(v.first, 123);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toUInt64Quietly(BASE16_VALUE, 0);
  EXPECT_EQ(v.first, 291);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toUInt64Quietly(BASE8_VALUE, 0);
  EXPECT_EQ(v.first, 83);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toUInt64Quietly(BASE10_VALUE, 16);
  EXPECT_EQ(v.first, 291);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toUInt64Quietly(BASE8_VALUE, 10);
  EXPECT_EQ(v.first, 123);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  // "0x123" is not a valid base-10 integer, so result is NOT_AN_INTEGER
  v= toUInt64Quietly(BASE16_VALUE, 10);
  // v.first is undefined
  EXPECT_EQ(v.second, NumConversionResult::NOT_AN_INTEGER);

  // Base=0 treats the input as base-10 if it has no prefix.  "1AF" is not
  // a valid base-10 integer, so result is NOT_AN_INTEGER
  v= toUInt64Quietly(BASE16_VALUE_NO_PREFIX, 0);
  // v.first is undefined
  EXPECT_EQ(v.second, NumConversionResult::NOT_AN_INTEGER);

  // However, "1AF" is a valid base-16 integer.
  v= toUInt64Quietly(BASE16_VALUE_NO_PREFIX, 16);
  EXPECT_EQ(v.first, 431);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  // Negative numbers underflow rather than producing NOT_AN_INTEGER
  v= toUInt64Quietly(NEGATIVE_VALUE, 0);
  EXPECT_EQ(v.first, 0);
  EXPECT_EQ(v.second, NumConversionResult::UNDERFLOWED);

  v= toUInt64Quietly(PADDED_VALUE, 0);
  EXPECT_EQ(v.first, 123);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  // Extra characters at the end render the number NOT_AN_INTEGER
  v= toUInt64Quietly(VALUE_WITH_EXTRA_CHARS, 0);
  // v.first is undefined
  EXPECT_EQ(v.second, NumConversionResult::NOT_AN_INTEGER);

  v= toUInt64Quietly(NOT_AN_INTEGER, 0);
  // v.first is undefined, so don't check it
  EXPECT_EQ(v.second, NumConversionResult::NOT_AN_INTEGER);

  v= toUInt64Quietly(UNDERFLOWS, 0);
  EXPECT_EQ(v.first, 0);
  EXPECT_EQ(v.second, NumConversionResult::UNDERFLOWED);

  v= toUInt64Quietly(OVERFLOWS, 0);
  EXPECT_EQ(v.first, UINT64_MAX);
  EXPECT_EQ(v.second, NumConversionResult::OVERFLOWED);

  // Invalid bases - v.first is undefined in these cases, so it is not
  // checked
  v= toUInt64Quietly(PADDED_VALUE, -1);
  EXPECT_EQ(v.second, NumConversionResult::INVALID_BASE);

  v= toUInt64Quietly(PADDED_VALUE, 1);
  EXPECT_EQ(v.second, NumConversionResult::INVALID_BASE);

  v= toUInt64Quietly(PADDED_VALUE, 37);
  EXPECT_EQ(v.second, NumConversionResult::INVALID_BASE);
}

TEST(NumUtilTests, ToUInt64) {
  const char* const EMPTY_STRING= "";
  const char* const ALL_WHITESPACE= "    \t  \r\n\r\n   ";
  const char* const BASE10_VALUE= "123";
  const char* const BASE16_VALUE= "0x123";
  const char* const BASE8_VALUE= "0123";
  const char* const BASE16_VALUE_NO_PREFIX= "1AF";
  const char* const NEGATIVE_VALUE= "-123";
  const char* const PADDED_VALUE= "   123   ";
  const char* const VALUE_WITH_EXTRA_CHARS = "9876.123";
  const char* const NOT_AN_INTEGER = "  the quick brown fox jumped";
  const char* const OVERFLOWS = "1348742838729873492873472389239890823";
  const char* const UNDERFLOWS = "-1348742838729873492873472389239890823";
  int64_t v;

  EXPECT_THROW(toUInt64(EMPTY_STRING, 0), IllegalValueError);
  EXPECT_THROW(toUInt64(ALL_WHITESPACE, 0), IllegalValueError);

  v= toUInt64(BASE10_VALUE, 0);
  EXPECT_EQ(v, 123);

  v= toUInt64(BASE16_VALUE, 0);
  EXPECT_EQ(v, 291);

  v= toUInt64(BASE8_VALUE, 0);
  EXPECT_EQ(v, 83);

  v= toUInt64(BASE10_VALUE, 16);
  EXPECT_EQ(v, 291);

  v= toUInt64(BASE8_VALUE, 10);
  EXPECT_EQ(v, 123);

  // "0x123" is not a valid base-10 integer, so toInt64() throws
  EXPECT_THROW(toUInt64(BASE16_VALUE, 10), IllegalValueError);

  // Base=0 treats the input as base-10 if it has no prefix.  "1AF" is not
  // a valid base-10 integer, so toInt64() throws
  EXPECT_THROW(toUInt64(BASE16_VALUE_NO_PREFIX, 0), IllegalValueError);

  // However, "1AF" is a valid base-16 integer.
  v= toUInt64(BASE16_VALUE_NO_PREFIX, 16);
  EXPECT_EQ(v, 431);

  EXPECT_THROW(toUInt64(NEGATIVE_VALUE, 0), IllegalValueError);

  v= toUInt64(PADDED_VALUE, 0);
  EXPECT_EQ(v, 123);

  // Extra characters at the end render the number not an integer and
  // cause toInt64() to throw
  EXPECT_THROW(toUInt64(VALUE_WITH_EXTRA_CHARS, 0), IllegalValueError);

  EXPECT_THROW(toUInt64(NOT_AN_INTEGER, 0), IllegalValueError);

  // Overflow and underflow both produce IllegalValueErrors
  EXPECT_THROW(toUInt64(UNDERFLOWS, 0), IllegalValueError);
  EXPECT_THROW(toUInt64(OVERFLOWS, 0), IllegalValueError);

  // Invalid bases - produce an IllegalValueError for the second argument
  EXPECT_THROW(toUInt64(PADDED_VALUE, -1), IllegalValueError);
  EXPECT_THROW(toUInt64(PADDED_VALUE, 1), IllegalValueError);
  EXPECT_THROW(toUInt64(PADDED_VALUE, 37), IllegalValueError);
}

TEST(NumUtilTests, ToDoubleQuietly) {
  const char* const EMPTY_STRING= "";
  const char* const ALL_WHITESPACE= "    \t  \r\n\r\n   ";
  const char* const BASE10_VALUE= "123.456";
  const char* const NEGATIVE_VALUE= "-123.456";
  const char* const PADDED_VALUE= "   987.65   ";
  const char* const VALUE_WITH_EXTRA_CHARS = "9876.123AZB";
  const char* const NOT_FLOATING_PT = "  the quick brown fox jumped";
  const char* const OVERFLOWS = "1E+10000";
  const char* const UNDERFLOWS = "1E-10000";
  const char* end= nullptr;
  std::pair<double, NumConversionResult> v= toDoubleQuietly(EMPTY_STRING);

  EXPECT_EQ(v.first, 0.0);
  EXPECT_EQ(v.second, NumConversionResult::EMPTY_STRING);

  v= toDoubleQuietly(ALL_WHITESPACE);
  EXPECT_EQ(v.first, 0.0);
  EXPECT_EQ(v.second, NumConversionResult::EMPTY_STRING);

  v= toDoubleQuietly(BASE10_VALUE);
  EXPECT_NEAR(v.first, 123.456, 1e-10);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toDoubleQuietly(NEGATIVE_VALUE);
  EXPECT_NEAR(v.first, -123.456, 1e-10);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  v= toDoubleQuietly(PADDED_VALUE);
  EXPECT_NEAR(v.first, 987.65, 1e-10);
  EXPECT_EQ(v.second, NumConversionResult::OK);

  // Extra chars past the end of the number render it invalid
  v= toDoubleQuietly(VALUE_WITH_EXTRA_CHARS);
  // v.first is undefined
  EXPECT_EQ(v.second, NumConversionResult::NOT_FLOATING_PT);

  v= toDoubleQuietly(NOT_FLOATING_PT);
  // v.first is undefined
  EXPECT_EQ(v.second, NumConversionResult::NOT_FLOATING_PT);

  v= toDoubleQuietly(UNDERFLOWS);
  EXPECT_EQ(v.first, 0);
  EXPECT_EQ(v.second, NumConversionResult::UNDERFLOWED);

  v= toDoubleQuietly(OVERFLOWS);
  EXPECT_EQ(v.first, HUGE_VAL);
  EXPECT_EQ(v.second, NumConversionResult::OVERFLOWED);
}

TEST(NumUtilTests, ToDouble) {
  const char* const EMPTY_STRING= "";
  const char* const ALL_WHITESPACE= "    \t  \r\n\r\n   ";
  const char* const BASE10_VALUE= "123.456";
  const char* const NEGATIVE_VALUE= "-123.456";
  const char* const PADDED_VALUE= "   987.65   ";
  const char* const VALUE_WITH_EXTRA_CHARS = "9876.123AZB";
  const char* const NOT_FLOATING_PT = "  the quick brown fox jumped";
  const char* const OVERFLOWS = "1E+10000";
  const char* const UNDERFLOWS = "1E-10000";
  const char* end= nullptr;
  double v;

  EXPECT_THROW(toDouble(EMPTY_STRING), IllegalValueError);
  EXPECT_THROW(toDouble(ALL_WHITESPACE), IllegalValueError);

  v= toDouble(BASE10_VALUE);
  EXPECT_NEAR(v, 123.456, 1e-10);

  v= toDouble(NEGATIVE_VALUE);
  EXPECT_NEAR(v, -123.456, 1e-10);

  v= toDouble(PADDED_VALUE);
  EXPECT_NEAR(v, 987.65, 1e-10);

  // Extra chars past the end of the number render it invalid
  EXPECT_THROW(toDouble(VALUE_WITH_EXTRA_CHARS), IllegalValueError);

  EXPECT_THROW(toDouble(NOT_FLOATING_PT), IllegalValueError);

  EXPECT_THROW(toDouble(UNDERFLOWS), IllegalValueError);
  EXPECT_THROW(toDouble(OVERFLOWS), IllegalValueError);
}
