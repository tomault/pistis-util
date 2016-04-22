#ifndef __PISTIS__UTIL__CSTRINGUTIL_HPP__
#define __PISTIS__UTIL__CSTRINGUTIL_HPP__

/** @file CStringUtil.hpp
 *
 *  Utilities for working with null-terminated "C" strings.
 */

#include <string>
#include <string.h>
#include <stdint.h>

namespace pistis {
  namespace util {
      
    /** @brief Functor that computes a hash code for a C string */
    struct CStringHasher {
      /** @brief Compute a hash code for s
       *
       *  The current implementation uses djb2.
       */
      uint64_t operator()(const char* s) const {
	uint64_t v= 5381;
	for (const char*p= s; *p; ++p) {
	  v= ((v << 5) + v) + (int)*p;
	}
	return v;	
      }
    };

    /** @brief Less-than predicate for C strings */
    struct CStringsLess {
      /** @brief Returns true if left comes before right in a character-by-
       *         character comparison.
       */
      bool operator(const char* left, const char* right) const {
	return strcmp(left, right) < 0;
      }
    };

    /** @brief Equality predicate for C strings */
    struct CStringsEqual {
      /** @brief Returns true if its arguments are equal */
      bool operator(const char* left, const char* right) const {
	return !strcmp(left, right);
      }
    };

    /** @brief Compute a hash code for a C string */
    inline uint64_t computeHashCode(const char* s) {
      static const CStringHasher HASH;
      return HASH(s);
    }

  }
}
#endif

