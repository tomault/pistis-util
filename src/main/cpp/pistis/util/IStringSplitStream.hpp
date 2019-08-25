#ifndef __PISTIS__UTIL__ISTRINGSPLITSTREAM_HPP__
#define __PISTIS__UTIL__ISTRINGSPLITSTREAM_HPP__

#include <pistis/exceptions/EndOfStream.hpp>
#include <algorithm>
#include <iterator>
#include <limits>
#include <regex>
#include <utility>
#include <vector>

namespace pistis {
  namespace util {
    template <typename Char, typename CharTraits, typename Allocator>
    class ImmutableString;

    namespace detail {
      template <typename DerivedStream, typename SourceString>
      class IStringSplitStreamBase {
      public:
	typedef SourceString SourceStringType;
	
	static const size_t MAX_SPLITS = std::numeric_limits<size_t>::max();
      
      public:
	template <typename Function>
	void forEach(Function f) {
	  while (*this) {
	    f(self().next());
	  }
	}

	template <typename Container>
	Container& to(Container& c) {
	  while (*this) {
	    c.push_back(self().next());
	  }
	  return c;
	}

	template <typename Container>
	Container to() {
	  Container c;
	  this->to(c);
	  return std::move(c);
	}
	
	std::vector<SourceString> toVector() {
	  return this->to< std::vector<SourceString> >();
	}

	operator bool() const { return self().ready(); }

      protected:
	const DerivedStream& self() const {
	  return static_cast<const DerivedStream&>(*this);
	}

	DerivedStream& self() { return static_cast<DerivedStream&>(*this); }

	template <typename T, typename U, typename V>
	friend IStringSplitStreamBase<T, U>& operator>>(
	    IStringSplitStreamBase<T, U>&, V&
	);
      };

      template <typename T, typename U, typename V>
      inline IStringSplitStreamBase<T, U>& operator>>(
	  IStringSplitStreamBase<T, U>& stream, V& value
      ) {
	value = stream.self().next();
	return stream;
      }
    }

    template <typename Char, typename CharTraits, typename Allocator,
	      typename TargetChar, typename TargetCharTraits,
	      typename TargetAllocator>
    class IStringSplitStream :
        public detail::IStringSplitStreamBase<
            IStringSplitStream<Char, CharTraits, Allocator,
			       TargetChar, TargetCharTraits, TargetAllocator>,
            ImmutableString<Char, CharTraits, Allocator>
        > {
    private:
      typedef detail::IStringSplitStreamBase<
          IStringSplitStream<Char, CharTraits, Allocator,
			     TargetChar, TargetCharTraits, TargetAllocator>,
          ImmutableString<Char, CharTraits, Allocator>
      > ParentType;

    public:
      using typename ParentType::SourceStringType;
      using ParentType::MAX_SPLITS;
      typedef ImmutableString<TargetChar, TargetCharTraits, TargetAllocator>
              TargetStringType;

    public:
      IStringSplitStream(const SourceStringType& source,
			 const TargetStringType& target,
			 size_t maxSplits = MAX_SPLITS):
	  source_(source), target_(target), maxSplits_(maxSplits), current_(0),
          splitCount_(0), ready_(source.size()) {
      }
      IStringSplitStream(const IStringSplitStream&) = default;
      IStringSplitStream(IStringSplitStream&&) = default;

      bool ready() const { return ready_; }

      SourceStringType next() {
	if (!ready()) {
	  throw pistis::exceptions::EndOfStream(PISTIS_EX_HERE);
	} else if (splitCount_ >= maxSplits_) {
	  const size_t i = current_;
	  current_ = source_.size();
	  ready_ = false;
	  return source_.substr(i);
	} else if (!target_.size()) {
	  ++current_;
	  ++splitCount_;
	  ready_ = current_ < source_.size();
	  return source_.substr(current_ - 1, current_);
	} else {
	  const size_t last = current_;
	  const size_t next = source_.find(target_, current_);
	  if (next < source_.size()) {
	    current_ = next + target_.size();
	  } else {
	    current_ = source_.size();
	    ready_ = false;
	  }
	  ++splitCount_;
	  return source_.substr(last, next);
	}
      }

      IStringSplitStream& operator=(const IStringSplitStream&) = default;
      IStringSplitStream& operator=(IStringSplitStream&&) = default;

    private:
      const ImmutableString<Char, CharTraits, Allocator> source_;
      const ImmutableString<Char, CharTraits, Allocator> target_;
      const size_t maxSplits_;
      size_t current_;
      size_t splitCount_;
      bool ready_;
    };

    template <typename Char, typename CharTraits, typename Allocator,
	      typename RegexTraits = std::regex_traits<Char> >
    class RegexIStringSplitStream:
        public detail::IStringSplitStreamBase<
            RegexIStringSplitStream<Char, CharTraits, Allocator, RegexTraits>,
            ImmutableString<Char, CharTraits, Allocator>
        > {
    private:
      typedef detail::IStringSplitStreamBase<
          RegexIStringSplitStream<Char, CharTraits, Allocator, RegexTraits>,
          ImmutableString<Char, CharTraits, Allocator>
      > ParentType;

    public:
      using typename ParentType::SourceStringType;
      using ParentType::MAX_SPLITS;
      typedef std::basic_regex<Char, RegexTraits> RegexType;

    private:
      typedef typename std::match_results<
	                  typename SourceStringType::ConstIterator
                       >::value_type::difference_type
              MatchLengthType;
    public:
      RegexIStringSplitStream(const SourceStringType& source,
			      const RegexType& regex,
			      size_t maxSplits = MAX_SPLITS):
	  source_(source), target_(regex), maxSplits_(maxSplits),
	  current_(source_.begin()), splitCount_(0), ready_(source.size()) {
      }
      RegexIStringSplitStream(const RegexIStringSplitStream&) = default;
      RegexIStringSplitStream(RegexIStringSplitStream&&) = default;

      bool ready() const { return ready_; }

      SourceStringType next() {
	if (!ready()) {
	  throw pistis::exceptions::EndOfStream(PISTIS_EX_HERE);
	} else if (splitCount_ >= maxSplits_) {
	  const size_t p = source_.indexFor(current_);
	  current_ = source_.end();
	  ready_ = false;
	  return source_.substr(p);
	} else {
	  std::match_results<typename SourceStringType::ConstIterator> match;
	  if (std::regex_search(current_, source_.end(), match, target_)) {
	    const size_t i = source_.indexFor(current_);
	    const size_t p = match.position();
	    if (match.length()) {
	      current_ += p + match.length();
	      ++splitCount_;
	      return source_.substr(i, i + p);
	    } else {
	      ++current_;
	      ++splitCount_;
	      ready_ = current_ < source_.end();
	      return source_.substr(i, i + 1);
	    }
	  } else {
	    const size_t i = source_.indexFor(current_);
	    current_ = source_.end();
	    ++splitCount_;
	    ready_ = false;
	    return source_.substr(i);
	  }
	}
      }

      RegexIStringSplitStream& operator=(const RegexIStringSplitStream&) =
          default;
      RegexIStringSplitStream& operator=(RegexIStringSplitStream&&) = default;

    private:
      const SourceStringType source_;
      const RegexType target_;
      const size_t maxSplits_;
      typename SourceStringType::ConstIterator current_;
      size_t splitCount_;
      bool ready_;
    };
    
  }
}

#endif
