#ifndef __PISTIS__CORE__TYPES__NUMUTIL_HPP__
#define __PISTIS__CORE__TYPES__NUMUTIL_HPP__

/** @file NumUtil.hpp
 *
 *  Utilities for working with the primitive integer (short, int, long,
 *  int64_t and their unsigned counterparts) and primitive floating-point
 *  types (float, double and long double).
 */

#include <pistis/util/NumConversionResult.hpp>

namespace pistis {
  namespace util {

    /** @brief Convert a string to a 64-bit signed integer.
     *
     *  The @i strToInt64 function is essentially a replacement for
     *  @i strtoll that is (a) guaranteed to convert its argument to
     *  a 64-bit integer, and (b) returns an optional result code indicating
     *  why the conversion failed that can easily be interpreted and
     *  tranformed to a legible error message.
     *
     *  Much like @i strtoll, @c strToInt64 skips leading whitespace
     *  and then tries to read a number written in digits of the given
     *  @i base, with an optional leading "+" or "-" sign.  If @i base is 0
     *  or 16, the "+" or "-" sign may be followed by an optional "0x"
     *  prefix.  Reading stops either at the end of the string or at the
     *  first unparsable character, and @i strToInt64 returns the value of
     *  the number read so far.  If @i p is not null, @i strToInt64 will
     *  set it to the location in @i v where parsing stopped.  If @i
     *  resultCode is not null, then it will be set to a value that 
     *  indicates the outcome of the conversion.  The following list
     *  enumerates the different parsing outcomes and the corresponding
     *  return value from @i strToInt64 and value of @i resultCode:
     *  <ul>
     *    <li>If one or more digits are read, and the value of those
     *        digits fits in a signed 64-bit integer, @i strToInt64
     *        returns the value of that integer and sets @i resultCode
     *        to NumConversionResult::OK.</li>
     *    <li>If one or more digits are read, and the value of those
     *        digits overflows a signed 64-bit integer, @i strToInt64
     *        returns INT64_MAX and sets @i resultCode to
     *        NumConversionResult::OVERFLOW.</li>
     *    <li>If one or more digits are read, and the value of those
     *        digits underflows a signed 64-bit integer, @i strToInt64
     *        returns INT64_MIN and sets @i resultCode to
     *        NumConversionResult::UNDERFLOW.</li>
     *    <li>If @i v is empty or consists of all whitespace characters,
     *        @i strToInt64 returns 0 and sets @i resultCode to
     *        NumConversionResult::EMPTY_STRING.</li>
     *    <li>If base is less than zero, 1, or greater than 36, the return
     *        value of @i strToInt64 is undefined and @i result code
     *        is set to NumConversionResult::INVALID_BASE.</li>
     *    <li>Otherwise, the return value of @i strToInt64 is undefined
     *        and @i resultCode is set to
     *        NumConversionResult::NOT_AN_INTEGER</li>.
     *  </ul>
     *  
     *  The @i base must be between 2 and 36 inclusive, or the special value
     *  zero.  If @i base is 0, then the number is read in base-16 if
     *  it begins with a "0x" prefix, base-8 if it begins with a "0" prefix,
     *  and base-10 otherwise.
     *
     *  @param v  The string to convert.
     *  @param base  The base used to write @i v; must be between 2 and 36
     *                 (inclusive) or the special value 0.
     *  @param p     If not null, upon return, @i p will contain the
     *                 location in @i v where parsing stopped.
     *  @param resultCode If not null, upon return, will contain a code
     *                      indicating the outcome of the conversion.
     *  @returns  The value of the converted number.
     *  @throws  Does not throw
     */
    int64_t strToInt64(const char* v, int base=0, const char** p= nullptr,
		       NumConversionResult* resultCode= nullptr);

    /** @brief Convert a string to a 64-bit unsigned integer.
     *
     *  The @i strToUInt64 function is essentially a replacement for
     *  @i strtoull that is (a) guaranteed to convert its argument to
     *  a 64-bit integer, and (b) returns an optional result code indicating
     *  why the conversion failed that can easily be interpreted and
     *  tranformed to a legible error message.
     *
     *  Much like @i strtoull, @c strToInt64 skips leading whitespace
     *  and then tries to read a number written in digits of the given
     *  @i base, with an optional leading "+" or "-" sign.  If @i base is 0
     *  or 16, the "+" or "-" sign may be followed by an optional "0x"
     *  prefix.  Reading stops either at the end of the string or at the
     *  first unparsable character, and @i strToInt64 returns the value of
     *  the number read so far.  If @i p is not null, @i strToInt64 will
     *  set it to the location in @i v where parsing stopped.  If @i
     *  resultCode is not null, then it will be set to a value that 
     *  indicates the outcome of the conversion.  The following list
     *  enumerates the different parsing outcomes and the corresponding
     *  return value from @i strToInt64 and value of @i resultCode:
     *  <ul>
     *    <li>If one or more digits are read, and the value of those
     *        digits is non-negative and fits in an unsigned 64-bit integer,
     *        @i strToUInt64 returns the value of that integer and sets
     *        @i resultCode to NumConversionResult::OK.</li>
     *    <li>If one or more digits are read, and the value of those
     *        digits overflows an unsigned 64-bit integer, @i strToUInt64
     *        returns UINT64_MAX and sets @i resultCode to
     *        NumConversionResult::OVERFLOW.</li>
     *    <li>If the number is negative, @i strToInt64 returns 0 and sets
     *        @i resultCode to NumConversionResult::UNDERFLOW.  Note
     *        that this behavior is different from @i strtoull, which
     *        will convert a negagtive number to its unsigned equivalent.
     *        Note however that @i strToUInt64 will still read the
     *        digits of the negative number, so @i p will point past the
     *        last digit read rather than to the negative sign.</li>
     *    <li>If @i v is empty or consists of all whitespace characters,
     *        @i strToUInt64 returns 0 and sets @i resultCode to
     *        NumConversionResult::EMPTY_STRING.</li>
     *    <li>If base is less than zero, 1, or greater than 36, the return
     *        value of @i strToUInt64 is undefined and @i result code
     *        is set to NumConversionResult::INVALID_BASE.</li>
     *    <li>Otherwise, the return value of @i strToUInt64 is undefined
     *        and @i resultCode is set to
     *        NumConversionResult::NOT_AN_INTEGER</li>.
     *  </ul>
     *  
     *  The @i base must be between 2 and 36 inclusive, or the special value
     *  zero.  If @i base is 0, then the number is read in base-16 if
     *  it begins with a "0x" prefix, base-8 if it begins with a "0" prefix,
     *  and base-10 otherwise.
     *
     *  @param v  The string to convert.
     *  @param base  The base used to write @i v; must be between 2 and 36
     *                 (inclusive) or the special value 0.
     *  @param p     If not null, upon return, @i p will contain the
     *                 location in @i v where parsing stopped.
     *  @param resultCode If not null, upon return, will contain a code
     *                      indicating the outcome of the conversion.
     *  @returns The value of the converted number.
     *  @throws Does not throw
     */
    uint64_t strToUInt64(const char* v, int base=0, const char** p= nullptr,
			 NumConversionResult* resultCode= nullptr);

    /** @brief Convert a string to a double
     *
     *  The @i strToDouble function is a wrapper around @i strtod that
     *  provides an optional result code that explains what went wrong
     *  in the event of an error and can easily be transformed to a legible
     *  error message.  The @i strToDouble function follows the same
     *  conversion rules as @i strtod.  The return value and result code
     *  are set as follows:
     *  <ul>
     *    <li>If the input could be converted to a double without underflow
     *        or overflow, @i strToDouble returns the value of the input
     *        and sets the result code to NumConversionResult::OK.</li>
     *    <li>If overflow occured during conversion, @i strToDouble returns
     *        either HUGE_VAL or -HUGE_VAL and sets the result code to
     *        NumConversionResult::OVERFLOW.</li>
     *    <li>If underflow occurred during conversion, @i strToDouble
     *        returns 0 and sets the result code to
     *        NumConversionResult::UNDERFLOW.</li>
     *    <li>If the input is the empty string or all whitespace,
     *        @i strToDouble returns 0 and sets the result code to
     *        NumConversionResult::EMPTY_STRING.</li>
     *    <li>Otherwise, @i strToDouble failed to convert any characters
     *        in the input to a double, and so it sets result code
     *        to NumConversionResult::NOT_FLOATING_PT.  The return value
     *        in this case is undefined.</li>
     *  </ul>
     *
     *  @param v  The string to convert.
     *  @param p     If not null, upon return, @i p will contain the
     *                 location in @i v where parsing stopped.
     *  @param resultCode If not null, upon return, will contain a code
     *                      indicating the outcome of the conversion.
     *  @returns The value of the converted number.
     *  @throws Does not throw
     */
    double strToDouble(const char* v, const char**p= nullptr,
		       NumConversionResult* resultCode= nullptr);

    /** @brief Convert a string to a 64-bit signed integer reliably.
     *
     *  "Reliable" conversion means that the string is a valid representation
     *  of a 64-bit integer in the supplied base.  The input string must
     *  be an integer (but may have leading and trailing whitespace)
     *  representable in 64-bits without underflow or overflow and written
     *  in digits of the base indicated by the @i base argument with an
     *  optional leading "+" or "-" sign (and an optional "0x" or "0" prefix,
     *  depending on the value of @i base; see below).  If these conditions
     *  are violated, @i toInt64Quietly will return a NumConversionResult
     *  code indicating an error.  If this happens, the converted value
     *  returned to the caller will be as follows:
     *  <ul>
     *    <li>If underflow occurs, the converted value will be INT64_MIN</li>
     *    <li>If overflow occurs, the converted value will be INT64_MAX</li>
     *    <li>If @c v is empty or all-whitespace, the converted value
     *          will be zero</li>
     *    <li>Otherwise, the converted value will be undefined.</li>
     *  </ul>
     *
     *  The @i base must be between 2 and 36 inclusive, or the special value
     *  zero.  If @i base is zero, then the number is read in base-16 if
     *  it begins with a "0x" prefix, base-8 if it begins with a "0" prefix,
     *  and base-10 otherwise.  If @i base is 16, then the number may begin
     *  with a "0x" prefix, which will be ignored.
     *
     *  @param v  The value to convert
     *  @param base  Base for the number.  Must be between 2 and 36, or
     *                 the special value zero.
     *  @returns The converted value and a NumConversionResult indicating
     *             the outcome of the conversion.
     *  @throws  Does not throw
     *  @see toInt64Quietly(const std::string&, int)
     *  @see toInt64(const char*, int)
     *  @see toUInt64Quietly(const char*, int)
     */
    std::pair<int64_t, NumConversionResult> toInt64Quietly(const char* v,
							   int base=10);

    /** @brief Convert a string to a 64-bit signed integer reliably.
     *
     *  "Reliable" conversion means that the string is a valid representation
     *  of a 64-bit integer in the supplied base.  The input string must
     *  be an integer (but may have leading and trailing whitespace)
     *  representable in 64-bits without underflow or overflow and written
     *  in digits of the base indicated by the @i base argument with an
     *  optional leading "+" or "-" sign (and an optional "0x" or "0" prefix,
     *  depending on the value of @i base; see below).  If these conditions
     *  are violated, @i toInt64Quietly will return a NumConversionResult
     *  code indicating an error.  If this happens, the converted value
     *  returned to the caller will be as follows:
     *  <ul>
     *    <li>If underflow occurs, the converted value will be INT64_MIN</li>
     *    <li>If overflow occurs, the converted value will be INT64_MAX</li>
     *    <li>If @c v is empty or all-whitespace, the converted value
     *          will be zero</li>
     *    <li>Otherwise, the converted value will be undefined.</li>
     *  </ul>
     *
     *  The @i base must be between 2 and 36 inclusive, or the special value
     *  zero.  If @i base is zero, then the number is read in base-16 if
     *  it begins with a "0x" prefix, base-8 if it begins with a "0" prefix,
     *  and base-10 otherwise.  If @i base is 16, then the number may begin
     *  with a "0x" prefix, which will be ignored.
     *
     *  @param v  The value to convert
     *  @param base  Base for the number.  Must be between 2 and 36, or
     *                 the special value zero.
     *  @returns The converted value and a NumConversionResult indicating
     *             the outcome of the conversion.
     *  @throws  Does not throw
     *  @see toInt64Quietly(const char*, int)
     *  @see toInt64(const std::string&, int)
     *  @see toUInt64Quietly(const std::string&, int)
     */
    inline std::pair<int64_t, NumConversionResult> toInt64Quietly(
        const std::string& v, int base=10
    ) {
      return toInt64Quietly(v.c_str(), base);
    }

    /** @brief Convert a string to a 64-bit signed integer reliably.
     *
     *  "Reliable" conversion means that the string is a valid representation
     *  of a 64-bit integer in the supplied base.  The input string must
     *  be an integer (but may have leading and trailing whitespace)
     *  representable in 64-bits without underflow or overflow and written
     *  in digits of the base indicated by the @i base argument with an
     *  optional leading "+" or "-" sign (and an optional "0x" or "0" prefix,
     *  depending on the value of @i base; see below).  If these conditions
     *  are violated, @i toInt64 will throw an IllegalValueException.
     *
     *  The @i base must be between 2 and 36 inclusive, or the special value
     *  zero.  If @i base is zero, then the number is read in base-16 if
     *  it begins with a "0x" prefix, base-8 if it begins with a "0" prefix,
     *  and base-10 otherwise.  If @i base is 16, then the number may begin
     *  with a "0x" prefix, which will be ignored.
     *
     *  @param v  The value to convert as a null-terminated string.
     *  @param base  Base for the number.  Must be between 2 and 36, or
     *                 the special value zero.
     *  @returns The converted value and a NumConversionResult indicating
     *             the outcome of the conversion.
     *  @throws  IllegalValueError if the number is empty, not a number in
     *             the specified base, or is not representable using a
     *             64-bit integer.
     *  @see toInt64Quietly(const char*, int)
     *  @see toInt64(const std::string&, int)
     *  @see toUInt64(const char*, int)
     */
    int64_t toInt64(const char* v, int base=10);

    /** @brief Convert a string to a 64-bit signed integer reliably.
     *
     *  "Reliable" conversion means that the string is a valid representation
     *  of a 64-bit integer in the supplied base.  The input string must
     *  be an integer (but may have leading and trailing whitespace)
     *  representable in 64-bits without underflow or overflow and written
     *  in digits of the base indicated by the @i base argument with an
     *  optional leading "+" or "-" sign (and an optional "0x" or "0" prefix,
     *  depending on the value of @i base; see below).  If these conditions
     *  are violated, @i toInt64 will throw an IllegalValueException.
     *
     *  The @i base must be between 2 and 36 inclusive, or the special value
     *  zero.  If @i base is zero, then the number is read in base-16 if
     *  it begins with a "0x" prefix, base-8 if it begins with a "0" prefix,
     *  and base-10 otherwise.  If @i base is 16, then the number may begin
     *  with a "0x" prefix, which will be ignored.
     *
     *  @param v  The value to convert
     *  @param base  Base for the number.  Must be between 2 and 36, or
     *                 the special value zero.
     *  @returns The converted value and a NumConversionResult indicating
     *             the outcome of the conversion.
     *  @throws  IllegalValueError if the number is empty, not a number in
     *             the specified base, or is not representable using a
     *             64-bit integer.
     *  @see toInt64Quietly(const std::string&, int)
     *  @see toInt64(const char*, int)
     *  @see toUInt64(const std::string&, int)
     */
    inline int64_t toInt64(const std::string& v, int base) {
      return toInt64(v.c_str(), base);
    }

    /** @brief Convert a string to a 64-bit signed integer reliably.
     *
     *  "Reliable" conversion means that the string is a valid representation
     *  of a 64-bit integer in the supplied base.  The input string must
     *  be an integer (but may have leading and trailing whitespace)
     *  representable in 64-bits without underflow or overflow and written
     *  in digits of the base indicated by the @i base argument with an
     *  optional leading "+" or "-" sign (and an optional "0x" or "0" prefix,
     *  depending on the value of @i base; see below).  If these conditions
     *  are violated, @i toInt64Quietly will return a NumConversionResult
     *  code indicating an error.  If this happens, the converted value
     *  returned to the caller will be as follows:
     *  <ul>
     *    <li>If overflow occurs, the converted value will be UINT64_MAX</li>
     *    <li>If @c v is negative, the converted value will be 0</li>
     *    <li>If @c v is empty or all-whitespace, the converted value
     *          will also be 0</li>
     *    <li>Otherwise, the converted value will be undefined.</li>
     *  </ul>
     *
     *  The @i base must be between 2 and 36 inclusive, or the special value
     *  zero.  If @i base is zero, then the number is read in base-16 if
     *  it begins with a "0x" prefix, base-8 if it begins with a "0" prefix,
     *  and base-10 otherwise.  If @i base is 16, then the number may begin
     *  with a "0x" prefix, which will be ignored.
     *
     *  @param v  The value to convert as a null-terminated string
     *  @param base  Base for the number.  Must be between 2 and 36, or
     *                 the special value zero.
     *  @returns The converted value and a NumConversionResult indicating
     *             the outcome of the conversion.
     *  @throws  Does not throw
     *  @see toUInt64Quietly(const std::string&, int)
     *  @see toUInt64(const char*, int)
     *  @see toInt64Quietly(const char*, int)
     */
    std::pair<uint64_t, NumConversionResult> toUInt64Quietly(
        const char* v, int base=0
    );

    /** @brief Convert a string to a 64-bit unsigned integer reliably.
     *
     *  "Reliable" conversion means that the string is a valid representation
     *  of a 64-bit integer in the supplied base.  The input string must
     *  be an integer (but may have leading and trailing whitespace)
     *  representable in 64-bits without underflow or overflow and written
     *  in digits of the base indicated by the @i base argument with an
     *  optional leading "+" or "-" sign (and an optional "0x" or "0" prefix,
     *  depending on the value of @i base; see below).  If these conditions
     *  are violated, @i toInt64Quietly will return a NumConversionResult
     *  code indicating an error.  If this happens, the converted value
     *  returned to the caller will be as follows:
     *  <ul>
     *    <li>If overflow occurs, the converted value will be UINT64_MAX</li>
     *    <li>If @c v is negative, the converted value will be 0</li>
     *    <li>If @c v is empty or all-whitespace, the converted value
     *          will also be 0</li>
     *    <li>Otherwise, the converted value will be undefined.</li>
     *  </ul>
     *
     *  The @i base must be between 2 and 36 inclusive, or the special value
     *  zero.  If @i base is zero, then the number is read in base-16 if
     *  it begins with a "0x" prefix, base-8 if it begins with a "0" prefix,
     *  and base-10 otherwise.  If @i base is 16, then the number may begin
     *  with a "0x" prefix, which will be ignored.
     *
     *  @param v  The value to convert.
     *  @param base  Base for the number.  Must be between 2 and 36, or
     *                 the special value zero.
     *  @returns The converted value and a NumConversionResult indicating
     *             the outcome of the conversion.
     *  @throws  Does not throw
     *  @see toUInt64Quietly(const char*, int)
     *  @see toUInt64(const std::string&, int)
     *  @see toInt64Quietly(const std::string&, int)
     */
    inline std::pair<uint64_t, NumConversionResult> toUInt64Quietly(
        const std::string& v, int base=0
    ) {
      return toUInt64Quietly(v.c_str(), base);
    }

    /** @brief Convert a string to a 64-bit unsigned integer reliably.
     *
     *  "Reliable" conversion means that the string is a valid representation
     *  of a 64-bit integer in the supplied base.  The input string must
     *  be an integer (but may have leading and trailing whitespace)
     *  representable in 64-bits without underflow or overflow and written
     *  in digits of the base indicated by the @i base argument with an
     *  optional leading "+" or "-" sign (and an optional "0x" or "0" prefix,
     *  depending on the value of @i base; see below).  If these conditions
     *  are violated, @i toUInt64 will throw an IllegalValueError
     *  exception.
     *
     *  The @i base must be between 2 and 36 inclusive, or the special value
     *  zero.  If @i base is zero, then the number is read in base-16 if
     *  it begins with a "0x" prefix, base-8 if it begins with a "0" prefix,
     *  and base-10 otherwise.  If @i base is 16, then the number may begin
     *  with a "0x" prefix, which will be ignored.
     *
     *  @param v  The value to convert, as a null-terminated string.
     *  @param base  Base for the number.  Must be between 2 and 36, or
     *                 the special value zero.
     *  @returns The converted value and a NumConversionResult indicating
     *             the outcome of the conversion.
     *  @throws  IllegalValueError if the number is empty, not a number in
     *             the specified base, or is not representable using a
     *             64-bit unsigned integer.
     *  @see toUInt64Quietly(const char*, int)
     *  @see toUInt64(const std::string&, int)
     *  @see toInt64(const char*, int)
     */
    uint64_t toUInt64(const char* v, int base=0);

    /** @brief Convert a string to a 64-bit unsigned integer reliably.
     *
     *  "Reliable" conversion means that the string is a valid representation
     *  of a 64-bit integer in the supplied base.  The input string must
     *  be an integer (but may have leading and trailing whitespace)
     *  representable in 64-bits without underflow or overflow and written
     *  in digits of the base indicated by the @i base argument with an
     *  optional leading "+" or "-" sign (and an optional "0x" or "0" prefix,
     *  depending on the value of @i base; see below).  If these conditions
     *  are violated, @i toUInt64 will throw an IllegalValueError
     *  exception.
     *
     *  The @i base must be between 2 and 36 inclusive, or the special value
     *  zero.  If @i base is zero, then the number is read in base-16 if
     *  it begins with a "0x" prefix, base-8 if it begins with a "0" prefix,
     *  and base-10 otherwise.  If @i base is 16, then the number may begin
     *  with a "0x" prefix, which will be ignored.
     *
     *  @param v  The value to convert.
     *  @param base  Base for the number.  Must be between 2 and 36, or
     *                 the special value zero.
     *  @returns The converted value and a NumConversionResult indicating
     *             the outcome of the conversion.
     *  @throws  IllegalValueError if the number is empty, not a number in
     *             the specified base, or is not representable using a
     *             64-bit unsigned integer.
     *  @see toUInt64Quietly(const std::string&, int)
     *  @see toUInt64(const char*, int)
     *  @see toInt64(const std::string&, int)
     */
    inline uint64_t toUInt64(const std::string& v, int base=0) {
      return toUInt64(v.c_str(), base);
    }

    /** @brief Convert a string to a double
     *
     *  Converts a string to a double by calling @i strToDouble, verifying
     *  that the rest of the string after parsing stops is empty or
     *  whitespace and  assembling the result into a (converted value,
     *  result code) pair.  Result codes follow the same rules as
     *  @i strToDouble, except that the entire string must be a valid
     *  floating-point number (with optional leading and trailing
     *  whitespace).  If the string contains non-whitespace characters
     *  after the floating-point number, @i toDoubleQuietly will set
     *  the result code to NumConversionResult::NOT_FLOATING_PT and its
     *  return value will be undefined.
     *
     *  @param v  The value to convert
     *  @returns  The converted value and a result code indicating whether
     *              the conversion succeeded and how it failed if it did
     *              not succeed.
     *  @throws   Does not throw
     *  @see strToDouble(const char*, const char**, NumConversionResult*)
     */
    std::pair<double, NumConversionResult> toDoubleQuietly(const char* v);

    /** @brief Convert a string to a double
     *
     *  Converts a string to a double by calling @i strToDouble, verifying
     *  that the rest of the string after parsing stops is empty or
     *  whitespace and  assembling the result into a (converted value,
     *  result code) pair.  Result codes follow the same rules as
     *  @i strToDouble, except that the entire string must be a valid
     *  floating-point number (with optional leading and trailing
     *  whitespace).  If the string contains non-whitespace characters
     *  after the floating-point number, @i toDoubleQuietly will set
     *  the result code to NumConversionResult::NOT_FLOATING_PT and its
     *  return value will be undefined.
     *
     *  @param v  The value to convert
     *  @returns  The converted value and a result code indicating whether
     *              the conversion succeeded and how it failed if it did
     *              not succeed.
     *  @throws   Does not throw
     *  @see strToDouble(const char*, const char**, NumConversionResult*)
     */
    inline std::pair<double, NumConversionResult> toDoubleQuietly(
        const std::string& v
    ) {
      return toDoubleQuietly(v.c_str());
    }

    /** @brief Convert a string to a double
     *
     *  Converts a string to a double by calling @i strToDouble, verifying
     *  that the rest of the string after parsing stops is empty or
     *  whitespace, and assembling the result into a (converted value,
     *  result code) pair.  Throws IllegalValueError if the string is empty,
     *  underflow or overflow occurs, the string cannot be converted to
     *  a double, or the string contains extra characters after the first
     *  floating-point value.
     *
     *  @param v  The value to convert
     *  @returns  The converted value and a result code indicating whether
     *              the conversion succeeded and how it failed if it did
     *              not succeed.
     *  @throws   IllegalValueError if the string is empty, does not
     *              represent a floating-point number, or underflow or
     *              overflow occurs.
     *  @see strToDouble(const char*, const char**, NumConversionResult*)
     */
    double toDouble(const char* v);

    /** @brief Convert a string to a double
     *
     *  Converts a string to a double by calling @i strToDouble, verifying
     *  that the rest of the string after parsing stops is empty or
     *  whitespace, and assembling the result into a (converted value,
     *  result code) pair.  Throws IllegalValueError if the string is empty,
     *  underflow or overflow occurs, the string cannot be converted to
     *  a double, or the string contains extra characters after the first
     *  floating-point value.
     *
     *  @param v  The value to convert
     *  @returns  The converted value and a result code indicating whether
     *              the conversion succeeded and how it failed if it did
     *              not succeed.
     *  @throws   IllegalValueError if the string is empty, does not
     *              represent a floating-point number, or underflow or
     *              overflow occurs.
     *  @see strToDouble(const char*, const char**, NumConversionResult*)
     */
    inline double toDouble(const std::string& v) {
      return toDouble(v.c_str());
    }

  }
}

#endif
