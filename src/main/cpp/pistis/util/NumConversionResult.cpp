#include "NumConversionResult.hpp"
#include <vector>
#include <stdint.h>

using namespace pistis::util;

namespace {
  class NameMap {
  public:
    NameMap();
    const std::string& operator[](NumConversionResult result) const {
      static const std::string UNKNOWN_RESULT("**UNKNOWN**");
      uint32_t n = (uint32_t)result;
      return (n < names_.size()) ? names_[n] : UNKNOWN_RESULT;
    }

  private:
    std::vector<std::string> names_;
  };

  NameMap::NameMap():
      names_() {
    names_.reserve(7);
    names_.emplace_back("OK");
    names_.emplace_back("UNDERFLOWED");
    names_.emplace_back("OVERFLOWED");
    names_.emplace_back("EMPTY_STRING");
    names_.emplace_back("INVALID_BASE");
    names_.emplace_back("NOT_AN_INTEGER");
    names_.emplace_back("NOT_FLOATING_PT");
  }

  class DescriptionMap {
  public:
    DescriptionMap();
    const std::string& operator[](NumConversionResult result) const {
      static const std::string UNKNOWN_RESULT("**UNKNOWN**");
      uint32_t n = (uint32_t)result;
      return (n < names_.size()) ? names_[n] : UNKNOWN_RESULT;
    }

  private:
    std::vector<std::string> names_;
  };

  DescriptionMap::DescriptionMap():
      names_() {
    names_.reserve(7);
    names_.emplace_back("Conversion successful");
    names_.emplace_back("Value is too small");
    names_.emplace_back("Value is too large");
    names_.emplace_back("Value is empty");
    names_.emplace_back("The base is invalid");
    names_.emplace_back("Value is not an integer");
    names_.emplace_back("Value is not a floating-point number");
  }
}

const std::string& pistis::util::toString(
    pistis::util::NumConversionResult result
) {
  static const NameMap NAME_MAP;
  return NAME_MAP[result];
}

const std::string& pistis::util::descriptionFor(
    pistis::util::NumConversionResult result
) {
  static const DescriptionMap DESCRIPTION_MAP;
  return DESCRIPTION_MAP[result];
}
