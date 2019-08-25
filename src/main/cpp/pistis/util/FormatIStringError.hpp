#ifndef __PISTIS__UTIL__FORMATISTRINGERROR_HPP__
#define __PISTIS__UTIL__FORMATISTRINGERROR_HPP__

#include <pistis/exceptions/PistisException.hpp>

namespace pistis {
  namespace util {

    class FormatIStringError : public pistis::exceptions::PistisException {
    public:
      FormatIStringError(const std::string& details,
			 const pistis::exceptions::ExceptionOrigin& origin);
      virtual ~FormatIStringError() noexcept;

      static FormatIStringError invalidFormatSpecifier(
	  const std::string& specifier,
	  const pistis::exceptions::ExceptionOrigin& origin
      );

      static FormatIStringError incorrectType(
	   const std::string& baseSpecifier,
	   const std::string& expectedTypeName,
	   const std::string& givenTypeName,
	   const pistis::exceptions::ExceptionOrigin& origin
      );

      static FormatIStringError notEnoughArguments(
          const std::string& specifier,
	  const pistis::exceptions::ExceptionOrigin& origin
      );

      static FormatIStringError notAllArgumentsConverted(
	  const std::string& specifier,
	  const pistis::exceptions::ExceptionOrigin& origin
      );
    };
    
  }
}
#endif
