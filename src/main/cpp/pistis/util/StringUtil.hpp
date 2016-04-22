#ifndef __PISTIS__UTIL__STRINGUTIL_HPP__
#define __PISTIS__UTIL__STRINGUTIL_HPP__

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <ctype.h>
#include <stddef.h>
#include <string.h>

namespace pistis {
  namespace util {

    namespace detail {
      template <typename IterT, typename CharT>
      class StringJoiner {
      public:
	StringJoiner(const IterT& begin, const IterT& end, CharT separator):
	  _begin(begin), _end(end), _sep(separator) {
	}
	  
	std::ostream& print(std::ostream& out) const {
	  for (auto i= _begin; i != _end; ++i) {
	    if (i != _begin) {
	      out << _sep;
	    }
	    out << *i;
	  }
	  return out;
	}

      private:
	IterT _begin;  ///< Start of items to join together
	IterT _end;    ///< End of items to join together
	CharT _sep;    ///< Separator between joined items
      };

      template <typename StreamCharT, typename StreamTraitsT,
		typename IterT, typename JoinCharT>
      std::basic_ostream<StreamCharT, StreamTraitsT>& operator<<(
          std::basic_ostream<StreamCharT, StreamTraitsT>& out,
	  const detail::StringJoiner<IterT, JoinCharT>& joiner
      ) {
	return joiner.print(out);
      }
    }
 
    class SplitIterator {
    public:
      typedef std::input_iterator_tag iterator_category;
      typedef std::string value_type;
      typedef std::string reference;
      typedef std::string* pointer;
      typedef ptrdiff_t difference_type;

    public:
      SplitIterator();
      SplitIterator(const std::string& text, const std::string& separator);
      SplitIterator(const std::string::const_iterator& begin,
		    const std::string::const_iterator& end,
		    const std::string& separator);
      SplitIterator(const SplitIterator& other);
      SplitIterator(SplitIterator&& other);

      std::string rest() const { return std::string(_current, _end); }

      SplitIterator& operator=(const SplitIterator& other);
      SplitIterator& operator=(SplitIterator&& other);

      bool operator==(const SplitIterator& other) const {
	return _done && other._done;
      }
      bool operator!=(const SplitIterator& other) const {
	return !_done || !other._done;
      }

      std::string operator*() const {
	return !_done ? std::string(_current, _next) : std::string();
      }
      SplitIterator& operator++() {
	if (!_done) {
	  _advance();
	}
	return *this;
      }
      SplitIterator operator++(int dummy) {
	SplitIterator tmp(*this);
	if (!_done) {
	  _advance();
	}
	return tmp;
      }

    private:
      std::string::const_iterator _current;
      std::string::const_iterator _next;
      std::string::const_iterator _end;
      std::string _sep;
      bool _done;

      void _advance();
      static std::string::const_iterator _findFirst(
          const std::string::const_iterator& begin,
	  const std::string::const_iterator& end,
	  const std::string& sep
      );
      static std::string::const_iterator _findNext(
	  const std::string::const_iterator& begin,
	  const std::string::const_iterator& end,
	  const std::string& sep
      );
    };

    template <typename IterT, typename CharT>
    inline detail::StringJoiner<IterT, CharT> join(const IterT& begin,
						   const IterT& end,
						   CharT separator) {
      return detail::StringJoiner<IterT, CharT>(begin, end, separator);
    }

    template <typename IterT, typename CharT>
    inline std::string joinAsString(const IterT& begin, const IterT& end,
				    CharT separator) {
      std::ostringstream tmp;
      tmp << join(begin, end, separator);
      return tmp.str();
    }

    template <typename OutIterT>
    inline OutIterT splitTo(const std::string& s, const std::string& sep,
			    const OutIterT& output) {
      return std::copy(SplitIterator(s, sep), SplitIterator(), output);
    }

    template <typename OutIterT>
    OutIterT splitTo(const std::string& s, const std::string& sep,
		     int maxSplits, const OutIterT& output) {
      static const SplitIterator END_OF_SPLIT= SplitIterator();
      if (maxSplits >= 0) {
	auto i= SplitIterator(s, sep);
	auto out= output;
	int cnt= 0;
	while ((cnt < maxSplits) && (i != END_OF_SPLIT)) {
	  *out= *i;
	  ++i; ++out; ++cnt;
	}
	if (i != END_OF_SPLIT) {
	  *out= i.rest();
	  ++out;
	}
	return out;
      } else {
	return splitTo(s, sep, output);
      }
    }

    inline std::vector<std::string> split(const std::string& s,
					  const std::string& sep,
					  int maxSplits= -1) {
      std::vector<std::string> result;
      splitTo(s, sep, maxSplits, std::back_inserter(result));
      return result;
    }

    template <typename PredT>
    std::string lstrip(const std::string& s, const PredT& p) {
      std::string::const_iterator i= s.begin();
      while ((i != s.end()) && p(*i)) {
	++i;
      }
      return std::string(i, s.end());
    }

    template <typename PredT>
    std::string rstrip(const std::string& s, const PredT& p) {
      std::string::const_iterator i= s.end();
      while ((i != s.begin()) && p(i[-1])) {
	--i;
      }
      return std::string(s.begin(), i);
    }

    template <typename PredT>
    std::string strip(const std::string& s, const PredT& p) {
      std::string::const_iterator i= s.begin();
      std::string::const_iterator j= s.end();
      while ((i != s.end()) && p(*i)) {
	++i;
      }
      while ((j != i) && p(j[-1])) {
	--j;
      }
      return std::string(i,j);
    }

    inline std::string lstrip(const std::string& s, const char* chars) {
      return lstrip(s, [chars](char c) { return strchr(chars, c); });
    }

    inline std::string rstrip(const std::string& s, const char* chars) {
      return rstrip(s, [chars](char c) { return strchr(chars, c); });
    }

    inline std::string strip(const std::string& s, const char* chars) {
      return strip(s, [chars](char c) { return strchr(chars, c); });
    }

    inline std::string lstrip(const std::string& s) {
      return lstrip(s, [](char c) { return isspace(c); });
    }

    inline std::string rstrip(const std::string& s) {
      return rstrip(s, [](char c) { return isspace(c); });
    }

    inline std::string strip(const std::string& s) {
      return strip(s, [](char c) { return isspace(c); });
    }

    template <size_t n>
    inline bool startsWith(const std::string& s, const char prefix[n]) {
      return (s.size() >= (n-1)) && !strncmp(s.c_str(), prefix, n-1);
    }
    
    inline bool startsWith(const std::string& s, const std::string& prefix) {
      return (s.size() >= prefix.size()) &&
	     !strncmp(s.c_str(), prefix.c_str(), prefix.size());
    }

    template <size_t n>
    inline bool endsWith(const std::string& s, const char suffix[n]) {
      return (s.size() >= (n-1)) &&
	     !strcmp(s.c_str() + s.size() - n + 1, suffix);
    }

    inline bool endsWith(const std::string& s, const std::string& suffix) {
      return (s.size() >= suffix.size()) &&
	     !strcmp(s.c_str() + s.size() - suffix.size(), suffix.c_str());
    }

  }
}
#endif
