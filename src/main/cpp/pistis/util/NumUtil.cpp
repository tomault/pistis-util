#include <pistis/util/NumUtil.hpp>
#include <pistis/exceptions/IllegalValueError.hpp>
#include <sstream>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

using namespace pistis::exceptions;
using namespace pistis::util;

static inline const char* skipWhitespace(const char* p) {
  while (*p && isspace(*p)) {
    ++p;
  }
  return p;
}

static inline bool restIsWhitespace(const char* p) {
  return !*skipWhitespace(p);
}

static bool checkBase(const char* v, int base, const char **p,
		      NumConversionResult* resultCode) {
  if (!base || ((base >= 2) && (base <= 36))) {
    return true;
  }
  if (p) {
    *p= v;
  }
  if (resultCode) {
    *resultCode= NumConversionResult::INVALID_BASE;
  }
  return false;
}

static const char* checkForEmptyString(const char* v, const char** p,
				       NumConversionResult* resultCode) {
  v= skipWhitespace(v);
  if (!*v) {
    if (p) {
      *p= v;
    }
    if (resultCode) {
      *resultCode= NumConversionResult::EMPTY_STRING;
    }
    return nullptr;
  }
  return v;
}

static void formatIllegalValue(std::ostream& out, const char* v) {
  // Only print up to 30 chars from the value to prevent spew
  const char* end= v;
  for (int i=0;i<30 && *end; ++i, ++end);
  out << "\"";
  out.write(v, end-v);
  if (*end) {
    out << "...";
  }
  out << "\"";
}

int64_t pistis::util::strToInt64(const char* v, int base, const char** p,
				 NumConversionResult* resultCode) {
  const char* pp= v;
  char* end;
  int64_t n;

  if (!checkBase(v, base, p, resultCode)) {
    return 0;
  }

  pp= checkForEmptyString(v, p, resultCode);
  if (!pp) {
    return 0;
  }

  errno= 0;
  /** @todo  Implement this in a more-portable way */
#if __WORDSIZE == 64
  n= strtol(pp, &end, base);
#else
  n= strtoll(pp, &end, base);
#endif

  if (!resultCode) {
    // Can skip checks to set the result code
  } else if (end == pp) {
    // No digits were consumed => not an integer
    *resultCode = NumConversionResult::NOT_AN_INTEGER;
  } else if (errno == ERANGE) {
    *resultCode = (n < 0) ? NumConversionResult::UNDERFLOWED
                          : NumConversionResult::OVERFLOWED;
  } else {
    *resultCode= NumConversionResult::OK;
  }
  if (p) {
    *p= end;
  }
  return n;
}

uint64_t pistis::util::strToUInt64(const char* v, int base, const char** p,
				   NumConversionResult* resultCode) {
  const char* pp;
  char* end;
  uint64_t n;

  if (!checkBase(v, base, p, resultCode)) {
    return 0;
  }

  pp= checkForEmptyString(v, p, resultCode);
  if (!pp) {
    return 0;
  }

  errno= 0;
  /** @todo  Implement this in a more-portable way */
#if __WORDSIZE == 64
  n= strtoul(pp, &end, base);
#else
  n= strtoull(pp, &end, base);
#endif

  if (*pp == '-') {
    // Underflow.  We let strtoul(l) do its work so it would place end
    // correctly.  Set the return value to 0 and resultCode if needed
    if (resultCode) {
      *resultCode= NumConversionResult::UNDERFLOWED;
    }
    n= 0;
  } else if (!resultCode) {
    // Can skip checks to set the result code
  } else if (end == pp) {
    // No digits were consumed => not an integer
    *resultCode= NumConversionResult::NOT_AN_INTEGER;
  } else if (errno == ERANGE) {
    *resultCode= NumConversionResult::OVERFLOWED;
  } else {
    *resultCode= NumConversionResult::OK;
  }
  if (p) {
    *p= end;
  }
  return n;
}

double pistis::util::strToDouble(const char* v, const char** p,
				 NumConversionResult* resultCode) {
  const char* pp;
  char* end;
  double n;

  pp= checkForEmptyString(v, p, resultCode);
  if (!pp) {
    return 0.0;
  }

  errno= 0;
  n= strtod(pp, &end);

  if (!resultCode) {
    // Can skip checks to set the result code
  } else if (end == pp) {
    // No digits were consumed => not an integer
    *resultCode = NumConversionResult::NOT_FLOATING_PT;
  } else if (errno == ERANGE) {
    *resultCode = ((n > -1.0) && (n < 1.0)) ? NumConversionResult::UNDERFLOWED
                                            : NumConversionResult::OVERFLOWED;
  } else {
    *resultCode= NumConversionResult::OK;
  }
  if (p) {
    *p= end;
  }
  return n;
}

std::pair<int64_t, NumConversionResult>
    pistis::util::toInt64Quietly(const char* v, int base) {
  const char* p;
  NumConversionResult r;
  int64_t n= strToInt64(v, base, &p, &r);
  if ((r == NumConversionResult::OK) && !restIsWhitespace(p)) {
    r= NumConversionResult::NOT_AN_INTEGER;
  }
  return std::make_pair(n, r);
}

int64_t pistis::util::toInt64(const char* v, int base) {
  std::pair<int64_t, NumConversionResult> r(toInt64Quietly(v, base));
  if (r.second == NumConversionResult::INVALID_BASE) {
    std::ostringstream msg;
    msg << base << " is not a valid base";
    throw IllegalValueError(msg.str(), PISTIS_EX_HERE);
  } else if (r.second != NumConversionResult::OK) {
    std::ostringstream msg;
    formatIllegalValue(msg, v);
    msg << " is not a signed 64-bit integer (" << descriptionFor(r.second)
	<< ")";
    throw IllegalValueError(msg.str(), PISTIS_EX_HERE);
  }
  return r.first;
}

std::pair<uint64_t, NumConversionResult>
    pistis::util::toUInt64Quietly(const char* v, int base) {
  const char* p;
  NumConversionResult r;
  uint64_t n= strToUInt64(v, base, &p, &r);
  if ((r == NumConversionResult::OK) && !restIsWhitespace(p)) {
    r= NumConversionResult::NOT_AN_INTEGER;
  }
  return std::make_pair(n, r);
}

uint64_t pistis::util::toUInt64(const char* v, int base) {
  std::pair<int64_t, NumConversionResult> r(toUInt64Quietly(v, base));
  if (r.second == NumConversionResult::INVALID_BASE) {
    std::ostringstream msg;
    msg << base << " is not a valid base";
    throw IllegalValueError(msg.str(), PISTIS_EX_HERE);
  } else if (r.second != NumConversionResult::OK) {
    std::ostringstream msg;
    formatIllegalValue(msg, v);
    msg << " is not an unsigned 64-bit integer (";
    if (r.second == NumConversionResult::UNDERFLOWED) {
      // Use a more-sensible message
      msg << "Value is negative";
    } else {
      msg << descriptionFor(r.second);
    }
    msg << ")";
    throw IllegalValueError(msg.str(), PISTIS_EX_HERE);
  }
  return r.first;
}

std::pair<double, NumConversionResult>
    pistis::util::toDoubleQuietly(const char* v) {
  NumConversionResult r;
  const char* end;
  double n= strToDouble(v, &end, &r);
  if ((r == NumConversionResult::OK) && !restIsWhitespace(end)) {
    r= NumConversionResult::NOT_FLOATING_PT;
  }
  return std::make_pair(n, r);
}

double pistis::util::toDouble(const char* v) {
  std::pair<double, NumConversionResult> r= toDoubleQuietly(v);
  if (r.second != NumConversionResult::OK) {
    std::ostringstream msg;
    formatIllegalValue(msg, v);
    msg << " is not a floating-point number (" << descriptionFor(r.second)
	<< ")";
    throw IllegalValueError(msg.str(), PISTIS_EX_HERE);
  }
  return r.first;
}
