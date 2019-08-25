#include <pistis/util/detail/IStringFormatter.hpp>
#include <pistis/util/IString.hpp>
#include <pistis/util/FormatIStringError.hpp>
#include <gtest/gtest.h>

using namespace pistis::util;
using namespace pistis::util::detail;

namespace {
  template <typename Char>
  const std::string& justificationChar(
      typename isf::FormatSpecifier<Char>::Justification j
  ) {
    static const std::string LEFT("-");
    static const std::string CENTER("^");
    static const std::string RIGHT("");

    switch(j) {
      case isf::FormatSpecifier<Char>::LEFT: return LEFT;
      case isf::FormatSpecifier<Char>::CENTER: return CENTER;
      case isf::FormatSpecifier<Char>::RIGHT: return RIGHT;
      default: assert(false);
    }
  }
}

namespace pistis {
  namespace util {
    namespace detail {
      namespace isf {

	template <typename Char>
	std::ostream& operator<<(std::ostream& out,
				 const FormatSpecifier<Char>& s) {
	  out << "%" << justificationChar<Char>(s.justification)
	      << (s.addSign ? "+" : "");
	  if (s.pad != Char(' ')) {
	    out << s.pad;
	  }
	  if (s.width >= 0) {
	    out << s.width;
	  }
	  if (s.precision >= 0) {
	    out << "." << s.precision;
	  }
	  return out << s.baseFormatString()[1];
	}

	template <typename Char>
	bool operator==(const FormatSpecifier<Char>& left,
			const FormatSpecifier<Char>& right) {
	  return (left.type == right.type) &&
	         (left.justification == right.justification) &&
	         (left.pad == right.pad) && (left.width == right.width) &&
	         (left.precision == right.precision) &&
	         (left.useUppercase == right.useUppercase) &&
                 (left.addSign == right.addSign);
	}

	template <typename Char>
	bool operator!=(const FormatSpecifier<Char>& left,
			const FormatSpecifier<Char>& right) {
	  return (left.type != right.type) ||
                 (left.justification != right.justification) ||
	         (left.pad != right.pad) || (left.width != right.width) ||
                 (left.precision != right.precision) ||
                 (left.useUppercase != right.useUppercase) ||
                 (left.addSign != right.addSign);
	}

	
      }
    }
  }
}

namespace {
  struct Something { };
  inline std::ostream& operator<<(std::ostream& out, const Something&) {
    return out << "Something";
  }
  
  template <typename Char>
  ::testing::AssertionResult executeParseTest(
      const Char* fmt, const Char* fmtEnd,
      const isf::FormatSpecifier<Char>& truth,
      const Char* trueEnd
  ) {
    isf::FormatSpecifier<Char> spec;
    const char* p = isf::FormatSpecifier<Char>::parse(fmt, fmtEnd, spec);

    if (spec != truth) {
      return ::testing::AssertionFailure()
	  << "Parsing format specifier \""
	  << isf::encodeFormatSpecifier(fmt, fmtEnd) << "\" produced \""
	  << spec << "\" -- it should have produced \"" << truth << "\"";
    } else if (p != trueEnd) {
      return ::testing::AssertionFailure()
	  << "Parsing format specifier ended at " << (p - fmt)
	  << " -- it should have ended at " << (trueEnd - fmt);
    } else {
      return ::testing::AssertionSuccess();
    }
  }

  template <typename Char>
  ::testing::AssertionResult executeParseTest(
      const Char* fmt, const Char* fmtEnd,
      const isf::FormatSpecifier<Char>& truth
  ) {
    return executeParseTest(fmt, fmtEnd, truth, fmtEnd);
  }
  
  template <typename Char>
  ::testing::AssertionResult executeParseTest(
      const Char* fmt, const isf::FormatSpecifier<Char>& truth,
      const Char* trueEnd = nullptr
  ) {
    const Char* end = fmt;
    while (*end) {
      ++end;
    }
    return executeParseTest(fmt, end, truth, trueEnd ? trueEnd : end);
  }

  template <typename Char>
  isf::FormatSpecifier<Char> makeSpec(
      typename isf::FormatSpecifier<Char>::Type type =
          isf::FormatSpecifier<Char>::PERCENT,
      typename isf::FormatSpecifier<Char>::Justification justification =
          isf::FormatSpecifier<Char>::RIGHT,
      Char pad  = ' ', int32_t width = -1, int32_t precision = -1,
      bool useUppercase = false, bool addSign = false
  ) {
    return isf::FormatSpecifier<Char>(type, justification, pad, width,
				      precision, useUppercase, addSign);
  }

  template <typename Char>
  isf::FormatSpecifier<Char> makePercentSpec() {
    return makeSpec<Char>(isf::FormatSpecifier<Char>::PERCENT);
  }
  
  template <typename Char>
  isf::FormatSpecifier<Char> makeStringSpec(
      int32_t width = -1,
      typename isf::FormatSpecifier<Char>::Justification justification =
          isf::FormatSpecifier<Char>::RIGHT,
      Char pad = ' '
  ) {
    return makeSpec(isf::FormatSpecifier<Char>::STRING, justification,
		    pad, width);
  }

  template <typename Char>
  isf::FormatSpecifier<Char> makeIntSpec(
      int32_t width = -1,
      typename isf::FormatSpecifier<Char>::Justification justification =
          isf::FormatSpecifier<Char>::RIGHT,
      Char pad = ' ', bool addSign = false
  ) {
    return makeSpec(
       isf::FormatSpecifier<Char>::INTEGER, justification, pad, width, -1,
       false, addSign
    );
  }
					 
  template <typename Char>
  isf::FormatSpecifier<Char> makeOctalSpec(
      int32_t width = -1,
      typename isf::FormatSpecifier<Char>::Justification justification =
          isf::FormatSpecifier<Char>::RIGHT,
      Char pad = ' ', bool addSign = false
  ) {
    return makeSpec(
       isf::FormatSpecifier<Char>::OCTAL, justification, pad, width, -1,
       false, addSign
    );
  }

  template <typename Char>
  isf::FormatSpecifier<Char> makeHexSpec(
      int32_t width = -1,
      typename isf::FormatSpecifier<Char>::Justification justification =
          isf::FormatSpecifier<Char>::RIGHT,
      Char pad = ' ', bool useUppercase = false, bool addSign = false
  ) {
    return makeSpec(
       isf::FormatSpecifier<Char>::HEX, justification, pad, width, -1,
       useUppercase, addSign
    );
  }

  template <typename Char>
  isf::FormatSpecifier<Char> makeFloatSpec(
      int32_t width = -1, int32_t precision = -1,
      typename isf::FormatSpecifier<Char>::Justification justification =
          isf::FormatSpecifier<Char>::RIGHT,
      Char pad = ' ', bool addSign = false
  ) {
    return makeSpec(
       isf::FormatSpecifier<Char>::FLOAT, justification, pad, width, precision,
       false, addSign
    );
  }

  template <typename Char>
  isf::FormatSpecifier<Char> makeExpSpec(
      int32_t width = -1, int32_t precision = -1,
      typename isf::FormatSpecifier<Char>::Justification justification =
          isf::FormatSpecifier<Char>::RIGHT,
      Char pad = ' ', bool useUppercase = false, bool addSign = false
  ) {
    return makeSpec(
       isf::FormatSpecifier<Char>::EXPONENTIAL, justification, pad, width,
       precision, useUppercase, addSign
    );
  }

  template <typename Char>
  isf::FormatSpecifier<Char> makeGeneralFloatSpec(
      int32_t width = -1, int32_t precision = -1,
      typename isf::FormatSpecifier<Char>::Justification justification =
          isf::FormatSpecifier<Char>::RIGHT,
      Char pad = ' ', bool useUppercase = false, bool addSign = false
  ) {
    return makeSpec(
       isf::FormatSpecifier<Char>::GENERAL_FLOAT, justification, pad, width,
       precision, useUppercase, addSign
    );
  }

  
  template <typename C, typename T, typename A, typename Input>
  ::testing::AssertionResult testFormatString(
      const std::basic_string<C, T, A>& truth, const Input& input,
      const isf::FormatSpecifier<C>& spec
  ) {
    isf::StringFormatter format;
    ImmutableStringBuilder<C, T> builder;

    const ImmutableStringBuilder<C, T>& result =
        format(builder, input, spec);
    if (&result != &builder) {
      return ::testing::AssertionFailure()
	  << "Return value from format() is incorrect.  It is " << &result
	  << ", but it should be " << &builder;
    }

    auto s = builder.done();
    if (truth != s) {
      return ::testing::AssertionFailure()
	  << "Constructed string is [" << s << "], but it should be ["
	  << truth << "]";
    }

    return ::testing::AssertionSuccess();
  }

  template <typename C, typename T, typename A, typename V>
  ::testing::AssertionResult testFormatInt(
      const std::basic_string<C, T, A>& truth, const V& value,
      const isf::FormatSpecifier<C>& spec
  ) {
    isf::IntegerFormatter format;
    ImmutableStringBuilder<C, T> builder;

    const ImmutableStringBuilder<C, T>& result =
        format(builder, value, spec);
    if (&result != &builder) {
      return ::testing::AssertionFailure()
	  << "Return value from format() is incorrect.  It is " << &result
	  << ", but it should be " << &builder;
    }
    
    auto s = builder.done();
    if (truth != s) {
      return ::testing::AssertionFailure()
	  << "Constructed string is [" << s << "], but it should be ["
	  << truth << "]";
    }

    return ::testing::AssertionSuccess();
  }
  
  template <typename C, typename T, typename A, typename V>
  ::testing::AssertionResult testFormatFloat(
      const std::basic_string<C, T, A>& truth, const V& value,
      const isf::FormatSpecifier<C>& spec
  ) {
    isf::FloatFormatter format;
    ImmutableStringBuilder<C, T> builder;

    const ImmutableStringBuilder<C, T>& result =
        format(builder, value, spec);
    if (&result != &builder) {
      return ::testing::AssertionFailure()
	  << "Return value from format() is incorrect.  It is " << &result
	  << ", but it should be " << &builder;
    }
    
    auto s = builder.done();
    if (truth != s) {
      return ::testing::AssertionFailure()
	  << "Constructed string is [" << s << "], but it should be ["
	  << truth << "]";
    }

    return ::testing::AssertionSuccess();
  }

  template <typename C, typename T, typename A, typename... Args>
  ::testing::AssertionResult testFormatting(
      const ImmutableString<C, T, A>& truth,
      const ImmutableString<C, T, A>& fmt,
      Args&&... args
  ) {
    ImmutableStringBuilder<C, T, A> builder;
    ImmutableString<C, T, A> result =
        formatIString(builder, fmt.data(), fmt.data() + fmt.size(),
		      fmt.data(), std::forward<Args>(args)...);
    if (result != truth) {
      return ::testing::AssertionFailure()
	  << "Formatting \"" << fmt << "\" failed.  Result is [" << result
	  << "], but it should be [" << truth << "]";
    }
    return ::testing::AssertionSuccess();
  }
}

TEST(IStringFormatterTests, ParseFormatSpecifier) {
  auto LEFT = isf::FormatSpecifier<char>::LEFT;
  auto CENTER = isf::FormatSpecifier<char>::CENTER;
  auto RIGHT = isf::FormatSpecifier<char>::RIGHT;
  
  EXPECT_TRUE(executeParseTest("%%", makePercentSpec<char>()));

  EXPECT_TRUE(executeParseTest("%s", makeStringSpec<char>()));
  EXPECT_TRUE(executeParseTest("%10s", makeStringSpec<char>(10)));
  EXPECT_TRUE(executeParseTest("%-10s", makeStringSpec<char>(10, LEFT)));
  EXPECT_TRUE(executeParseTest("%^10s", makeStringSpec<char>(10, CENTER)));
  EXPECT_TRUE(executeParseTest("%010s", makeStringSpec<char>(10, RIGHT, '0')));

  EXPECT_TRUE(executeParseTest("%d", makeIntSpec<char>()));
  EXPECT_TRUE(executeParseTest("%10d", makeIntSpec<char>(10)));
  EXPECT_TRUE(executeParseTest("%+10d",
			       makeIntSpec<char>(10, RIGHT, ' ', true)));

  EXPECT_TRUE(executeParseTest("%o", makeOctalSpec<char>()));
  EXPECT_TRUE(executeParseTest("%x", makeHexSpec<char>()));
  EXPECT_TRUE(executeParseTest("%X", makeHexSpec<char>(-1, RIGHT, ' ', true)));

  EXPECT_TRUE(executeParseTest("%f", makeFloatSpec<char>()));
  EXPECT_TRUE(executeParseTest("%8f", makeFloatSpec<char>(8)));
  EXPECT_TRUE(executeParseTest("%8.6f", makeFloatSpec<char>(8, 6)));
  EXPECT_TRUE(executeParseTest("%-8.6f", makeFloatSpec<char>(8, 6, LEFT)));
  EXPECT_TRUE(executeParseTest("%^8.6f", makeFloatSpec<char>(8, 6, CENTER)));
  EXPECT_TRUE(executeParseTest("%08.6f",
			       makeFloatSpec<char>(8, 6, RIGHT, '0')));
  EXPECT_TRUE(executeParseTest("%+08.6f",
			       makeFloatSpec<char>(8, 6, RIGHT, '0', true)));

  EXPECT_TRUE(executeParseTest("%e", makeExpSpec<char>()));
  EXPECT_TRUE(executeParseTest("%E",
			       makeExpSpec<char>(-1, -1, RIGHT, ' ', true)));
  EXPECT_TRUE(executeParseTest("%g", makeGeneralFloatSpec<char>()));
  EXPECT_TRUE(executeParseTest("%G",
			       makeGeneralFloatSpec<char>(-1, -1, RIGHT, ' ',
							  true)));

  EXPECT_THROW(executeParseTest("%", makeSpec<char>()), FormatIStringError);
  EXPECT_THROW(executeParseTest("%+-0", makeSpec<char>()), FormatIStringError);
  EXPECT_THROW(executeParseTest("%-10", makeSpec<char>()), FormatIStringError);
  EXPECT_THROW(executeParseTest("%8.f", makeSpec<char>()), FormatIStringError);
  EXPECT_THROW(executeParseTest("%8.6u", makeSpec<char>()), FormatIStringError);

  // const char TEXT[] = "%8.6f";
  // isf::FormatSpecifier<char> spec;

  // const char* p =
  //     isf::FormatSpecifier<char>::parse(TEXT, TEXT + sizeof(TEXT) - 1, spec);
  // std::cout << spec << " " << (p - TEXT) << std::endl;
}

TEST(IStringFormatterTests, ConfigureBuilderFromSpecifier) {
  auto LEFT = isf::FormatSpecifier<char>::LEFT;
  auto CENTER = isf::FormatSpecifier<char>::CENTER;
  auto RIGHT = isf::FormatSpecifier<char>::RIGHT;
  isf::FormatSpecifier<char> spec =
      makeFloatSpec<char>(11, 6, CENTER, '*', true);
  IStringBuilder builder;

  spec.setWidth(builder).setPrecision(builder).setJustification(builder)
      .setAddSign(builder);
  EXPECT_EQ("*+0.250000*", builder.append(0.25).done());

  builder.width(10).precision(4).left().pad('*').addSign();
  spec = makeFloatSpec<char>();
  spec.setWidth(builder).setPrecision(builder).setJustification(builder)
      .setAddSign(builder);
  EXPECT_EQ("1.5", builder.append(1.5).done());
}

TEST(IStringFormatterTests, BaseFormatStringForSpecifier) {
  auto RIGHT = isf::FormatSpecifier<char>::RIGHT;

  EXPECT_EQ("%%", makePercentSpec<char>().baseFormatString());
  EXPECT_EQ("%d", makeIntSpec<char>().baseFormatString());
  EXPECT_EQ("%o", makeOctalSpec<char>().baseFormatString());
  EXPECT_EQ("%x", makeHexSpec<char>().baseFormatString());
  EXPECT_EQ("%X", makeHexSpec<char>(-1, RIGHT, ' ', true).baseFormatString());
  EXPECT_EQ("%f", makeFloatSpec<char>().baseFormatString());
  EXPECT_EQ("%e", makeExpSpec<char>().baseFormatString());
  EXPECT_EQ("%E",
	    makeExpSpec<char>(-1, -1, RIGHT, ' ', true).baseFormatString());
  EXPECT_EQ("%g", makeGeneralFloatSpec<char>().baseFormatString());
  EXPECT_EQ("%G", makeGeneralFloatSpec<char>(-1, -1, RIGHT, ' ', true)
	             .baseFormatString());
  EXPECT_EQ("%s", makeStringSpec<char>().baseFormatString());
}

TEST(IStringFormatterTests, ArgumentTypeNameForSpecifier) {
  EXPECT_EQ("", makePercentSpec<char>().argumentTypeName());
  EXPECT_EQ("integer", makeIntSpec<char>().argumentTypeName());
  EXPECT_EQ("integer", makeOctalSpec<char>().argumentTypeName());
  EXPECT_EQ("integer", makeHexSpec<char>().argumentTypeName());
  EXPECT_EQ("floating-point number", makeFloatSpec<char>().argumentTypeName());
  EXPECT_EQ("floating-point number", makeExpSpec<char>().argumentTypeName());
  EXPECT_EQ("floating-point number",
	    makeGeneralFloatSpec<char>().argumentTypeName());
  EXPECT_EQ("string", makeStringSpec<char>().argumentTypeName());
}

TEST(IStringFormatterTests, ConvertString) {
  auto LEFT = isf::FormatSpecifier<char>::LEFT;
  auto CENTER = isf::FormatSpecifier<char>::CENTER;
  auto RIGHT = isf::FormatSpecifier<char>::RIGHT;

  EXPECT_TRUE(testFormatString(std::string("  Hello  "), IString("Hello"), \
			       makeStringSpec<char>(9, CENTER)));
  EXPECT_TRUE(testFormatString(std::string("    World"), std::string("World"),
			       makeStringSpec<char>(9)));
  EXPECT_TRUE(testFormatString(std::string("Moo!"), "Moo!",
			       makeStringSpec<char>()));
  const char* cows = "cows";
  EXPECT_TRUE(testFormatString(std::string("cows"), cows,
			       makeStringSpec<char>()));
  EXPECT_TRUE(testFormatString(std::string("a"), 'a', makeStringSpec<char>()));
  EXPECT_TRUE(testFormatString(std::string("123"), 123,
			       makeStringSpec<char>()));
  EXPECT_TRUE(testFormatString(std::string("0.5"), 0.5,
			       makeStringSpec<char>()));
  EXPECT_TRUE(testFormatString(std::string("Something"), Something(),
			       makeStringSpec<char>()));
}

TEST(IStringFormatterTests, ConvertInteger) {
  auto LEFT = isf::FormatSpecifier<char>::LEFT;
  auto CENTER = isf::FormatSpecifier<char>::CENTER;
  auto RIGHT = isf::FormatSpecifier<char>::RIGHT;

  EXPECT_TRUE(testFormatInt(std::string("001024"), 1024,
			    makeIntSpec<char>(6, RIGHT, '0')));
  EXPECT_TRUE(testFormatInt(std::string("  +88 "), (long)88,
			    makeIntSpec<char>(6, CENTER, ' ' , true)));
  EXPECT_TRUE(testFormatInt(std::string("65"), 'A', makeIntSpec<char>()));
  EXPECT_THROW(testFormatInt(std::string(""), std::string("abc"),
			     makeIntSpec<char>()), FormatIStringError);
}

TEST(IStringFormatterTests, ConvertFloat) {
  auto LEFT = isf::FormatSpecifier<char>::LEFT;
  auto CENTER = isf::FormatSpecifier<char>::CENTER;
  auto RIGHT = isf::FormatSpecifier<char>::RIGHT;
  
  EXPECT_TRUE(testFormatFloat(std::string("0001.25"), 1.25,
			      makeFloatSpec<char>(7, 2, RIGHT, '0')));
  EXPECT_TRUE(testFormatFloat(std::string("+0.7500   "), 0.75,
			      makeFloatSpec<char>(10, 4, LEFT, ' ', true)));
  EXPECT_TRUE(testFormatFloat(std::string("**96**"), 96,
			      makeFloatSpec<char>(6, 0, CENTER, '*')));
  EXPECT_TRUE(testFormatFloat(std::string("   +96.000"), 96,
			      makeFloatSpec<char>(10, 3, RIGHT, ' ', true)));
  EXPECT_THROW(testFormatFloat(std::string(""), IString("abc"),
			       makeFloatSpec<char>()), FormatIStringError);
}

TEST(IStringFormatterTests, FormatWithNoSubstitutions) {
  EXPECT_TRUE(testFormatting(IString("This is a test."),
			     IString("This is a test.")));
  EXPECT_TRUE(testFormatting(IString("% An escape: % another %"),
			     IString("%% An escape: %% another %%")));
}

TEST(IStringFormatterTests, FormatWithOneSubstitution) {
  EXPECT_TRUE(testFormatting(IString("Cows: Moo!"), IString("Cows: %s!"),
			     "Moo"));
  EXPECT_TRUE(testFormatting(IString("|     Hello"), IString("|%10s"),
			     IString("Hello")));
  EXPECT_TRUE(testFormatting(IString("  Penguin  "), IString("%^11s"),
			     std::string("Penguin")));

}

TEST(IStringFormatterTests, FormatWithSeveralSubstitutions) {
  EXPECT_TRUE(testFormatting(IString("The price of ice cream is $  3.50"),
			     IString("The price of %s is $%6.2f"),
			     "ice cream", 3.5));
  EXPECT_TRUE(testFormatting(IString("John is  20.25% lead and 16.75% % gold"),
			     IString("%s is %6.2f%% %s and %.2f%% %% %s"),
			     "John", 20.25, "lead", 16.75, "gold"));
}

TEST(IStringFormatterTests, FormatStrings) {
  EXPECT_TRUE(testFormatting(IString("     hello world        center   z"),
			     IString("%10s %-10s %^10s %s"),
			     IString("hello"), std::string("world"), "center",
			     'z'));
  EXPECT_TRUE(testFormatting(IString("-12 1.575 Something"),
			     IString("%s %s %s"), -12, 1.575, Something()));
}

TEST(IStringFormatterTests, FormatIntegers) {
  EXPECT_TRUE(testFormatting(
      IString("     -9999|16384     |    66    |       +72|0000000184"),
      IString("%10d|%-10d|%^10d|%+10d|%010d"),
      -9999, 16384, 'B', 72, (unsigned long)184)
  );
  EXPECT_TRUE(testFormatting(IString("103 deadbeef DEADBEEF"),
			     IString("%o %x %X"),
			     'C', 0xDEADBEEF, 0xDEADBEEF));
			     
}

TEST(IStringFormatterTests, FormatFloatingPointNumbers) {
  EXPECT_TRUE(testFormatting(
      IString("  5.125000|-2.062500 | 1.875000 | +2.750000|008.625000"),
      IString("%10.6f|%-10.6f|%^10.6f|%+10.6f|%010.6f"),
      5.125, -2.0625, 1.875, 2.75, 8.625)
  );
  EXPECT_TRUE(testFormatting(
      IString("1.250e+01 -3.750E-01 1.12500 40.2500 1.23456e+06 1.25000E-06"),
      IString("%.3e %.3E %.6g %.6G %.6g %.6G"),
      12.5, -.375, 1.125, 40.25, 1234560, 1.25e-06)
  );
}

