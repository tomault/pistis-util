#include <pistis/util/FormatIStringError.hpp>
#include <sstream>
#include <ctype.h>

using namespace pistis::exceptions;
using namespace pistis::util;

namespace {
  inline bool isVowel(char c) {
    return (c == 'a') || (c == 'e') || (c == 'i') || (c == 'o') || (c == 'u');
  }
  
  inline bool startsWithVowel(const std::string& s) {
    return s.size() && isVowel(::tolower(s[0]));
  }
  
  const std::string& getCapitalizedArticle(const std::string& s) {
    static const std::string A("a");
    static const std::string AN("an");

    // Good enough for type names.  An all-purpose article would have to
    // understand the sound the word starts with.
    return startsWithVowel(s) ? A : AN;
  }
  
  const std::string& getArticle(const std::string& s) {
    static const std::string A("a");
    static const std::string AN("an");

    // Good enough for type names.  An all-purpose article would have to
    // understand the sound the word starts with.
    return startsWithVowel(s) ? A : AN;
  }  
}

FormatIStringError::FormatIStringError(
    const std::string& details,
    const pistis::exceptions::ExceptionOrigin& origin
): PistisException(details, origin) {
}

FormatIStringError::~FormatIStringError() noexcept {
}

FormatIStringError FormatIStringError::invalidFormatSpecifier(
    const std::string& specifier,
    const pistis::exceptions::ExceptionOrigin& origin
) {
  std::ostringstream msg;
  msg << "Invalid format specifier \"" << specifier << "\"";
  return FormatIStringError(msg.str(), origin);
}

FormatIStringError FormatIStringError::incorrectType(
    const std::string& baseSpecifier, const std::string& expectedTypeName,
    const std::string& givenTypeName,
    const pistis::exceptions::ExceptionOrigin& origin
) {
  std::ostringstream msg;

  msg << "Incorrect type for \"" << baseSpecifier << "\": "
      << getCapitalizedArticle(expectedTypeName) << " " << expectedTypeName
      << " was expected, not " << getArticle(givenTypeName) << " "
      << givenTypeName;
  return FormatIStringError(msg.str(), origin);
}

FormatIStringError FormatIStringError::notEnoughArguments(
    const std::string& specifier,
    const pistis::exceptions::ExceptionOrigin& origin
) {
  std::ostringstream msg;

  msg << "Not enough arguments for format specifier \"" << specifier << "\"";
  return FormatIStringError(msg.str(), origin);
}

FormatIStringError FormatIStringError::notAllArgumentsConverted(
    const std::string& specifier,
    const pistis::exceptions::ExceptionOrigin& origin
)
{
  std::ostringstream msg;
  msg << "Did not convert all arguments while formatting \"" << specifier
      << "\"";
  return FormatIStringError(msg.str(), origin);
}
