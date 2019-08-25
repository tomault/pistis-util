#ifndef __PISTIS__UTIL__RANGEITERATOR_HPP__
#define __PISTIS__UTIL__RANGEITERATOR_HPP__

#include <pistis/typeutil/ExtendedTypeTraits.hpp>
#include <iterator>

namespace pistis {
  namespace util {

    template <typename BaseIterator>
    class StlRangeIterator {
    public:
      typedef BaseIterator BaseIteratorType;
      typedef decltype(**(BaseIterator*)0) DereferenceType;
      typedef typename pistis::typeutil::RemoveCVR<DereferenceType>::type
              ValueType;

      typedef std::forward_iterator_tag iterator_category;
      typedef ValueType value_type;
      typedef DereferenceType reference;
      typedef void* pointer;
      typedef ssize_t difference_type;

    private:
      class Proxy {
      public:
	Proxy(BaseIterator p): p_(p) { }
	auto& operator*() const { return *p_; }

      private:
	BaseIterator p_;
      };
      
    public:
      StlRangeIterator(BaseIterator startOfRange, BaseIterator endOfRange):
  	  p_(startOfRange), end_(endOfRange) {
      }
      StlRangeIterator(const StlRangeIterator<BaseIterator>& other) = default;

      StlRangeIterator<BaseIterator>& operator=(
	  const StlRangeIterator<BaseIterator>&
      ) = default;

      operator bool() const { return p_ != end_; }
      auto& operator*() const { return *p_; }

      StlRangeIterator<BaseIterator>& operator++() { ++p_; return *this; }
      Proxy operator++(int) { Proxy tmp(p_); ++p_; return tmp; }
      StlRangeIterator<BaseIterator>& operator+=(size_t n) {
	std::advance(p_);
	return *this;
      }
      StlRangeIterator<BaseIterator> operator+(size_t n) {
	return StlRangeIterator<BaseIterator>(std::next(p_, n));
      }

      bool operator==(const StlRangeIterator<BaseIterator>& other) const {
	return p_ == other.p_;
      }
      bool operator!=(const StlRangeIterator<BaseIterator>& other) const {
	return p_ != other.p_;
      }

    private:
      BaseIterator p_;
      BaseIterator end_;
    };

    template <typename Iterator>
    inline StlRangeIterator<Iterator> makeStlRangeIterator(Iterator begin,
							   Iterator end) {
      return StlRangeIterator<Iterator>(begin, end);
    }
  }
}

#endif

