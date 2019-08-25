#ifndef __PISTIS__UTIL__ISTRING_HPP__
#define __PISTIS__UTIL__ISTRING_HPP__

#include <pistis/util/detail/IStringText.hpp>
#include <pistis/util/detail/IStringFormatter.hpp>
#include <pistis/util/IStringBuilder_.hpp>
#include <pistis/util/IStringSplitStream.hpp>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <stdint.h>
#include <string.h>

namespace pistis {
  namespace util {

    /** @brief An immutable string class
     *
     *  @todo  Add variations of split() for std::string, const Char[],
     *         etc.
     */
    template <typename Char, typename CharTraits = std::char_traits<Char>,
	      typename Allocator = std::allocator<uint8_t> >
    class ImmutableString {
    private:
      typedef detail::IStringTextPtr<Char, Allocator> StringTextPtr;
      
    public:
      typedef Char CharType;
      typedef CharTraits CharTraitsType;
      typedef Allocator AllocatorType;
      
      static constexpr const size_t NPOS = std::numeric_limits<size_t>::max();

      // TODO: Get this from IStringSplitStream somehow...
      static constexpr const size_t MAX_SPLITS =
	  std::numeric_limits<size_t>::max();

      class ConstIterator {
      public:
	typedef std::random_access_iterator_tag iterator_category;
	typedef Char value_type;
	typedef ptrdiff_t difference_type;
	typedef const Char& reference;
	typedef const Char* pointer;

      public:
	ConstIterator(const Char* p = nullptr): p_(p) { }

	reference operator*() const { return *p_; }
	pointer operator->() const { return p_; }
	ConstIterator& operator++() { ++p_; return *this; }
	ConstIterator  operator++(int) {
	  ConstIterator tmp(*this);
	  ++p_;
	  return tmp;
	}
	
	ConstIterator& operator--() { --p_; return *this; }
	ConstIterator  operator--(int) {
	  ConstIterator tmp(*this);
	  --p_;
	  return tmp;
	}
	
	ConstIterator  operator+(difference_type n) {
	  return ConstIterator(p_ + n);
	}

	ConstIterator  operator-(difference_type n) {
	  return ConstIterator(p_ - n);
	}

	difference_type operator-(const ConstIterator& other) const {
	  return p_ - other.p_;
	}

	ConstIterator& operator+=(difference_type n) {
	  p_ += n;
	  return *this;
	}

	ConstIterator& operator-=(difference_type n) {
	  p_ -= n;
	  return *this;
	}

	Char operator[](difference_type n) const { return p_[n]; }

	bool operator==(const ConstIterator& other) const {
	  return p_ == other.p_;
	}

	bool operator!=(const ConstIterator& other) const {
	  return p_ != other.p_;
	}

	bool operator<(const ConstIterator& other) const {
	  return p_ < other.p_;
	}

	bool operator<=(const ConstIterator& other) const {
	  return p_ <= other.p_;
	}

	bool operator>=(const ConstIterator& other) const {
	  return p_ >= other.p_;
	}

	bool operator>(const ConstIterator& other) const {
	  return p_ > other.p_;
	}
	
      private:
	const Char* p_;

	friend class ImmutableString;
      };
      
    private:
      typedef ImmutableStringBuilder<Char, CharTraits, Allocator> Builder;
      static constexpr size_t CHAR_INT_SIZE =
	  sizeof(typename CharTraits::int_type);

      struct LiteralTag { };
      
    public:
      ImmutableString(const Allocator& allocator = Allocator()):
	  text_(allocator), begin_(nullptr), end_(nullptr) {
      }

      template <typename Iterator>
      ImmutableString(const Iterator& begin, const Iterator& end,
		      const Allocator& allocator = Allocator()):
	  ImmutableString(std::distance(begin, end), begin, allocator) {
      }

      template <typename Iterator>
      ImmutableString(size_t size, const Iterator& begin,
		      const Allocator& allocator = Allocator()):
	  text_(makeText_(size, begin, allocator)),
	  begin_(size ? text_->text : nullptr),
	  end_(begin_ + size) {
      }

      template <size_t N>
      explicit ImmutableString(const Char (&text)[N],
			       const Allocator& allocator = Allocator()):
	  text_(allocator), begin_(text), end_(text + N - 1) {
      }
      
      template <typename C, size_t N,
		typename Enabled =
		    typename std::enable_if<
		                 !std::is_same<C, const Char>::value, int
		             >::type>
      explicit ImmutableString(C (&text)[N],
			       const Allocator& allocator = Allocator(),
			       Enabled = 0):
	  text_(makeText_(N - 1, text, allocator)),
	  begin_((N > 1) ? text_->text : nullptr),
	  end_(begin_ + N - 1) {
      }

      template <typename C, typename T, typename A>
      explicit ImmutableString(const std::basic_string<C, T, A>& text,
			       const Allocator& allocator = Allocator()):
	  ImmutableString(text.size(), text.begin(), allocator) {
      }
      
      ImmutableString(const ImmutableString& other):
	  text_(other.text_), begin_(other.begin_), end_(other.end_) {
      }

      template <typename OtherChar, typename OtherTraits,
		typename OtherAllocator>
      explicit ImmutableString(
	  const ImmutableString<OtherChar, OtherTraits, OtherAllocator>& other
      ):
	  text_(makeText_(other.size(), other.data(), other.allocator())),
	  begin_(other.size() ? text_->text : nullptr),
	  end_(begin_ + other.size()) {
      }
	
      ImmutableString(ImmutableString&& other):
	  text_(std::move(other.text_)), begin_(other.begin_),
	  end_(other.end_) {
	other.begin_ = nullptr;
	other.end_ = nullptr;
      }
      
      ~ImmutableString() { }

      const Allocator& allocator() const { return text_.allocator(); }
      Allocator& allocator() { return text_.allocator(); }
      
      size_t size() const { return end_ - begin_; }
      const Char* data() const { return begin_; }
      
      ConstIterator begin() const { return ConstIterator(begin_); }
      ConstIterator end() const { return ConstIterator(end_); }
      ConstIterator cbegin() const { return begin(); }
      ConstIterator cend() const { return end(); }
      ConstIterator position(size_t p) const {
	  return ConstIterator(begin_ + p);
      }
      size_t indexFor(ConstIterator p) const {
	return p.p_ - begin_;
      }

      template <typename C, typename T, typename A>
      int cmp(const ImmutableString<C, T, A>& other) const {
	return cmp_(other.data(), other.size(), (T*)0);
      }

      template <typename C, typename T, typename A>
      int cmp(const std::basic_string<C, T, A>& other) const {
	return cmp_(other.data(), other.size(), (T*)0);
      }

      template <typename C, size_t N>
      int cmp(C (&other)[N]) const { return cmp_(other, N - 1); }

      template <typename C>
      int cmp(C* const& other) const {
	return cmp(other, findFirstNull_(other));
      }

      template <typename C>
      int cmp(C* other, C* end) const { return cmp_(other, end - other); }
      
      ImmutableString substr(const size_t start,
			     const size_t end = NPOS) const {
	const Char* const e = begin_ + std::min(end, size());
	const Char* const s = std::min(begin_ + start, e);
	return ImmutableString(text_, s, e);
      }

      template <typename C,
		typename Enabled =
		    typename std::enable_if<std::is_integral<C>::value,
					    size_t>::type>
      size_t find(C c, size_t start = 0, size_t end = NPOS) const {
	const size_t e = std::min(size(), end);
	const size_t s = std::min(start, e);
	return setNpos_(findFirstChar_(begin_, c, s, e,
				       (std::char_traits<C>*)0), e);
      }

      template <typename C, typename T, typename A>
      size_t find(const ImmutableString<C, T, A>& s, size_t start = 0,
		  size_t end = NPOS) const {
	return find_(s.data(), s.size(), start, end, (T*)0);
      }

      template <typename C, typename T, typename A>
      size_t find(const std::basic_string<C, T, A>& s, size_t start = 0,
		  size_t end = NPOS) const {
	return find_(s.data(), s.size(), start, end, (T*)0);
      }

      template <typename C, size_t N>
      size_t find(C (&text)[N], size_t start = 0, size_t end = NPOS) const {
	return find_(text, N - 1, start, end);
      }

      template <typename C>
      size_t find(C* const& text, size_t start = 0, size_t end = NPOS) const {
	return find(text, findFirstNull_(text), start, end);
      }

      template <typename C>
      size_t find(C* text, C* textEnd, size_t start = 0,
		  size_t end = NPOS) const {
	return find_(text, textEnd - text, start, end);
      }
      
      template <typename C,
		typename Enabled =
		    typename std::enable_if<std::is_integral<C>::value,
					    size_t>::type>
      size_t findLast(C c, size_t start = 0, size_t end = NPOS) const {
	const size_t e = std::min(end, size());
	const size_t s = std::min(start, e);
	return setNpos_(findLastChar_(begin_, c, s, e,
				      (std::char_traits<C>*)0), e);
      }

      template <typename C, typename T, typename A>
      size_t findLast(const ImmutableString<C, T, A>& s, size_t start = 0,
		      size_t end = NPOS) const {
	return findLast_(s.data(), s.size(), start, end, (T*)0);
      }

      template <typename C, typename T, typename A>
      size_t findLast(const std::basic_string<C, T, A>& s, size_t start = 0,
		      size_t end = NPOS) const {
	return findLast_(s.data(), s.size(), start, end, (T*)0);
      }
      
      template <typename C, size_t N>
      size_t findLast(C (&text)[N], size_t start = 0, size_t end = NPOS) const {
	return findLast_(text, N - 1, start, end);
      }

      template <typename C>
      size_t findLast(C* const& text, size_t start = 0,
		      size_t end = NPOS) const {
	return findLast(text, findFirstNull_(text), start, end);
      }
      
      template <typename C>
      size_t findLast(C* text, C* textEnd, size_t start = 0,
		      size_t end = NPOS) const {
	return findLast_(text, textEnd - text, start, end);
      }

      template <typename C, typename T, typename A>
      size_t findFirstOf(const ImmutableString<C, T, A>& chars,
			 size_t start = 0, size_t end = NPOS) const {
	return findFirstOf_(chars.data(), chars.size(), start, end, (T*)0);
      }

      template <typename C, typename T, typename A>
      size_t findFirstOf(const std::basic_string<C, T, A>& chars,
			 size_t start = 0, size_t end = NPOS) const {
	return findFirstOf_(chars.data(), chars.size(), start, end, (T*)0);
      }
      
      template <typename C, size_t N>
      size_t findFirstOf(C (&chars)[N], size_t start = 0,
			 size_t end = NPOS) const {
	return findFirstOf_(chars, N - 1, start, end);
      }

      template <typename C>
      size_t findFirstOf(C* const& chars, size_t start = 0,
			 size_t end = NPOS) const {
	return findFirstOf(chars, findFirstNull_(chars), start, end);
      }
      
      template <typename C>
      size_t findFirstOf(C* const& chars, C* charsEnd, size_t start = 0,
			 size_t end = NPOS) const {
	return findFirstOf_(chars, charsEnd - chars, start, end);
      }

      template <typename C, typename T, typename A>
      size_t findLastOf(const ImmutableString<C, T, A>& chars,
			size_t start = 0, size_t end = NPOS) const {
	return findLastOf_(chars.data(), chars.size(), start, end, (T*)0);
      }

      template <typename C, typename T, typename A>
      size_t findLastOf(const std::basic_string<C, T, A>& chars,
			size_t start = 0, size_t end = NPOS) const {
	return findLastOf_(chars.data(), chars.size(), start, end, (T*)0);
      }
      
      template <typename C, size_t N>
      size_t findLastOf(C (&chars)[N], size_t start = 0,
			size_t end = NPOS) const {
	return findLastOf_(chars, N - 1, start, end);
      }

      template <typename C>
      size_t findLastOf(C* const& chars, size_t start = 0,
			size_t end = NPOS) const {
	return findLastOf(chars, findFirstNull_(chars), start, end);
      }
      
      template <typename C>
      size_t findLastOf(C* chars, C* charsEnd, size_t start = 0,
			size_t end = NPOS) const {
	return findLastOf_(chars, charsEnd - chars, start, end);
      }

      template <typename... Args>
      auto fmt(Args&&... args) const {
	ImmutableStringBuilder<Char, CharTraits, Allocator> builder;
	return detail::formatIString(builder, begin_, end_, begin_,
				     std::forward<Args>(args)...);
      }

      template <typename C,
		typename Enabled =
		    typename std::enable_if<
		        std::is_integral<C>::value, int
		    >::type>
      auto insert(size_t pos, C c, Enabled = 0) const {
	typedef std::char_traits<C> OtherTraits;
	typedef typename StringBuilder<Char, CharTraits, C, OtherTraits>::type
	        Builder;
	Builder builder(size() + 1, allocator());
	const Char* const p = begin_ + pos;
	return builder.append(begin_, p).append(c).append(p, end_).done();
      }

      template <typename C, typename T, typename A>
      auto insert(size_t pos, const ImmutableString<C, T, A>& text) const {
	return insert_(pos, text.data(), text.size(), (T*)0);
      }

      template <typename C, typename T, typename A>
      auto insert(size_t pos, const std::basic_string<C, T, A>& text) const {
	return insert_(pos, text.data(), text.size(), (T*)0);
      }

      template <typename C, size_t N>
      auto insert(size_t pos, C (&text)[N]) const {
	return insert_(pos, text, N - 1);
      }

      template <typename C>
      auto insert(size_t pos, C* const& text) const {
	return insert(pos, text, findFirstNull_(text));
      }

      template <typename C>
      auto insert(size_t pos, C* text, C* end) const {
	return insert_(pos, text, end - text);
      }

      
      template <typename C,
		typename Enabled =
		    typename std::enable_if<
		        std::is_integral<C>::value, int
		    >::type>
      auto append(C c, Enabled = 0) const {
	typename StringBuilder<Char, CharTraits, C, std::char_traits<C> >::type
	    builder(size() + 1, allocator());
	return (builder << *this << c).done();
      }
      
      template <typename C, typename T, typename A>
      auto append(const ImmutableString<C, T, A>& suffix) const {
	return append_(suffix.data(), suffix.size(), (T*)0);
      }

      template <typename C, typename T, typename A>
      auto append(const std::basic_string<C, T, A>& suffix) const {
	return append_(suffix.data(), suffix.size(), (T*)0);
      }
      
      template <typename C, size_t N>
      auto append(C (&text)[N]) const { return append_(text, N - 1); }

      template <typename C>
      auto append(C* const& text) const {
	return append(text, findFirstNull_(text));
      }

      template <typename C>
      auto append(C* text, C* end) const {
	return append_(text, end - text);
      }

      template <typename C1, typename C2,
		typename Enabled =
		    typename std::enable_if<
		        std::is_integral<C1>::value &&
		            std::is_integral<C2>::value,
		        int
		    >::type>
      auto replace(C1 target, C2 replacement, size_t start = 0,
		   size_t end = NPOS) const {
	auto f = [target, replacement](Char c) {
	  return (c == target) ? replacement : (C2)c;
	};
	return apply(f, start, end);
      }

      template <typename C1, typename T1, typename A1,
		typename C2, typename T2, typename A2>
      auto replace(const ImmutableString<C1, T1, A1>& target,
		   const ImmutableString<C2, T2, A2>& replacement,
		   size_t start = 0, size_t end = NPOS) const {
	return replace_(target.data(), target.size(),
			replacement.data(), replacement.size(),
			start, end, (T1*)0, (T2*)0);
      }

      template <typename C1, typename T1, typename A1,
		typename C2, typename T2, typename A2>
      auto replace(const ImmutableString<C1, T1, A1>& target,
		   const std::basic_string<C2, T2, A2>& replacement,
		   size_t start = 0, size_t end = NPOS) const {
	return replace_(target.data(), target.size(), replacement.data(),
			replacement.size(), start, end, (T1*)0, (T2*)0);
      }
      
      template <typename C1, typename T1, typename A1, typename C2, size_t N>
      auto replace(const ImmutableString<C1, T1, A1>& target,
		   C2 (&replacement)[N], size_t start = 0,
		   size_t end = NPOS) const {
	typedef std::char_traits<typename std::remove_cv<C2>::type> C2T;
	return replace_(target.data(), target.size(), replacement, N - 1,
			start, end, (T1*)0, (C2T*)0);
      }

      template <typename C1, typename T1, typename A1, typename C2>
      auto replace(const ImmutableString<C1, T1, A1>& target,
		   C2* const& replacement, size_t start = 0,
		   size_t end = NPOS) const {
	return replace(target, replacement, findFirstNull_(replacement),
		       start, end);
      }

      template <typename C1, typename T1, typename A1, typename C2>
      auto replace(const ImmutableString<C1, T1, A1>& target,
		   C2* replacement, C2* replacementEnd, size_t start = 0,
		   size_t end = NPOS) const {
	typedef std::char_traits<typename std::remove_cv<C2>::type> C2T;
	return replace_(target.data(), target.size(), replacement,
			replacementEnd - replacement, start, end,
			(T1*)0, (C2T*)0);
      }

      template <typename C1, typename T1, typename A1,
		typename C2, typename T2, typename A2>
      auto replace(const std::basic_string<C1, T1, A1>& target,
		   const ImmutableString<C2, T2, A2>& replacement,
		   size_t start = 0, size_t end = NPOS) const {
	return replace_(target.data(), target.size(), replacement.data(),
			replacement.size(), start, end, (T1*)0, (T2*)0);
      }
      
      template <typename C1, typename T1, typename A1,
		typename C2, typename T2, typename A2>
      auto replace(const std::basic_string<C1, T1, A1>& target,
		   const std::basic_string<C2, T2, A2>& replacement,
		   size_t start = 0, size_t end = NPOS) const {
	return replace_(target.data(), target.size(), replacement.data(),
			replacement.size(), start, end, (T1*)0, (T2*)0);
      }

      template <typename C1, typename T1, typename A1, typename C2, size_t N>
      auto replace(const std::basic_string<C1, T1, A1>& target,
		   C2 (&replacement)[N], size_t start = 0,
		   size_t end = NPOS) const {
	typedef std::char_traits<typename std::remove_cv<C2>::type> C2T;
	return replace_(target.data(), target.size(), replacement, N - 1,
			start, end, (T1*)0, (C2T*)0);
      }

      template <typename C1, typename T1, typename A1, typename C2>
      auto replace(const std::basic_string<C1, T1, A1>& target,
		   C2* const& replacement, size_t start = 0,
		   size_t end = NPOS) const {
	return replace(target, replacement, findFirstNull_(replacement),
		       start, end);
      }

      template <typename C1, typename T1, typename A1, typename C2>
      auto replace(const std::basic_string<C1, T1, A1>& target,
		   C2* replacement, C2* replacementEnd,
		   size_t start = 0, size_t end = NPOS) const {
	typedef std::char_traits<typename std::remove_cv<C2>::type> C2T;
	return replace_(target.data(), target.size(), replacement,
			replacementEnd - replacement, start, end,
			(T1*)0, (C2T*)0);
      }

      template <typename C1, size_t N, typename C2, typename T2, typename A2>
      auto replace(C1 (&target)[N],
		   const ImmutableString<C2, T2, A2>& replacement,
		   size_t start = 0, size_t end = NPOS) const {
	typedef std::char_traits<typename std::remove_cv<C1>::type> C1T;
	return replace_(target, N - 1, replacement.data(), replacement.size(),
			start, end, (C1T*)0, (T2*)0);
      }

      template <typename C1, size_t N, typename C2, typename T2, typename A2>
      auto replace(C1 (&target)[N],
		   const std::basic_string<C2, T2, A2>& replacement,
		   size_t start = 0, size_t end = NPOS) const {
	typedef std::char_traits<typename std::remove_cv<C1>::type> C1T;
	return replace_(target, N - 1, replacement.data(), replacement.size(),
			start, end, (C1T*)0, (T2*)0);
      }

      template <typename C1, size_t N1, typename C2, size_t N2>
      auto replace(C1 (&target)[N1], C2 (&replacement)[N2],
		   size_t start = 0, size_t end = NPOS) const {
	return replace_(target, N1 - 1, replacement, N2 - 1, start, end);
      }

      template <typename C1, size_t N1, typename C2>
      auto replace(C1 (&target)[N1], C2* const& replacement, size_t start = 0,
		   size_t end = NPOS) const {
	return replace(target, replacement, findFirstNull_(replacement),
		       start, end);
      }

      template <typename C1, size_t N1, typename C2>
      auto replace(C1 (&target)[N1], C2* replacement, C2* replacementEnd,
		   size_t start = 0, size_t end = NPOS) const {
	return replace_(target, N1 - 1, replacement,
			replacementEnd - replacement, start, end);
      }

      template <typename C1, typename C2, typename T2, typename A2>
      auto replace(C1* const& target,
		   const ImmutableString<C2, T2, A2>& replacement,
		   size_t start = 0, size_t end = NPOS) const {
	return replace(target, findFirstNull_(target), replacement,
		       start, end);
      }

      template <typename C1, typename C2, typename T2, typename A2>
      auto replace(C1* const& target,
		   const std::basic_string<C2, T2, A2>& replacement,
		   size_t start = 0, size_t end = NPOS) const {
	return replace(target, findFirstNull_(target), replacement,
		       start, end);
      }

      template <typename C1, typename C2, size_t N>
      auto replace(C1* const& target, C2 (&replacement)[N], size_t start = 0,
		   size_t end = NPOS) const {
	return replace(target, findFirstNull_(target), replacement,
		       start, end);
      }

      template <typename C1, typename C2>
      auto replace(C1* const& target, C2* const& replacement, size_t start = 0,
		   size_t end = NPOS) const {
	return replace(target, findFirstNull_(target),
		       replacement, findFirstNull_(replacement), start,
		       end);
      }

      template <typename C1, typename C2, typename T2, typename A2>
      auto replace(C1* const& target, C1* const& targetEnd,
		   const ImmutableString<C2, T2, A2>& replacement,
		   size_t start = 0, size_t end = NPOS) const {
	typedef std::char_traits<typename std::remove_cv<C1>::type> C1T;
	return replace_(target, targetEnd - target, replacement.data(),
			replacement.size(), start, end, (C1T*)0, (T2*)0);
      }

      template <typename C1, typename C2, typename T2, typename A2>
      auto replace(C1* const& target, C1* const& targetEnd,
		   const std::basic_string<C2, T2, A2>& replacement,
		   size_t start = 0, size_t end = NPOS) const {
	typedef std::char_traits<typename std::remove_cv<C1>::type> C1T;
	return replace_(target, targetEnd - target, replacement.data(),
			replacement.size(), start, end, (C1T*)0, (T2*)0);
      }

      template <typename C1, typename C2, size_t N>
      auto replace(C1* const& target, C1* const& targetEnd,
		   C2 (&replacement)[N], size_t start = 0,
		   size_t end = NPOS) const {
	return replace_(target, targetEnd - target, replacement, N - 1,
			start, end);
      }

      template <typename C1, typename C2>
      auto replace(C1* const& target, C1* const& targetEnd,
		   C2* const& replacement, C2* const& replacementEnd,
		   size_t start = 0, size_t end = NPOS) const {
	return replace_(target, targetEnd - target, replacement,
			replacementEnd - replacement, start, end);
      }

      template <typename C,
		typename Enabled =
		    typename std::enable_if<
		        std::is_integral<C>::value, int
		    >::type>
      ImmutableString remove(C c, size_t start = 0, size_t end = NPOS) const {
	return filter([c](Char x) { return c != x; }, start, end);
      }

      template <typename C, typename T, typename A>
      ImmutableString remove(const ImmutableString<C, T, A>& text,
			     size_t start = 0, size_t end = NPOS) const {
	return remove_(text.data(), text.size(), start, end, (T*)0);
      }

      template <typename C, typename T, typename A>
      ImmutableString remove(const std::basic_string<C, T, A>& text,
			     size_t start = 0, size_t end = NPOS) const {
	return remove_(text.data(), text.size(), start, end, (T*)0);
      }

      template <typename C, size_t N>
      ImmutableString remove(C (&text)[N], size_t start = 0,
			     size_t end = NPOS) const {
	return remove_(text, N - 1, start, end);
      }

      template <typename C>
      ImmutableString remove(C* const& text, size_t start = 0,
			     size_t end = NPOS) const {
	return remove(text, findFirstNull_(text), start, end);
      }

      template <typename C>
      ImmutableString remove(C* const& text, C* const& textEnd,
			     size_t start = 0, size_t end = NPOS) const {
	return remove_(text, textEnd - text, start, end);
      }

      ImmutableString strip() const {
	const Char* s, *e;
	for (s = begin_; (s < end_) && std::isspace(*s); ++s) {
	}
	for (e = end_; (e > s) && std::isspace(e[-1]); --e) {
	}
	return ImmutableString(text_, s, e);
      }
      
      ImmutableString& shrink() {
	const size_t n = size();
	if ((begin_ > (const Char*)text_.get()) || (n < text_->size)) {
	  text_ = StringTextPtr::create(n, begin_, allocator());
	  begin_ = n ? text_->text : nullptr;
	  end_ = begin_ + n;
	}
	return *this;
      }

      template <typename C, typename T, typename A>
      bool startsWith(const ImmutableString<C, T, A>& prefix) const {
	return startsWith_(prefix.data(), prefix.size(), (T*)0);
      }
      
      template <typename C, typename T, typename A>
      bool startsWith(const std::basic_string<C, T, A>& prefix) const {
	return startsWith_(prefix.data(), prefix.size(), (T*)0);
      }
      
      template <typename C, size_t N>
      bool startsWith(C (&prefix)[N]) const {
	return startsWith_(prefix, N - 1);
      }

      template <typename C>
      bool startsWith(C* const& prefix) const {
	return startsWith(prefix, findFirstNull_(prefix));
      }

      template <typename C>
      bool startsWith(C* prefix, C* end) const {
	return startsWith_(prefix, end - prefix);
      }

      template <typename C, typename T, typename A>
      bool endsWith(const ImmutableString<C, T, A>& suffix) const {
	return endsWith_(suffix.data(), suffix.size(), (T*)0);
      }

      template <typename C, typename T, typename A>
      bool endsWith(const std::basic_string<C, T, A>& suffix) const {
	return endsWith_(suffix.data(), suffix.size(), (T*)0);
      }

      template <typename C, size_t N>
      bool endsWith(C (&suffix)[N]) const { return endsWith_(suffix, N - 1); }

      template <typename C>
      bool endsWith(C* const& suffix) const {
	return endsWith(suffix, findFirstNull_(suffix));
      }

      template <typename C>
      bool endsWith(C* suffix, C* end) const {
	return endsWith_(suffix, end - suffix);
      }			

      bool isLowerCase() const {
	return all([](Char c) { return std::islower(c); });
      }
      
      bool isUpperCase() const {
	return all([](Char c) { return std::isupper(c); });
      }

      template <typename Predicate>
      bool all(const Predicate& p, size_t start = 0, size_t end = NPOS) const {
	const size_t e = std::min(size(), end);
	const size_t s = std::min(start, e);
	return std::all_of(begin_ + s, begin_ + e, p);
      }

      template <typename Predicate>
      bool any(const Predicate& p, size_t start = 0, size_t end = NPOS) const {
	const size_t e = std::min(size(), end);
	const size_t s= std::min(start, e);
	return std::any_of(begin_ + s, begin_ + e, p);
      }
      
      template <typename Predicate>
      ImmutableString filter(Predicate f, size_t start = 0,
			     size_t end = NPOS) const {
	const Char* const e = begin_ + std::min(size(), end);
	const Char* p = std::min(begin_ + start, e);
	ImmutableStringBuilder<Char, CharTraits, Allocator>
	    builder(e - p, allocator());

	builder.append(begin_, p);
	for (; p < e; ++p) {
	  if (f(*p)) {
	    builder << *p;
	  }
	}
	return builder.append(p, end_).done();
      }
      
      template <typename Function>
      auto apply(Function f, size_t start = 0, size_t end = NPOS) const {
	typedef typename std::remove_reference<
	                     typename std::remove_cv<
			         decltype(f(Char(0)))
			     >::type
	                 >::type
	        ResultChar;
	typedef std::char_traits<ResultChar> ResultTraits;
	typedef typename StringBuilder<Char, CharTraits,
				       ResultChar, ResultTraits>::type
	        ResultBuilder;
	
	const Char* const e = begin_ + std::min(size(), end);
	const Char* p = std::min(begin_ + start, e);
	ResultBuilder builder(size());

	builder.append(begin_, p);
	for (; p < e; ++p) {
	  builder << f(*p);
	}
	return builder.append(e, end_).done();
      }

      template <typename Iterator>
      ImmutableString join(Iterator begin, Iterator end) const {
	Builder builder(size(), allocator());
	for (auto i = begin; i != end; ++i) {
	  if (i != begin) {
	    builder << *this;
	  }
	  builder << *i;
	}
	return builder.done();
      }

      template <typename Container>
      ImmutableString join(const Container& c) const {
	return join(c.begin(), c.end());
      }

      template <typename C, typename T, typename A>
      auto split(const ImmutableString<C, T, A>& separator,
		 const size_t maxSplits = MAX_SPLITS) const {
	return IStringSplitStream<CharType, CharTraits, Allocator, C, T, A>(
	    *this, separator, maxSplits
	);
      }

      template <typename RegexTraits>
      auto split(
	  const std::basic_regex<CharType, RegexTraits>& separator,
	  const size_t maxSplits = MAX_SPLITS
      ) const {
	return RegexIStringSplitStream<CharType, CharTraits, Allocator,
				       RegexTraits>(*this, separator,
						    maxSplits);
      }
      
      ImmutableString& operator=(const ImmutableString&) = default;
      
      ImmutableString& operator=(ImmutableString&& other) {
	if (&other != this) {
	  text_ = std::move(other.text_);
	  begin_ = other.begin_; other.begin_ = nullptr;
	  end_ = other.end_; other.end_ = nullptr;
	}
	return *this;
      }

      template <typename C, typename T, typename A>
      ImmutableString& operator=(const ImmutableString<C, T, A>& other) {
	assign_(other.data(), other.size(), (T*)0);
	return *this;
      }

      template <typename C, typename T, typename A>
      ImmutableString& operator=(const std::basic_string<C, T, A>& other) {
	assign_(other.data(), other.size(), (T*)0);
	return *this;
      }

      template <typename C, size_t N>
      ImmutableString& operator=(C (&other)[N]) {
	typedef std::char_traits<typename std::remove_cv<C>::type> OtherTraits;
	assign_(other, N - 1, (OtherTraits*)0);
	return *this;
      }

      template <typename C, typename T, typename A>
      bool operator==(const ImmutableString<C, T, A>& other) const {
	return !cmp(other);
      }
      
      template <typename C, typename T, typename A>
      bool operator==(const std::basic_string<C, T, A>& other) const {
	return !cmp(other);
      }

      template <typename C, size_t N>
      bool operator==(C (&other)[N]) const {
	return !cmp(other);
      }

      template <typename C, typename T, typename A>
      bool operator!=(const ImmutableString<C, T, A>& other) const {
	return (bool)cmp(other);
      }
      
      template <typename C, typename T, typename A>
      bool operator!=(const std::basic_string<C, T, A>& other) const {
	return (bool)cmp(other);
      }
      
      template <typename C, size_t N>
      bool operator!=(C (&other)[N]) const {
	return (bool)cmp(other);
      }

      template <typename C, typename T, typename A>
      bool operator<(const ImmutableString<C, T, A>& other) const {
	return cmp(other) < 0;
      }
      
      template <typename C, typename T, typename A>
      bool operator<(const std::basic_string<C, T, A>& other) const {
	return cmp(other) < 0;
      }
      
      template <typename C, size_t N>
      bool operator<(C (&other)[N]) const {
	return cmp(other) < 0;
      }

      template <typename C, typename T, typename A>
      bool operator<=(const ImmutableString<C, T, A>& other) const {
	return cmp(other) <= 0;
      }

      template <typename C, typename T, typename A>
      bool operator<=(const std::basic_string<C, T, A>& other) const {
	return cmp(other) <= 0;
      }
      
      template <typename C, size_t N>
      bool operator<=(C (&other)[N]) const {
	return cmp(other) <= 0;
      }

      template <typename C, typename T, typename A>
      bool operator>(const ImmutableString<C, T, A>& other) const {
	return cmp(other) > 0;
      }

      template <typename C, typename T, typename A>
      bool operator>(const std::basic_string<C, T, A>& other) const {
	return cmp(other) > 0;
      }
      
      template <typename C, size_t N>
      bool operator>(C (&other)[N]) const {
	return cmp(other) > 0;
      }

      template <typename C, typename T, typename A>
      bool operator>=(const ImmutableString<C, T, A>& other) const {
	return cmp(other) >= 0;
      }
      
      template <typename C, typename T, typename A>
      bool operator>=(const std::basic_string<C, T, A>& other) const {
	return cmp(other) >= 0;
      }
      
      template <typename C, size_t N>
      bool operator>=(C (&other)[N]) const {
	return cmp(other) >= 0;
      }

      Char operator[](size_t n) const { return begin_[n]; }

      static ImmutableString literal(const Char* text, size_t n,
				     const Allocator& allocator = Allocator()) {
	return ImmutableString(text, n, allocator, LiteralTag());
      }
      
    private:
      template <typename OtherTraits>
      struct IntTypeConverter {
	typedef typename std::conditional<
	   sizeof(typename CharTraits::int_type) <
	       sizeof(typename OtherTraits::int_type),
	   typename OtherTraits::int_type,
	   typename CharTraits::int_type
	>::type ResultType;

	template <typename C, typename T>
	ResultType operator()(C c, T*) const {
	  return ResultType(T::to_int_type(c));
	}
      };

      template <typename C1, typename T1, typename C2, typename T2,
		bool useC2>
      struct StringBuilder_ {
	typedef C2 CharType;
	typedef T2 TraitsType;
	typedef ImmutableStringBuilder<C2, T2, Allocator> type;
      };

      template <typename C1, typename T1, typename C2, typename T2>
      struct StringBuilder_<C1, T1, C2, T2, false> {
	typedef C1 CharType;
	typedef T1 TraitsType;
	typedef ImmutableStringBuilder<C1, T1, Allocator> type;
      };

      template <typename C1, typename T1, typename C2, typename T2>
      struct StringBuilder :
	  public StringBuilder_<C1, T1, C2, T2,
				(sizeof(C2) > sizeof(C1))> {
      };
      
    private:
      StringTextPtr text_;
      const Char* begin_;
      const Char* end_;

      ImmutableString(const StringTextPtr& text, const Char* begin,
		      const Char* end):
	  text_(text), begin_(begin), end_(end) {
      }

      ImmutableString(StringTextPtr&& text, const Char* begin,
		      const Char* end):
	  text_(std::move(text)), begin_(begin), end_(end) {
      }

      ImmutableString(const Char* text, size_t n, const Allocator& allocator,
		      LiteralTag):
	  text_(allocator), begin_(text), end_(text + n) {
      }

      template <typename C, typename T>
      void assign_(const C* other, size_t size, T* = 0) {
	static_assert(sizeof(C) <= sizeof(Char), "Char type is too big");
	
	if (!size) {
	  text_.reset();
	  begin_ = nullptr;
	  end_ = nullptr;
	} else {
	  text_ = StringTextPtr::create(size, other, allocator());
	  begin_ = text_->text;
	  end_ = begin_ + size;
	}
      }
      
      template <typename OtherChar>
      int cmp_(const OtherChar* other, size_t size) const {
	return cmp_(other, size, (std::char_traits<OtherChar>*)0);
      }
      
      template <typename OtherChar, typename OtherTraits>
      int cmp_(const OtherChar* other, size_t size, OtherTraits*) const {
	const size_t sz = this->size();
	if (size < sz) {
	  const int o = compareChars_(begin_, other, size, (OtherTraits*)0);
	  return o ? o : 1;
	} else {
	  const int o = compareChars_(begin_, other, sz, (OtherTraits*)0);
	  return o ? o : -int(sz < size);
	}
      }

      static int compareChars_(const Char* left, const Char* right, size_t n,
			       CharTraits*) {
	return CharTraits::compare(left, right, n);
      }

      template <typename OtherChar, typename OtherTraits>
      static int compareChars_(const Char* left, const OtherChar* right,
			       size_t n, OtherTraits*) {
	static const IntTypeConverter<OtherTraits> toInt;
	for (size_t i = 0; i < n; ++i) {
	  const auto l = toInt(left[i], (CharTraits*)0);
	  const auto r = toInt(right[i], (OtherTraits*)0);
	  const auto delta = l - r;
	  if (delta) {
	    return delta;
	  }
	}
	return 0;
      }

      static size_t findFirstChar_(const Char* text, Char c, size_t start,
				   size_t end, CharTraits*) {
	const Char* const p = CharTraits::find(text + start, end - start, c);
	return p ? (p - text) : end;
      }

      template <typename OtherChar, typename OtherTraits>
      static size_t findFirstChar_(const Char* text, OtherChar c, size_t start,
				   size_t end, OtherTraits*) {
	static const IntTypeConverter<OtherTraits> toInt;
	const auto target = toInt(c, (OtherTraits*)0);
	size_t i = start;
	while ((i < end) && (toInt(text[i], (CharTraits*)0) != target)) {
	  ++i;
	}
	return i;
      }

      template <typename C>
      size_t find_(const C* other, size_t n, size_t start, size_t end) const {
	return find_(other, n, start, end, (std::char_traits<C>*)0);
      }
      
      template <typename C, typename T>
      size_t find_(const C* other, size_t n, size_t start, size_t end,
		   T*) const {
	if (!n || (end < start) || ((end - start) < n)) {
	  return NPOS;
	} else if (n == 1) {
	  const size_t e = std::min(end, size());
	  return setNpos_(findFirstChar_(begin_, *other, start, e, (T*)0), e);
	} else {
	  const size_t e = std::min(end, size()) - n + 1;
	  size_t p = findFirstChar_(begin_, *other, start, e, (T*)0);
	  while ((p < e) &&
		 compareChars_(begin_ + p + 1, other + 1, n - 1, (T*)0)) {
	    p = findFirstChar_(begin_, *other, p + 1, e, (T*)0);
	  }
	  return setNpos_(p, e);
	}
      }

      template <typename C, typename T>
      static size_t findLastChar_(const Char* text, C c, size_t start,
				  size_t end, T*) {
	static const IntTypeConverter<T> toInt;
	const auto target = toInt(c, (T*)0);
	size_t i = end;
	while ((i > start) && (toInt(text[i - 1], (CharTraits*)0) != target)) {
	  --i;
	}
	return i > start ? i - 1 : end;
      }

      template <typename C>
      size_t findLast_(const C* other, size_t n, size_t start,
		       size_t end) const {
	return findLast_(other, n, start, end, (std::char_traits<C>*)0);
      }
      
      template <typename C, typename T>
      size_t findLast_(const C* other, size_t n, size_t start, size_t end,
		       T*) const {
	if (!n || (end < start) || ((end - start) < n)) {
	  return NPOS;
	} else if (n == 1) {
	  const size_t e = std::min(end, size());
	  return setNpos_(findLastChar_(begin_, *other, start, e, (T*)0), e);
	} else {
	  const size_t e = std::min(end, size()) - n + 1;
	  size_t p = findLastChar_(begin_, *other, start, e, (T*)0);
	  while ((p < e) &&
		 compareChars_(begin_ + p + 1, other + 1, n - 1, (T*)0)) {
	    p = findLastChar_(begin_, *other, start, p, (T*)0);
	  }
	  return setNpos_(p, e);
	}
      }

      template <typename C, typename T>
      bool isInSet_(const C* chars, const C* end, Char target, T*) const {
	static const IntTypeConverter<T> toInt;
	const auto t = toInt(target, (T*)0);
	return std::any_of(chars, end,
			   [t](C c) { return toInt(c, (CharTraits*)0) == t; });
      }
      
      template <typename C>
      size_t findFirstOf_(const C* chars, size_t n, size_t start,
			  size_t end) const {
	return findFirstOf_(chars, n, start, end, (std::char_traits<C>*)0);
      }

      template <typename C, typename T>
      size_t findFirstOf_(const C* chars, size_t n, size_t start, size_t end,
			  T*) const {
	if (!n || (end < start)) {
	  return NPOS;
	} else if (n == 1) {
	  const size_t e = std::min(end, size());
	  return setNpos_(findFirstChar_(begin_, *chars, start, e, (T*)0), e);
	} else {
	  const size_t e = std::min(end, size());
	  const C* const endOfChars = chars + n;
	  for (size_t i = start; i < e; ++i) {
	    if (isInSet_(chars, endOfChars, begin_[i], (T*)0)) {
	      return i;
	    }
	  }
	  return NPOS;
	}
      }

      template <typename C>
      size_t findLastOf_(const C* chars, size_t n, size_t start,
			 size_t end) const {
	return findLastOf_(chars, n, start, end, (std::char_traits<C>*)0);
      }

      template <typename C, typename T>
      size_t findLastOf_(const C* chars, size_t n, size_t start, size_t end,
			 T*) const {
	if (!n || (end < start)) {
	  return NPOS;
	} else if (n == 1) {
	  const size_t e = std::min(end, size());
	  return setNpos_(findLastChar_(begin_, *chars, start, end, (T*)0), e);
	} else {
	  const size_t e = std::min(end, size());
	  const C* const endOfChars = chars + n;
	  for (size_t i = e; i > start; --i) {
	    if (isInSet_(chars, endOfChars, begin_[i - 1], (T*)0)) {
	      return i - 1;
	    }
	  }
	  return NPOS;
	}
      }

      template <typename C>
      auto insert_(size_t pos, const C* text, size_t n) const {
	return insert_(pos, text, n, (std::char_traits<C>*)0);
      }

      template <typename C, typename T>
      auto insert_(size_t pos, const C* text, size_t n, T*) const {
	typedef typename StringBuilder<Char, CharTraits, C, T>::type
	        ResultBuilder;
	ResultBuilder builder(size() + n, allocator());
	const Char* const p = begin_ + pos;
	return builder.append(begin_, p).append(text, text + n)
	              .append(p, end_).done();
      }

      template <typename C>
      auto append_(const C* text, size_t n) const {
	return append_(text, n, (std::char_traits<C>*)0);
      }
      
      template <typename C, typename T>
      auto append_(const C* text, size_t n, T*) const {
	typedef typename StringBuilder<Char, CharTraits, C, T>::type
	        ResultBuilder;
	ResultBuilder builder(size() + n, allocator());
	return builder.append(begin_, end_).append(text, text + n).done();
      }

      template <typename C1, typename C2>
      auto replace_(const C1* target, size_t targetSize, const C2* replacement,
		    size_t replacementSize, size_t start, size_t end) const {
	return replace_(target, targetSize, replacement, replacementSize,
			start, end, (std::char_traits<C1>*)0,
			(std::char_traits<C2>*)0);
      }

      template <typename C1, typename C2, typename T1, typename T2>
      auto replace_(const C1* target, size_t targetSize, const C2* replacement,
		    size_t replacementSize, size_t start, size_t end,
		    T1*, T2*) const {
	typedef typename StringBuilder<Char, CharTraits, C2, T2>::type
	        ResultBuilder;
	const C2* const replacementEnd = replacement + replacementSize;
	ResultBuilder builder(size(), allocator());
	const size_t e = std::min(size(), end);
	size_t last = std::min(start, e);
	size_t p = find_(target, targetSize, last, e, (T1*)0);

	builder.append(begin_, begin_ + last);
	while (p != NPOS) {
	  builder.append(begin_ + last, begin_ + p)
	         .append(replacement, replacementEnd);
	  last = p + targetSize;
	  p = find_(target, targetSize, last, e, (T1*)0);
	}
	return builder.append(begin_ + last, end_).done();
      }
      
      template <typename C>
      auto remove_(const C* target, size_t targetSize, size_t start,
		   size_t end) const {
	return replace_(target, targetSize, (Char*)0, 0, start, end,
			(std::char_traits<C>*)0, (CharTraits*)0);
      }

      template <typename C, typename T>
      auto remove_(const C* target, size_t targetSize, size_t start,
		   size_t end, T*) const {
	return replace_(target, targetSize, (Char*)0, 0, start, end,
			(T*)0, (CharTraits*)0);
      }

      template <typename C>
      bool startsWith_(const C* prefix, size_t prefixSize) const {
	return startsWith_(prefix, prefixSize, (std::char_traits<C>*)0);
      }

      template <typename C, typename T>
      bool startsWith_(const C* prefix, size_t prefixSize, T*) const {
	return (prefixSize <= size()) &&
	       !compareChars_(begin_, prefix, prefixSize, (T*)0);
      }

      template <typename C>
      bool endsWith_(const C* suffix, size_t suffixSize) const {
	return endsWith_(suffix, suffixSize, (std::char_traits<C>*)0);
      }

      template <typename C, typename T>
      bool endsWith_(const C* suffix, size_t suffixSize, T*) const {
	return (suffixSize <= size()) &&
	       !compareChars_(end_ - suffixSize, suffix, suffixSize, (T*)0);
      }

      static size_t setNpos_(size_t x, size_t e) {
	return (x < e) ? x : NPOS;
      }

      template <typename C>
      static C* findFirstNull_(C* p) {
	return p + std::char_traits<C>::length(p);
      }

      template <typename Iterator>
      static StringTextPtr makeText_(size_t n, const Iterator& text,
				     const Allocator& allocator) {
	static_assert(sizeof(decltype(*text)) <= sizeof(Char),
		      "Char type is too big");
	Allocator newAllocator(allocator);
	return StringTextPtr::create(n, text, newAllocator);
      }
      
      friend class ImmutableStringBuilder<Char, CharTraits, Allocator>;
    };

    template <typename C, typename T, typename A>
    const size_t ImmutableString<C, T, A>::NPOS;
    
    // Add missing + and relation ops
    template <typename C1, typename T1, typename A1,
	      typename C2, typename T2, typename A2>
    auto operator+(const ImmutableString<C1, T1, A1>& left,
		   const ImmutableString<C2, T2, A2>& right) {
      return left.append(right);
    }

    template <typename C1, typename T1, typename A1,
	      typename C2, typename T2, typename A2>
    auto operator+(const ImmutableString<C1, T1, A1>& left,
		   const std::basic_string<C2, T2, A2>& right) {
      return left.append(right);
    }

    template <typename C1, typename T1, typename A1,
	      typename C2, typename T2, typename A2>
    auto operator+(const std::basic_string<C1, T1, A1>& left,
		   const ImmutableString<C2, T2, A2>& right) {
      typedef typename std::conditional<sizeof(C1) >= sizeof(C2), C1, C2>::type
	      ResultChar;
      typedef typename std::conditional<sizeof(C1) >= sizeof(C2), T1, T2>::type
	      ResultTraits;
      ImmutableStringBuilder<ResultChar, ResultTraits, A2>
	  builder(left.size() + right.size(), right.allocator());
      return (builder << left << right).done();
    }

    template <typename C1, typename T1, typename A1,\
	      typename C2, size_t N>
    auto operator+(const ImmutableString<C1, T1, A1>& left, C2 (&right)[N]) {
      return left.append(right);
    }

    template <typename C1, size_t N, typename C2, typename T2, typename A2>
    auto operator+(C1 (&left)[N], const ImmutableString<C2, T2, A2>& right) {
      typedef typename std::conditional<sizeof(C1) >= sizeof(C2), C1, C2>::type
	      RawResultChar;
      typedef typename std::remove_cv<RawResultChar>::type ResultChar;
      typedef std::char_traits<ResultChar> C1T;
      typedef typename std::conditional<sizeof(C1) >= sizeof(C2), C1T, T2>::type
	      ResultTraits;
      ImmutableStringBuilder<ResultChar, ResultTraits, A2>
	  builder(N + right.size(), right.allocator());
      return (builder << left << right).done();
    }

    template <typename C1, typename T1, typename A1,
	      typename C2, typename T2, typename A2>
    bool operator==(const std::basic_string<C1, T1, A1>& left,
		    const ImmutableString<C2, T2, A2>& right) {
      return right == left;
    }

    template <typename C1, size_t N, typename C2, typename T2, typename A2>
    bool operator==(C1 (&left)[N], const ImmutableString<C2, T2, A2>& right) {
      return right == left;
    }

    template <typename C1, typename T1, typename A1,
	      typename C2, typename T2, typename A2>
    bool operator!=(const std::basic_string<C1, T1, A1>& left,
		    const ImmutableString<C2, T2, A2>& right) {
      return right != left;
    }

    template <typename C1, size_t N, typename C2, typename T2, typename A2>
    bool operator!=(C1 (&left)[N], const ImmutableString<C2, T2, A2>& right) {
      return right != left;
    }

    template <typename C1, typename T1, typename A1,
	      typename C2, typename T2, typename A2>
    bool operator<(const std::basic_string<C1, T1, A1>& left,
		   const ImmutableString<C2, T2, A2>& right) {
      return right > left;
    }

    template <typename C1, size_t N, typename C2, typename T2, typename A2>
    bool operator<(C1 (&left)[N], const ImmutableString<C2, T2, A2>& right) {
      return right > left;
    }

    template <typename C1, typename T1, typename A1,
	      typename C2, typename T2, typename A2>
    bool operator<=(const std::basic_string<C1, T1, A1>& left,
		    const ImmutableString<C2, T2, A2>& right) {
      return right >= left;
    }

    template <typename C1, size_t N, typename C2, typename T2, typename A2>
    bool operator<=(C1 (&left)[N], const ImmutableString<C2, T2, A2>& right) {
      return right >= left;
    }

    template <typename C1, typename T1, typename A1,
	      typename C2, typename T2, typename A2>
    bool operator>(const std::basic_string<C1, T1, A1>& left,
		   const ImmutableString<C2, T2, A2>& right) {
      return right < left;
    }

    template <typename C1, size_t N, typename C2, typename T2, typename A2>
    bool operator>(C1 (&left)[N], const ImmutableString<C2, T2, A2>& right) {
      return right < left;
    }

    template <typename C1, typename T1, typename A1,
	      typename C2, typename T2, typename A2>
    bool operator>=(const std::basic_string<C1, T1, A1>& left,
		    const ImmutableString<C2, T2, A2>& right) {
      return right <= left;
    }

    template <typename C1, size_t N, typename C2, typename T2, typename A2>
    bool operator>=(C1 (&left)[N], const ImmutableString<C2, T2, A2>& right) {
      return right <= left;
    }

    template <typename Char, typename CharTraits, typename Allocator>
    std::basic_ostream<Char>& operator<<(
	std::basic_ostream<Char>& out,
	const ImmutableString<Char, CharTraits, Allocator>& s
    ) {
      out.write(s.data(), s.size());
      return out;
    }

    namespace detail {
      template <typename T>
      struct IStringFormatArgIsTuple : std:: false_type {
      };

      template <typename... Args>
      struct IStringFormatArgIsTuple< std::tuple<Args...> > : std::true_type {
      };

      template <typename Char, typename CharTraits, typename Allocator,
		typename Arg,
		typename Enabled =
		    typename std::enable_if<
		       !IStringFormatArgIsTuple<
			   typename std::decay<Arg>::type
		       >::value, int
		    >::type>
      auto invokeIStringFmt(
	  const ImmutableString<Char, CharTraits, Allocator>& pattern,
	  Arg&& arg, Enabled = 0
      ) {
	return pattern.fmt(arg);
      }
      
      template <typename Char, typename CharTraits, typename Allocator,
		typename Tuple, size_t... Indices>
      auto invokeIStringFmt_(
	  const ImmutableString<Char, CharTraits, Allocator>& pattern,
	  Tuple&& args,
	  std::index_sequence<Indices...>
      ) {
	return pattern.fmt(std::get<Indices>(std::forward<Tuple>(args))...);
      }

      template <typename Char, typename CharTraits, typename Allocator,
		typename... Args>
      auto invokeIStringFmt(
	  const ImmutableString<Char, CharTraits, Allocator>& pattern,
	  const std::tuple<Args...>& args
      ) {
	return invokeIStringFmt_(pattern, args,
				 std::index_sequence_for<Args...>());
      }
    }
    template <typename Char, typename CharTraits, typename Allocator,
	      typename Arg>
    auto operator%(const ImmutableString<Char, CharTraits, Allocator>& pattern,
		   Arg&& arg) {
      return detail::invokeIStringFmt(pattern, arg);
    }

    typedef ImmutableString<char> IString;
    typedef ImmutableString<wchar_t> WIString;
    typedef IString U8_IString;
    typedef ImmutableString<char16_t> U16_IString;
    typedef ImmutableString<char32_t> U32_IString;

    inline IString operator ""_is(const char* s, std::size_t n) {
      return IString::literal(s, n);
    }

    inline WIString operator ""_wis(const wchar_t* s, std::size_t n) {
      return WIString::literal(s, n);
    }
  }
}

#endif
