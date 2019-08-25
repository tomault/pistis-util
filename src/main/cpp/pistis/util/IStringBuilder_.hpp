#ifndef __PISTIS__UTIL__ISTRINGBUILDER__HPP__
#define __PISTIS__UTIL__ISTRINGBUILDER__HPP__

#include <pistis/typeutil/ExtendedTypeTraits.hpp>
#include <pistis/util/detail/IStringText.hpp>
#include <cmath>
#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <stdint.h>

namespace pistis {
  namespace util {

    template <typename Char, typename CharTraits, typename Allocator>
    class ImmutableString;

    template <typename Char, typename CharTraits = std::char_traits<Char>,
	      typename Allocator = std::allocator<uint8_t> >
    class ImmutableStringBuilder {
    private:
      typedef detail::IStringTextPtr<Char, Allocator> StringTextPtr;
      
    public:
      typedef ImmutableString<Char, CharTraits, Allocator> StringType;

    private:
      struct CharTag { };
      struct IntegralTag { };

    public:
      ImmutableStringBuilder(const Allocator& allocator = Allocator()):
	  text_(allocator), end_(nullptr), eos_(nullptr),
	  flags_(DEFAULT_FORMAT_FLAGS_), fieldWidth_(0), fieldPrecision_(0),
	  fieldPadding_(' ') {
      }

      explicit ImmutableStringBuilder(uint32_t initialBufferSize,
				      const Allocator& allocator = Allocator()):
	  text_(StringTextPtr::create(initialBufferSize, allocator)),
	  end_(initialBufferSize ? text_->text : nullptr),
	  eos_(initialBufferSize ? text_->text + text_->size : nullptr),
	  flags_(DEFAULT_FORMAT_FLAGS_), fieldWidth_(0), fieldPrecision_(0),
	  fieldPadding_(' ') {
      }
      ImmutableStringBuilder(const ImmutableStringBuilder&) = delete;
      ImmutableStringBuilder(ImmutableStringBuilder&&) = default;

      const Allocator& allocator() const { return text_.allocator(); }
      Allocator& allocator() { return text_.allocator(); }
      size_t allocated() const { return text_ ? eos_ - text_->text : 0; }
      size_t size() const { return text_ ? end_ - text_->text : 0; }
      
      template <typename C,
		typename Enabled =
		    typename std::enable_if<
		        pistis::typeutil::IsCharType<C>::value,
		        CharTag
		    >::type>
      ImmutableStringBuilder& append(C c, CharTag = Enabled()) {
	static_assert(sizeof(C) <= sizeof(Char), "Value too large");
	put_(&c, &c + 1);
	return *this;
      }

      template <typename T,
		typename Enabled =
		    typename std::enable_if<
		        std::is_integral<T>::value &&
		            !pistis::typeutil::IsCharType<T>::value,
		        IntegralTag
		    >::type>
      ImmutableStringBuilder& append(T value, IntegralTag = Enabled()) {
	std::string s = formatInt_(value);
	put_(s.begin(), s.end());
	return *this;
      }

      template <typename T,
		typename Enabled =
		    typename std::enable_if<std::is_floating_point<T>::value,
					    int>::type>
      ImmutableStringBuilder& append(T value, Enabled = 0) {
	std::string s = formatFloat_(value);
	put_(s.begin(), s.end());
	return *this;
      }
      
      template <typename Iterator>
      ImmutableStringBuilder& append(Iterator start, Iterator end) {
	static_assert(sizeof(decltype(*start)) <= sizeof(Char),
		      "Char type too large");
	put_(start, end);
	return *this;
      }

      template <typename C, typename T, typename A>
      ImmutableStringBuilder& append(
	  const ImmutableString<C, T, A>& text
      ) {
	static_assert(sizeof(C) <= sizeof(Char), "Char type too large");
	put_(text.begin(), text.end());
	return *this;
      }

      template <typename C, typename T, typename A>
      ImmutableStringBuilder& append(
	  const std::basic_string<C, T, A>& text
      ) {
	static_assert(sizeof(C) <= sizeof(Char), "Char type too large");
	put_(text.begin(), text.end());
	return *this;
      }

      template <typename C, size_t N>
      ImmutableStringBuilder& append(C (&text)[N]) {
	static_assert(sizeof(C) <= sizeof(Char), "Char type too large");
	put_((const C*)text, (const C*)text + N - 1);
	return *this;
      }

      ImmutableStringBuilder& left() {
	setJustification_(FORMAT_FIELD_JUSTIFY_LEFT_);
	return *this;
      }

      ImmutableStringBuilder& right() {
	setJustification_(FORMAT_FIELD_JUSTIFY_RIGHT_);
	return *this;
      }

      ImmutableStringBuilder& center() {
	setJustification_(FORMAT_FIELD_JUSTIFY_CENTER_);
	return *this;
      }

      ImmutableStringBuilder& width(uint32_t n) {
	fieldWidth_ = n;
	setFormatFlags_(FORMAT_FIELD_WIDTH_);
	return *this;
      }

      ImmutableStringBuilder& noWidth() {
	clearFormatFlag_(FORMAT_FIELD_WIDTH_);
      }

      ImmutableStringBuilder& pad(Char c) {
	fieldPadding_ = c;
	return *this;
      }

      ImmutableStringBuilder& padWithDefault() {
	return pad(Char(' '));
      }
      
      ImmutableStringBuilder& addSign() {
	setFormatFlags_(FORMAT_NUMBER_ADD_SIGN_);
	return *this;
      }

      ImmutableStringBuilder& noSign() {
	clearFormatFlag_(FORMAT_NUMBER_ADD_SIGN_);
      }
      
      ImmutableStringBuilder& precision(uint32_t n) {
	fieldPrecision_ = n;
	setFormatFlags_(FORMAT_FLOAT_PRECISION_);
	return *this;
      }

      ImmutableStringBuilder& noPrecision() {
	clearFormatFlag_(FORMAT_FLOAT_PRECISION_);
	return *this;
      }

      ImmutableStringBuilder& exp() {
	setFloatFormat_(FORMAT_FLOAT_EXP_LOWER_);
	return *this;
      }

      ImmutableStringBuilder& expLower() {
	setFloatFormat_(FORMAT_FLOAT_EXP_LOWER_);
	return *this;
      }
      
      ImmutableStringBuilder& expUpper() {
	setFloatFormat_(FORMAT_FLOAT_EXP_UPPER_);
	return *this;
      }

      ImmutableStringBuilder& expGeneral() {
	setFloatFormat_(FORMAT_FLOAT_EXP_GENERAL_);
	return *this;
      }

      ImmutableStringBuilder& expGeneralUpper() {
	setFloatFormat_(FORMAT_FLOAT_EXP_GENERAL_UPPER_);
	return *this;
      }

      ImmutableStringBuilder& hex() {
	setIntFormat_(FORMAT_INT_HEX_LOWER_);
	return *this;
      }

      ImmutableStringBuilder& hexLower() {
	setIntFormat_(FORMAT_INT_HEX_LOWER_);
	return *this;
      }

      ImmutableStringBuilder& hexUpper() {
	setIntFormat_(FORMAT_INT_HEX_UPPER_);
	return *this;
      }
      
      ImmutableStringBuilder& oct() {
	setIntFormat_(FORMAT_INT_OCTAL_);
	return *this;
      }

      ImmutableStringBuilder& dec() {
	setFormatFlags_(FORMAT_INT_DECIMAL_ | FORMAT_FLOAT_DECIMAL_,
			~(FORMAT_INT_MASK_|FORMAT_FLOAT_MASK_));
	return *this;
      }

      ImmutableStringBuilder& resetFormat() {
	flags_ = DEFAULT_FORMAT_FLAGS_;
	fieldWidth_ = 0;
	fieldPrecision_ = 0;
	fieldPadding_ = ' ';
	return *this;
      }
      
      void reset() {
	resetFormat();
	end_ = text_ ? text_->text : nullptr;
      }

      StringType done() {
	StringType tmp = makeString_();

	resetFormat();
	text_.reset();
	end_ = eos_ = nullptr;
	return std::move(tmp);
      }

      ImmutableStringBuilder& operator=(const ImmutableStringBuilder&) =
          default;
      ImmutableStringBuilder& operator=(ImmutableStringBuilder&&) = default;

    private:
      static constexpr const uint32_t FORMAT_FIELD_WIDTH_ = 0x1;
      static constexpr const uint32_t FORMAT_NUMBER_ADD_SIGN_ = 0x2;
      static constexpr const uint32_t FORMAT_FIELD_JUSTIFY_LEFT_ = 0x4;
      static constexpr const uint32_t FORMAT_FIELD_JUSTIFY_RIGHT_ = 0x0;
      static constexpr const uint32_t FORMAT_FIELD_JUSTIFY_CENTER_ = 0x8;
      static constexpr const uint32_t FORMAT_INT_DECIMAL_ = 0x0;
      static constexpr const uint32_t FORMAT_INT_OCTAL_ = 0x10;
      static constexpr const uint32_t FORMAT_INT_HEX_LOWER_ = 0x20;
      static constexpr const uint32_t FORMAT_INT_HEX_UPPER_ = 0x30;
      static constexpr const uint32_t FORMAT_FLOAT_DECIMAL_ = 0x0;
      static constexpr const uint32_t FORMAT_FLOAT_EXP_LOWER_ = 0x40;
      static constexpr const uint32_t FORMAT_FLOAT_EXP_UPPER_ = 0x80;
      static constexpr const uint32_t FORMAT_FLOAT_EXP_GENERAL_ = 0x100;
      static constexpr const uint32_t FORMAT_FLOAT_EXP_GENERAL_UPPER_ = 0x180;
      static constexpr const uint32_t FORMAT_FLOAT_PRECISION_ = 0x200;

      static constexpr const uint32_t FORMAT_JUSTIFY_MASK_ = 0x0C;
      static constexpr const uint32_t FORMAT_INT_MASK_ = 0x30;
      static constexpr const uint32_t FORMAT_FLOAT_MASK_ = 0x1C0;

      static constexpr const uint32_t DEFAULT_FORMAT_FLAGS_ =
	  FORMAT_FIELD_JUSTIFY_RIGHT_ | FORMAT_INT_DECIMAL_ |
	  FORMAT_FLOAT_DECIMAL_;
      
    private:
      StringTextPtr text_;
      Char* end_;
      Char* eos_;
      uint32_t flags_;
      uint32_t fieldWidth_;
      uint32_t fieldPrecision_;
      Char fieldPadding_;

      void increaseSize_(size_t minSize) {
	static const size_t MIN_ALLOC_SIZE = 8;
	static const size_t MAX_ALLOC_SIZE =
	    std::numeric_limits<uint32_t>::max();
	if (minSize > MAX_ALLOC_SIZE) {
	  throw std::bad_alloc();
	} else if (allocated() < MAX_ALLOC_SIZE) {
	  size_t newSize = std::max(std::min(allocated() << 1, MAX_ALLOC_SIZE),
				    MIN_ALLOC_SIZE);
	  while (newSize < minSize) {
	    newSize = std::min(newSize << 1, MAX_ALLOC_SIZE);
	  }

	  StringTextPtr newText = StringTextPtr::create(newSize, allocator());
	  if (text_) {
	    std::copy(text_->text, end_, newText->text);
	  }
	  end_ = newText->text + size();
	  eos_ = newText->text + newSize;
	  text_ = newText;
	}
      }

      StringType makeString_() {
	if (!text_) {
	  return StringType(allocator());
	} else if (end_ < eos_) {
	  return StringType((Char*)(text_->text), end_, allocator());
	} else {
	  return StringType(std::move(text_), text_->text, end_);
	}
      }

      void setFormatFlags_(uint32_t flags, uint32_t mask = 0xFFFFFFFF) {
	flags_ = (flags_ & mask) | flags;
      }

      void clearFormatFlag_(uint32_t flag) {
	flags_ = flags_ & ~flag;
      }

      void setJustification_(uint32_t j) {
	setFormatFlags_(j, ~FORMAT_JUSTIFY_MASK_);
      }

      void setIntFormat_(uint32_t f) {
	setFormatFlags_(f, ~FORMAT_INT_MASK_);
      }
      
      void setFloatFormat_(uint32_t f) {
	setFormatFlags_(f, ~FORMAT_FLOAT_MASK_);
      }
      
      template <typename Integer>
      std::string formatInt_(Integer i) const {
	std::ostringstream tmp;
	std::ios_base::fmtflags flags =
	    (flags_ & FORMAT_NUMBER_ADD_SIGN_) ? std::ios::showpos
	                                       : std::ios_base::fmtflags(0);
	switch (flags_ & FORMAT_INT_MASK_) {
	  case FORMAT_INT_DECIMAL_: flags |= std::ios::dec; break;
	  case FORMAT_INT_OCTAL_: flags |= std::ios::oct; break;
	  case FORMAT_INT_HEX_LOWER_ :
	    flags |= (std::ios::hex & ~std::ios::uppercase);
	    break;
	    
	  case FORMAT_INT_HEX_UPPER_ :
	    flags |= std::ios::hex | std::ios::uppercase;
	    break;
	    
	  default:
	    assert(false);
	}

	tmp.flags(flags);
	tmp << i;
	return tmp.str();
      }

      template <typename Float>
      std::string formatFloat_(Float f) const {
	std::ostringstream tmp;
	std::ios_base::fmtflags iosFlags =
	    (flags_ & FORMAT_NUMBER_ADD_SIGN_) ? std::ios::showpos
	                                       : std::ios::fmtflags(0);
	uint32_t p = (flags_ & FORMAT_FLOAT_PRECISION_) ? fieldPrecision_ : 6;

	switch (flags_ & FORMAT_FLOAT_MASK_) {
	  case FORMAT_FLOAT_DECIMAL_:
	    if (flags_ & FORMAT_FLOAT_PRECISION_) {
	      iosFlags |= std::ios::fixed;
	      tmp.precision(p);
	    }
	    break;

	  case FORMAT_FLOAT_EXP_LOWER_:
	    iosFlags |= std::ios::scientific;
	    tmp.precision(p);
	    break;

	  case FORMAT_FLOAT_EXP_UPPER_:
	    iosFlags |= std::ios::scientific | std::ios::uppercase;
	    tmp.precision(p);
	    break;
	    
	  case FORMAT_FLOAT_EXP_GENERAL_:
	    std::tie(iosFlags, p) = deduceGeneralFlags_(f, iosFlags, p);
	    tmp.precision(p);
	    break;

	  case FORMAT_FLOAT_EXP_GENERAL_UPPER_:
	    std::tie(iosFlags, p) = deduceGeneralFlags_(f, iosFlags, p);
	    iosFlags |= std::ios::uppercase;
	    tmp.precision(p);
	    break;

	  default:
	    assert(false);
	}

	tmp.flags(iosFlags);
	tmp << f;
	return tmp.str();
      }

      template <typename Iterator>
      void put_(const Iterator& begin, const Iterator& end) {
	const size_t n = std::distance(begin, end);
	
	if ((!(flags_ & FORMAT_FIELD_WIDTH_)) || (n >= fieldWidth_)) {
	  if ((end_ + n) > eos_) {
	    increaseSize_(size() + n);
	  }
	  std::copy(begin, end, end_);
	  end_ += n;
	} else {
	  size_t padLeft = 0;
	  size_t padRight = 0;

	  switch (flags_ & FORMAT_JUSTIFY_MASK_) {
	    case FORMAT_FIELD_JUSTIFY_LEFT_:
	      padRight = fieldWidth_ - n;
	      break;

	    case FORMAT_FIELD_JUSTIFY_RIGHT_:
	      padLeft = fieldWidth_ - n;
	      break;

	    case FORMAT_FIELD_JUSTIFY_CENTER_:
	      padRight = (fieldWidth_ - n) >> 1;
	      padLeft = fieldWidth_ - n - padRight;
	      break;

	    default:
	      assert(0);
	  }

	  if ((end_ + fieldWidth_) > eos_) {
	    increaseSize_(size() + fieldWidth_);
	  }

	  while (padLeft) {
	    *end_++ = fieldPadding_;
	    --padLeft;
	  }
	  std::copy(begin, end, end_);
	  end_ += n;
	  while (padRight) {
	    *end_++ = fieldPadding_;
	    --padRight;
	  }
	}
      }
      
      template <typename Float>
      std::tuple<std::ios_base::fmtflags, uint32_t> deduceGeneralFlags_(
	  Float f, std::ios_base::fmtflags iosFlags, uint32_t p
      ) const {
	int32_t e = getExponent_(f);
	if ((e <= -4) || !(flags_ & FORMAT_FLOAT_PRECISION_) ||
	    (e >= (int32_t)p)) {
	  return std::make_tuple(iosFlags | std::ios::scientific, p - 1);
	} else {
	  return std::make_tuple(iosFlags | std::ios::fixed,
				 (uint32_t)(p - e - 1));
	}
      }
      
      template <typename Float>
      static int32_t getExponent_(Float f) {
	return (int32_t)std::floor(std::log10(std::fabs(f)));
      }
    };

    template <typename C1, typename T1, typename A1,
	      typename C2, typename T2, typename A2>
    ImmutableStringBuilder<C1, T1, A1>& operator<<(
	ImmutableStringBuilder<C1, T1, A1>& builder,
	const ImmutableString<C2, T2, A2>& s
    ) {
      return builder.append(s);
    }

    template <typename C1, typename T1, typename A1,
	      typename C2, typename T2, typename A2>
    ImmutableStringBuilder<C1, T1, A1>& operator<<(
	ImmutableStringBuilder<C1, T1, A1>& builder,
	const std::basic_string<C2, T2, A2>& s
    ) {
      return builder.append(s);
    }

    template <typename C1, typename T1, typename A1, typename C2, size_t N>
    ImmutableStringBuilder<C1, T1, A1>& operator<<(
	ImmutableStringBuilder<C1, T1, A1>& builder, C2 (&s)[N]
    ) {
      return builder.append(s);
    }

    template <typename C1, typename T1, typename A1, typename C2,
	      typename Enabled =
	          typename std::enable_if<
	              std::is_integral<C2>::value ||
		          std::is_floating_point<C2>::value ||
		          pistis::typeutil::IsCharType<C2>::value,
		      ImmutableStringBuilder<C1, T1, A1>
		  >::type>
    Enabled& operator<<(ImmutableStringBuilder<C1, T1, A1>& builder,
			C2 value) {
      return builder.append(value);
    }

    template <typename C, typename T, typename A>
    ImmutableStringBuilder<C, T, A>& operator<<(
	ImmutableStringBuilder<C, T, A>& builder,
	ImmutableStringBuilder<C, T, A>& (&f)(
	    ImmutableStringBuilder<C, T, A>&
        )
    ) {
      return f(builder);
    }

    typedef ImmutableStringBuilder<char>      IStringBuilder;
    typedef ImmutableStringBuilder<wchar_t>   WIStringBuilder;
    typedef IStringBuilder                    U8_IStringBuilder;
    typedef ImmutableStringBuilder<char16_t>  U16_IStringBuilder;
    typedef ImmutableStringBuilder<char32_t>  U32_IStringBuilder;

  }
}
#endif
