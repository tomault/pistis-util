#ifndef __PISTIS__UTIL__MAPPINGITERATOR_HPP__
#define __PISTIS__UTIL__MAPPINGITERATOR_HPP__

#include <pistis/typeutil/ExtendedTypeTraits.hpp>
#include <iterator>

#include <iostream>

namespace pistis {
  namespace util {

    template <typename BaseIterator, typename Function>
    class MappingIterator {
    public:
      typedef BaseIterator BaseIteratorType;
      typedef Function MappingFunction;

      typedef typename pistis::typeutil::RemoveQualifiers<
	  decltype((*(Function*)0)(**(BaseIterator*)0))
      >::type ValueType;
      typedef ValueType DereferenceType;

      typedef std::forward_iterator_tag iterator_category;
      typedef ValueType value_type;
      typedef const ValueType& reference;
      typedef const ValueType* pointer;
      typedef ssize_t difference_type;

    private:
      class Proxy {
      public:
	Proxy(BaseIterator p, Function f) : p_(p), f_(f) { }

	auto operator*() const { return f_(*p_); }

      private:
	BaseIterator p_;
	Function f_;
      };

    public:
      MappingIterator(const BaseIterator& base, Function f):
	  p_(base), f_(f) {
      }
      MappingIterator(const MappingIterator<BaseIterator, Function>&) = default;

      MappingIterator<BaseIterator, Function>& operator=(
	  const MappingIterator<BaseIterator, Function>&
      ) = default;
      
      operator bool() const { return bool(p_); }
      auto operator*() const { return f_(*p_); }

      MappingIterator<BaseIterator, Function>& operator++() {
	++p_;
	return *this;
      }

      Proxy operator++(int) {
	Proxy tmp(p_, f_);
	++p_;
	return tmp;
      }

      MappingIterator<BaseIterator, Function>& operator+=(size_t n) {
	p_ += n;
	return *this;
      }

      MappingIterator<BaseIterator, Function>  operator+(size_t n) {
	return MappingIterator<BaseIterator, Function>(p_ + n, f_);
      }
      
      bool operator==(
	  const MappingIterator<BaseIterator, Function>& other
      ) const {
	return p_ == other.p_;
      }
      
      bool operator!=(
	  const MappingIterator<BaseIterator, Function>& other
      ) const {
	return p_ != other.p_;
      }
      
    private:
      BaseIterator p_; ///< Current position in base sequence
      Function f_;     ///< Transforms base into mapped sequence
    };

    template <typename Iterator, typename Function>
    MappingIterator<Iterator, Function> makeMappingIterator(Iterator it,
							    Function f) {
      return MappingIterator<Iterator, Function>(it, f);
    }
  }
}
#endif

