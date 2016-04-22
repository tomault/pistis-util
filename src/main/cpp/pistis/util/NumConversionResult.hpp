#ifndef __PISTIS__TYPES__NUMCONVERSIONRESULT_HPP__
#define __PISTIS__TYPES__NUMCONVERSIONRESULT_HPP__

#include <ostream>
#include <string>

namespace pistis {
  namespace util {

    enum class NumConversionResult {
      OK = 0,          ///< Conversion performed successfully
      UNDERFLOWED,     ///< Conversion underflowed its type
      OVERFLOWED,      ///< Conversion overflowed its type
      EMPTY_STRING,    ///< Asked to convert an empty string
      INVALID_BASE,    ///< Conversion base is negative
      NOT_AN_INTEGER,  ///< String is not an integer
      NOT_FLOATING_PT  ///< String is not a floating point number
    };

    const std::string& toString(NumConversionResult result);
    const std::string& descriptionFor(NumConversionResult result);

    inline std::ostream& operator<<(std::ostream& out,
				    NumConversionResult result) {
      return out << toString(result);
    }
    
      
  }
}
#endif

