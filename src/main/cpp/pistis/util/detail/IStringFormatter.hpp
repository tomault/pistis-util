#ifndef __PISTIS__UTIL__DETAIL__ISTRINGFORMATTER_HPP__
#define __PISTIS__UTIL__DETAIL__ISTRINGFORMATTER_HPP__

#include <pistis/typeutil/ExtendedTypeTraits.hpp>
#include <pistis/typeutil/NameOf.hpp>
#include <pistis/util/IStringBuilder_.hpp>
#include <pistis/util/FormatIStringError.hpp>
#include <algorithm>
#include <ostream>
#include <sstream>
#include <type_traits>
#include <stdint.h>

namespace pistis {
  namespace util {
    template <typename Char, typename CharTraits, typename Allocator>
    class ImmutableString;
    

    namespace detail {
      namespace isf {
	/** @brief Transform a format specifier to a std::string so it can
	 *         be read easily in error messages
	 *
	 *  This overload accepts characters with a width larger than one
	 *  byte and encodes them into a byte string using UTF-8.
	 *
	 *  @param begin  Start of the format specifier
	 *  @param end    End of the format specifier
	 *  @returns      The format specifier encoded in UTF-8
	 *  @throws       Does not throw (except on allocation errors)
	 */
	template <typename Char>
	std::string encodeFormatSpecifier(const Char* begin, const Char* end) {
	  std::ostringstream spec;
	  for (const Char* p = begin; p != end; ++p) {
	    const auto c = std::char_traits<Char>::to_int_type(*p);

	    // Encode to UTF-8 inline.  Not the fastest conversion, but
	    // encodeFormatSpecifier() is called from error routines
	    // so that's ok.  The extra masking done on the lead byte
	    // is to prevent the sign bit from clobbering the high
	    // bits in the lead byte T::int_type is signed and the
	    // conversion to T::int_type produces a negative value.
	    // The encoding algorithm for UTF-8 can only encode integers
	    // up to 31 bits, so anything larger than 31 bits is lost.
	    // Since the current Unicode standard has 17 planes, making
	    // legal characters 21 bits or less, this shouldn't be a
	    // problem.
	    if (c < 0x80) {
	      spec.put((char)c);
	    } else if (c < 0x800) {
	      spec.put((char)(0xC0|((c >> 6) & 0x1F)));
	      spec.put((char)(0x80|(c & 0x1F)));
	    } else if (c < 0x10000) {
	      spec.put((char)(0xE0|((c >> 12) & 0x0F)));
	      spec.put((char)(0x80|((c >>  6) & 0x1F)));
	      spec.put((char)(0x80|(c & 0x1F)));
	    } else if (c < 0x200000) {
	      spec.put((char)(0xF0|((c >> 18) & 0x07)));
	      spec.put((char)(0x80|((c >> 12) & 0x1F)));
	      spec.put((char)(0x80|((c >>  6) & 0x1F)));
	      spec.put((char)(0x80|(c & 0x1F)));
	    } else if (c < 0x4000000) {
	      spec.put((char)(0xF8|((c >> 24) & 0x03)));
	      spec.put((char)(0x80|((c >> 18) & 0x1F)));
	      spec.put((char)(0x80|((c >> 12) & 0x1F)));
	      spec.put((char)(0x80|((c >>  6) & 0x1F)));
	      spec.put((char)(0x80|(c & 0x1F)));
	    } else {
	      spec.put((char)(0xFC|((c >> 30) & 0x01)));
	      spec.put((char)(0x80|((c >> 24) & 0x1F)));
	      spec.put((char)(0x80|((c >> 18) & 0x1F)));
	      spec.put((char)(0x80|((c >> 12) & 0x1F)));
	      spec.put((char)(0x80|((c >>  6) & 0x1F)));
	      spec.put((char)(0x80|(c & 0x1F)));
	    }
	  }
	  return spec.str();
	}

	template <typename Char>
	struct FormatSpecifier {
	  enum Type {
	    PERCENT,
	    INTEGER,
	    OCTAL,
	    HEX,
	    FLOAT,
	    EXPONENTIAL,
	    GENERAL_FLOAT,
	    STRING
	  };

	  enum Justification {
	    LEFT,
	    CENTER,
	    RIGHT
	  };

	  Type type;
	  Justification justification;
	  Char pad;
	  int32_t width;
	  int32_t precision;
	  bool useUppercase;
	  bool addSign;

	  FormatSpecifier():
	    FormatSpecifier(PERCENT, RIGHT, Char(' '), -1, -1, false, false) {
	  }
	  
	  FormatSpecifier(Type t, Justification j, Char pd, int32_t w,
			  int32_t pr, bool uc, bool sign):
	      type(t), justification(j), pad(pd), width(w), precision(pr),
	      useUppercase(uc), addSign(sign) {
	  }

	  template <typename C, typename T, typename A>
	  const FormatSpecifier& setWidth(
	      ImmutableStringBuilder<C, T, A>& builder
	  ) const {
	    if (this->width >= 0) {
	      builder.width(this->width).pad(pad);
	    } else {
	      builder.noWidth();
	    }
	    return *this;
	  }

	  template <typename C, typename T, typename A>
	  const FormatSpecifier& setPrecision(
	      ImmutableStringBuilder<C, T, A>& builder
	  ) const {
	    if (this->precision >= 0) {
	      builder.precision(this->precision);
	    } else {
	      builder.noPrecision();
	    }
	    return *this;
	  }

	  template <typename C, typename T, typename A>
	  const FormatSpecifier& setJustification(
	      ImmutableStringBuilder<C, T, A>& builder
	  ) const {
	    switch(justification) {
	      case LEFT: builder.left(); break;
	      case CENTER: builder.center(); break;
	      case RIGHT: break;
	      default: assert(false);
	    }
	    return *this;
	  }

	  template <typename C, typename T, typename A>
	  const FormatSpecifier& setAddSign(
	      ImmutableStringBuilder<C, T, A>& builder
	  ) const {
	    if (this->addSign) {
	      builder.addSign();
	    } else {
	      builder.noSign();
	    }
	    return *this;
	  }

	  const std::string& baseFormatString() const {
	    static const std::string PERCENT_FORMAT("%%");
	    static const std::string INTEGER_FORMAT("%d");
	    static const std::string OCTAL_FORMAT("%o");
	    static const std::string HEX_FORMAT("%x");
	    static const std::string HEX_UPPER_FORMAT("%X");
	    static const std::string FLOAT_FORMAT("%f");
	    static const std::string EXP_FORMAT("%e");
	    static const std::string EXP_UPPER_FORMAT("%E");
	    static const std::string GENERAL_FLOAT_FORMAT("%g");
	    static const std::string GENERAL_FLOAT_UPPER_FORMAT("%G");
	    static const std::string STRING_FORMAT("%s");
	    
	    switch (type) {
	      case PERCENT: return PERCENT_FORMAT;
	      case INTEGER: return INTEGER_FORMAT;
	      case OCTAL: return OCTAL_FORMAT;
	      case HEX: return useUppercase ? HEX_UPPER_FORMAT : HEX_FORMAT;
	      case FLOAT: return FLOAT_FORMAT;
	      case EXPONENTIAL:
		return useUppercase ? EXP_UPPER_FORMAT : EXP_FORMAT;
	      case GENERAL_FLOAT:
		return useUppercase ? GENERAL_FLOAT_UPPER_FORMAT
		                    : GENERAL_FLOAT_FORMAT;
	      case STRING: return STRING_FORMAT;
	      default: assert(false); 
	    }
	  }

	  const std::string& argumentTypeName() const {
	    static const std::string EMPTY("");
	    static const std::string INTEGER_NAME("integer");
	    static const std::string FLOAT_NAME("floating-point number");
	    static const std::string STRING_NAME("string");

	    switch (type) {
	      case PERCENT:
		return EMPTY;

	      case INTEGER:
	      case OCTAL:
	      case HEX:
		return INTEGER_NAME;

	      case FLOAT:
	      case EXPONENTIAL:
	      case GENERAL_FLOAT:
		return FLOAT_NAME;

	      case STRING:
		return STRING_NAME;

	      default:
		assert(false);
	    }
	  }
	  
	  static const Char* parse(const Char* start, const Char* end,
				   FormatSpecifier<Char>& spec) {
	    assert(start < end);
	    assert(*start == Char('%'));

	    // Parse flags
	    const Char* p = start + 1;
	    if ((p < end) && (*p == '%')) {
	      spec.type = PERCENT;
	      return p + 1;
	    }

	    p = parseFlags_(spec, start + 1, end);
	    if (p == end) {
	      throw FormatIStringError::invalidFormatSpecifier(
		  encodeFormatSpecifier(start, end), PISTIS_EX_HERE
	      );
	    }

	    // Parse width
	    p = parseWidth_(spec, p, end);
	    if (p == end) {
	      throw FormatIStringError::invalidFormatSpecifier(
		  encodeFormatSpecifier(start, end), PISTIS_EX_HERE
	      );
	    }

	    // Parse precision
	    p = parsePrecision_(spec, p, end);
	    if (p == end) {
	      throw FormatIStringError::invalidFormatSpecifier(
		  encodeFormatSpecifier(start, end), PISTIS_EX_HERE
	      );
	    }

	    // Parse type
	    p = parseType_(spec, p, end);
	    if (!p) {
	      throw FormatIStringError::invalidFormatSpecifier(
		  encodeFormatSpecifier(start, end), PISTIS_EX_HERE
	      );
	    }
	    return p;
	  }

	private:
	  static const Char* parseFlags_(FormatSpecifier& spec, const Char* p,
					 const Char* end) {
	    while (p < end) {
	      if (*p == Char('-')) {
		spec.justification = LEFT;
	      } else if (*p == Char('+')) {
		spec.addSign = true;
	      } else if (*p == Char('0')) {
		spec.pad = Char('0');
	      } else if (*p == Char('^')) {
		spec.justification = CENTER;
	      } else {
		break;
	      }
	      ++p;
	    }
	    return p;
	  }

	  static const Char* parseWidth_(FormatSpecifier& spec, const Char* p,
					 const Char* end) {
	    if ((p < end) && isDigit_(*p)) {
	      int32_t w = toDigit_(*p++);
	      while ((p < end) && isDigit_(*p)) {
		w = w * 10 + toDigit_(*p);
		++p;
	      }
	      spec.width = w;
	    }
	    return p;
	  }

	  static const Char* parsePrecision_(FormatSpecifier& spec,
					     const Char* p, const Char* end) {
	    if ((p < end) && (*p == '.')) {
	      const Char* const s = ++p;
	      int32_t precision  = 0;
	      while ((p < end) && isDigit_(*p)) {
		precision = precision * 10 + toDigit_(*p);
		++p;
	      }
	      if (p == s) {
		return end;
	      }
	      spec.precision = precision;
	    }
	    return p;
	  }

	  static const Char* parseType_(FormatSpecifier& spec,
					const Char* p, const Char* end) {
	    if (*p == Char('s')) {
	      spec.type = STRING;
	    } else if (*p == Char('d')) {
	      spec.type = INTEGER;
	    } else if (*p == Char('o')) {
	      spec.type = OCTAL;
	    } else if (*p == Char('x')) {
	      spec.type = HEX;
	    } else if (*p == Char('X')) {
	      spec.type = HEX;
	      spec.useUppercase = true;
	    } else if (*p == Char('f')) {
	      spec.type = FLOAT;
	    } else if (*p == Char('e')) {
	      spec.type = EXPONENTIAL;
	    } else if (*p == Char('E')) {
	      spec.type = EXPONENTIAL;
	      spec.useUppercase = true;
	    } else if (*p == Char('g')) {
	      spec.type = GENERAL_FLOAT;
	    } else if (*p == Char('G')) {
	      spec.type = GENERAL_FLOAT;
	      spec.useUppercase = true;
	    } else {
	      return nullptr;
	    }
	    return p + 1;
	  }

	  static bool isDigit_(Char c) {
	    return (c >= Char('0')) && (c <= Char('9'));
	  }

	  static int32_t toDigit_(Char c) { return c - Char('0'); }
	};

	class StringFormatter {
	private:
	  struct IntTag { };
	  struct FloatTag { };
	  struct CharTag { };
	  struct ObjectTag { };

	public:
	  template <typename C1, typename T1, typename A1,
		    typename C2, typename T2, typename A2>
	  ImmutableStringBuilder<C1, T1, A1>& operator()(
	      ImmutableStringBuilder<C1, T1, A1>& builder,
	      const ImmutableString<C2, T2, A2>& s,
	      const FormatSpecifier<C1>& spec
	  ) const {
	    spec.setWidth(builder).setJustification(builder);
	    return builder.append(s);
	  }

	  template <typename C1, typename T1, typename A1,
		    typename C2, typename T2, typename A2>
	  ImmutableStringBuilder<C1, T1, A1>& operator()(
	      ImmutableStringBuilder<C1, T1, A1>& builder,
	      const std::basic_string<C2, T2, A2>& s,
	      const FormatSpecifier<C1>& spec
	  ) const {
	    spec.setWidth(builder).setJustification(builder);
	    return builder.append(s);
	  }
	  
	  template <typename C1, typename T1, typename A1,
		    typename C2, size_t N>
	  ImmutableStringBuilder<C1, T1, A1>& operator()(
	      ImmutableStringBuilder<C1, T1, A1>& builder, C2 (&s)[N],
	      const FormatSpecifier<C1>& spec
	  ) const {
	    spec.setWidth(builder).setJustification(builder);
	    return builder.append(s);
	  }

	  template <typename C1, typename T1, typename A1, typename C2,
		    typename Enabled =
		        typename std::enable_if<
		            pistis::typeutil::IsCharType<C2>::value,
		            ImmutableStringBuilder<C1, T1, A1>
		        >::type>
	  Enabled& operator()(ImmutableStringBuilder<C1, T1, A1>& builder,
			      C2* const (&s),
			      const FormatSpecifier<C1>& spec) const {
	    spec.setWidth(builder).setJustification(builder);
	    return builder.append(s);
	  }

	  template <typename C, typename T, typename A, typename V,
		    typename Enabled =
		        typename std::enable_if<
		            pistis::typeutil::IsCharType<V>::value, CharTag
		        >::type>
	  ImmutableStringBuilder<C, T, A>& operator()(
	      ImmutableStringBuilder<C, T, A>& builder, const V& value,
	      const FormatSpecifier<C>& spec, CharTag = Enabled()
	  ) const {
	    spec.setWidth(builder).setJustification(builder);
	    return builder.append(value);
	  }
	  
	  template <typename C, typename T, typename A, typename V,
		    typename Enabled =
		        typename std::enable_if<
		            std::is_integral<V>::value &&
		                !pistis::typeutil::IsCharType<V>::value,
		            IntTag
		        >::type>
	  ImmutableStringBuilder<C, T, A>& operator()(
	      ImmutableStringBuilder<C, T, A>& builder, const V& value,
	      const FormatSpecifier<C>& spec, IntTag = Enabled()
	  ) const {
	    spec.setWidth(builder).setJustification(builder);
	    return builder.dec().append(value);
	  }

	  template <typename C, typename T, typename A, typename V,
		    typename Enabled =
		        typename std::enable_if<
		            std::is_floating_point<V>::value, FloatTag
		        >::type>
	  ImmutableStringBuilder<C, T, A>& operator()(
	      ImmutableStringBuilder<C, T, A>& builder, const V& value,
	      const FormatSpecifier<C>& spec, FloatTag = Enabled()
	  ) const {
	    spec.setWidth(builder).setJustification(builder);
	    return builder.dec().append(value);
	  }

	  template <typename C, typename T, typename A, typename V,
		    typename Enabled =
		        typename std::enable_if<
		            !std::is_integral<V>::value &&
		                 !pistis::typeutil::IsCharType<V>::value &&
		                 !std::is_floating_point<V>::value,
		            ObjectTag
		    >::type>
	  ImmutableStringBuilder<C, T, A>& operator()(
	      ImmutableStringBuilder<C, T, A>& builder, const V& value,
	      const FormatSpecifier<C>& spec, ObjectTag = Enabled()
	  ) const {
	    typedef typename std::allocator_traits<A>::template rebind_alloc<C>
	            CharAllocator;
	    std::basic_ostringstream<C, T, CharAllocator> tmp;
	    tmp << value;
	    spec.setWidth(builder).setJustification(builder);
	    return builder.append(tmp.str());
	  }
	};

	class IntegerFormatter {
	private:
	  struct CharTag { };
	  struct IntTag { };
	  struct ObjectTag { };
	public:
	  template <typename C, typename T, typename A, typename V,
		    typename Enabled =
		        typename std::enable_if<
		            std::is_integral<V>::value &&
		      !pistis::typeutil::IsCharType<V>::value,
		            IntTag 
		        >::type>
	  ImmutableStringBuilder<C, T, A>& operator()(
	      ImmutableStringBuilder<C, T, A>& builder, const V& value,
	      const FormatSpecifier<C>& spec, IntTag = Enabled()
	  ) const {
	    spec.setWidth(builder).setJustification(builder)
	        .setAddSign(builder);
	    return builder.append(value);
	  }

	  template <typename C, typename T, typename A, typename V,
		    typename Enabled =
		        typename std::enable_if<
		            pistis::typeutil::IsCharType<V>::value, CharTag
		        >::type>
	  ImmutableStringBuilder<C, T, A>& operator()(
	      ImmutableStringBuilder<C, T, A>& builder, const V& value,
	      const FormatSpecifier<C>& spec, CharTag = Enabled()
	  ) const {
	    spec.setWidth(builder).setJustification(builder)
	        .setAddSign(builder);
	    return builder.append((long)value);
	  }


	  template <typename C, typename T, typename A, typename V,
		    typename Enabled =
		        typename std::enable_if<
			    !std::is_integral<V>::value, ObjectTag
			>::type>
	  ImmutableStringBuilder<C, T, A>& operator()(
	      ImmutableStringBuilder<C, T, A>& builder, const V& value,
	      const FormatSpecifier<C>& spec, ObjectTag = Enabled()) const {
	    std::string typeName = pistis::typeutil::nameOf<V>();
	    throw FormatIStringError::incorrectType(spec.baseFormatString(),
						    spec.argumentTypeName(),
						    typeName, PISTIS_EX_HERE);
	  }
	};

	class FloatFormatter {
	private:
	  struct IntTag { };
	  struct FloatTag { };
	  struct ObjectTag { };

	public:
	  template <typename C, typename T, typename A, typename V,
		    typename Enabled =
		        typename std::enable_if<
		            std::is_integral<V>::value, IntTag
		        >::type>
	  ImmutableStringBuilder<C, T, A>& operator()(
	      ImmutableStringBuilder<C, T, A>& builder, const V& value,
	      const FormatSpecifier<C>& spec, IntTag = Enabled()
	  ) const {
	    spec.setWidth(builder).setJustification(builder)
	        .setPrecision(builder).setAddSign(builder);
	    return builder.append((double)value);
	  }

	  template <typename C, typename T, typename A, typename V,
		    typename Enabled =
		        typename std::enable_if<
		            std::is_floating_point<V>::value, FloatTag
		        >::type>
	  ImmutableStringBuilder<C, T, A>& operator()(
	      ImmutableStringBuilder<C, T, A>& builder, const V& value,
	      const FormatSpecifier<C>& spec, FloatTag = Enabled()
	  ) const {
	    spec.setWidth(builder).setJustification(builder)
	        .setPrecision(builder).setAddSign(builder);
	    return builder.append(value);
	  }

	  template <typename C, typename T, typename A, typename V,
		    typename Enabled =
		        typename std::enable_if<
		            !std::is_integral<V>::value &&
		                !std::is_floating_point<V>::value,
		            ObjectTag
			>::type>
	  ImmutableStringBuilder<C, T, A>& operator()(
	      ImmutableStringBuilder<C, T, A>& builder, const V& value,
	      const FormatSpecifier<C>& spec, ObjectTag = Enabled()
	  ) const {
	    std::string typeName = pistis::typeutil::nameOf<V>();
	    throw FormatIStringError::incorrectType(spec.baseFormatString(),
						    spec.argumentTypeName(),
						    typeName, PISTIS_EX_HERE);
	  }
	};
      }

      template <typename Char, typename CharTraits, typename Allocator>
      auto formatIString(
	  ImmutableStringBuilder<Char, CharTraits, Allocator>& builder,
	  const Char* fmt, const Char* fmtEnd, const Char* current
      ) {
	const Char* last = current;
	const Char* p = std::find(current, fmtEnd, Char('%'));
	isf::FormatSpecifier<Char> spec;

	builder.resetFormat();
	while (p < fmtEnd) {
	  const Char* before = p;
	  p = isf::FormatSpecifier<Char>::parse(p, fmtEnd, spec);
	  if (spec.type != isf::FormatSpecifier<Char>::PERCENT) {
	    throw FormatIStringError::notEnoughArguments(
		isf::encodeFormatSpecifier(fmt, fmtEnd), PISTIS_EX_HERE
	    );
	  }
	  builder.append(last, before).append('%');
	  last = p;
	  p = std::find(p, fmtEnd, Char('%'));
	}
	return builder.append(last, fmtEnd).done();
      }
      
      template <typename Char, typename CharTraits, typename Allocator,
		typename Arg, typename... Args>
      auto formatIString(
	  ImmutableStringBuilder<Char, CharTraits, Allocator>& builder,
	  const Char* fmt, const Char* fmtEnd, const Char* current,
	  Arg&& arg, Args&&... args
      ) {
	static const isf::StringFormatter FORMAT_STRING;
	static const isf::IntegerFormatter FORMAT_INTEGER;
	static const isf::FloatFormatter FORMAT_FLOAT;
	
	const Char* p = std::find(current, fmtEnd, Char('%'));
	isf::FormatSpecifier<Char> spec;

	builder.resetFormat();
	while (p <= fmtEnd) {
	  if (p == fmtEnd) {
	    throw FormatIStringError::notAllArgumentsConverted(
		isf::encodeFormatSpecifier(fmt, fmtEnd), PISTIS_EX_HERE
	    );
	  }
	  builder.append(current, p);
	  p = isf::FormatSpecifier<Char>::parse(p, fmtEnd, spec);
	  if (spec.type != isf::FormatSpecifier<Char>::PERCENT) {
	    break;
	  }
	  builder.append(Char('%'));
	  current = p;
	  p = std::find(current, fmtEnd, Char('%'));
	}

	switch (spec.type) {
	  case isf::FormatSpecifier<Char>::PERCENT:
	    assert(false); // Should have been handled above
	    break;
	    
	  case isf::FormatSpecifier<Char>::INTEGER:
	    FORMAT_INTEGER(builder.dec(), std::forward<Arg>(arg), spec);
	    break;

	  case isf::FormatSpecifier<Char>::OCTAL:
	    FORMAT_INTEGER(builder.oct(), std::forward<Arg>(arg), spec);
	    break;
	  
	  case isf::FormatSpecifier<Char>::HEX:
	    if (spec.useUppercase) {
	      FORMAT_INTEGER(builder.hexUpper(), std::forward<Arg>(arg), spec);
	    } else {
	      FORMAT_INTEGER(builder.hex(), std::forward<Arg>(arg), spec);
	    }
	    break;

	  case isf::FormatSpecifier<Char>::FLOAT:
	    FORMAT_FLOAT(builder.dec(), std::forward<Arg>(arg), spec);
	    break;

	  case isf::FormatSpecifier<Char>::EXPONENTIAL:
	    if (spec.useUppercase) {
	      FORMAT_FLOAT(builder.expUpper(), std::forward<Arg>(arg), spec);
	    } else {
	      FORMAT_FLOAT(builder.exp(), std::forward<Arg>(arg), spec);
	    }
	    break;

	  case isf::FormatSpecifier<Char>::GENERAL_FLOAT:
	    if (spec.useUppercase) {
	      FORMAT_FLOAT(builder.expGeneralUpper(), std::forward<Arg>(arg),
			   spec);
	    } else {
	      FORMAT_FLOAT(builder.expGeneral(), std::forward<Arg>(arg), spec);
	    }
	    break;

	  case isf::FormatSpecifier<Char>::STRING:
	    FORMAT_STRING(builder, std::forward<Arg>(arg), spec);
	    break;

	  default:
	    assert(0);
	}

	return formatIString(builder, fmt, fmtEnd, p,
			     std::forward<Args>(args)...);
      }
      
    }
  }
}

#endif
