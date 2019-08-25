#include <pistis/util/IStringSplitStream.hpp>
#include <pistis/util/IString.hpp>
#include <gtest/gtest.h>

using namespace pistis::util;

TEST(IStringSplitStreamTests, SplitByString) {
  const std::vector<IString> TRUTH{ "abc"_is, "de"_is, "g"_is, "hijkl"_is };
  IString source("abc||de||g||hijkl");
  IString target("||");
  IStringSplitStream< char, std::char_traits<char>, std::allocator<uint8_t>,
		      char, std::char_traits<char>, std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitEmptyStringByString) {
  const std::vector<IString> TRUTH{ };
  IString source("");
  IString target("||");
  IStringSplitStream< char, std::char_traits<char>, std::allocator<uint8_t>,
		      char, std::char_traits<char>, std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByStringNoSeparators) {
  const std::vector<IString> TRUTH{ "zyxwvu"_is };
  IString source("zyxwvu");
  IString target(" ");
  IStringSplitStream< char, std::char_traits<char>, std::allocator<uint8_t>,
		      char, std::char_traits<char>, std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByStringSeparatorAtStart) {
  const std::vector<IString> TRUTH{ ""_is, "one"_is, "three"_is };
  IString source("::one::three");
  IString target("::");
  IStringSplitStream< char, std::char_traits<char>, std::allocator<uint8_t>,
		      char, std::char_traits<char>, std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByStringSeparatorAtEnd) {
  const std::vector<IString> TRUTH{ "two"_is, "one"_is, ""_is };
  IString source("two::one::");
  IString target("::");
  IStringSplitStream< char, std::char_traits<char>, std::allocator<uint8_t>,
		      char, std::char_traits<char>, std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByStringWithConsecutiveSeparators) {
  const std::vector<IString> TRUTH{ ""_is, "a"_is, ""_is, "b"_is, "c"_is,
                                    ""_is };
  IString source(":a::b:c:");
  IString target(":");
  IStringSplitStream< char, std::char_traits<char>, std::allocator<uint8_t>,
		      char, std::char_traits<char>, std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByEmptyString) {
  const std::vector<IString> TRUTH{ "a"_is, "b"_is, "d"_is, "h"_is, "p"_is };
  IString source("abdhp");
  IString target("");
  IStringSplitStream< char, std::char_traits<char>, std::allocator<uint8_t>,
		      char, std::char_traits<char>, std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByStringWithLimit) {
  const std::vector<IString> TRUTH{ "abc"_is, "de"_is, "g||hijkl"_is };
  IString source("abc||de||g||hijkl");
  IString target("||");
  IStringSplitStream< char, std::char_traits<char>, std::allocator<uint8_t>,
		      char, std::char_traits<char>, std::allocator<uint8_t> >
      splitStream(source, target, 2);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByRegex) {
  const std::vector<IString> TRUTH{
      "apples"_is, "oranges"_is, "bananas"_is, "grapes"_is
  };
  IString source("apples  oranges bananas   grapes");
  std::regex target("\\s+");
  RegexIStringSplitStream< char, std::char_traits<char>,
			   std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitEmptyStringByRegex) {
  const std::vector<IString> TRUTH{ };
  IString source("");
  std::regex target("\\s+");
  RegexIStringSplitStream< char, std::char_traits<char>,
			   std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByRegexNoSeparators) {
  const std::vector<IString> TRUTH{ };
  IString source("");
  std::regex target("\\s+");
  RegexIStringSplitStream< char, std::char_traits<char>,
			   std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByRegexSeparatorAtStart) {
  const std::vector<IString> TRUTH{ ""_is, "apple"_is, "banana"_is,
                                    "orange"_is };
  IString source(":apple:::banana::orange");
  std::regex target(":+");
  RegexIStringSplitStream< char, std::char_traits<char>,
			   std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByRegexSeparatorAtEnd) {
  const std::vector<IString> TRUTH{ "fred"_is, "barney"_is, "wilma"_is, ""_is };
  IString source("fred...;barney.wilma.");
  std::regex target("\\.+;?");
  RegexIStringSplitStream< char, std::char_traits<char>,
			   std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByRegexWithConsecutiveSeparators) {
  const std::vector<IString> TRUTH{ ""_is, "abc"_is, ""_is, ""_is, "zz"_is,
                                    "yy"_is, ""_is };
  IString source("#abc@@###zz@#yy@@@@@#");
  std::regex target("@*#");
  RegexIStringSplitStream< char, std::char_traits<char>,
			   std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByRegexMatchingZeroLengthString) {
  const std::vector<IString> TRUTH{ "a"_is, "b"_is, ""_is, "c"_is, "d"_is,
                                    "e"_is };
  IString source("ab  cde");
  std::regex target("\\s*");
  RegexIStringSplitStream< char, std::char_traits<char>,
			   std::allocator<uint8_t> >
      splitStream(source, target);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByRegexWithLimit) {
  const std::vector<IString> TRUTH{
      "apples"_is, "oranges"_is, "bananas   grapes"_is
  };
  IString source("apples  oranges bananas   grapes");
  std::regex target("\\s+");
  RegexIStringSplitStream< char, std::char_traits<char>,
			   std::allocator<uint8_t> >
      splitStream(source, target, 2);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}

TEST(IStringSplitStreamTests, SplitByRegexMatchingZeroLengthStringWithLimit) {
  const std::vector<IString> TRUTH{ "a"_is, "b"_is, "  cde"_is };
  IString source("ab  cde");
  std::regex target("\\s*");
  RegexIStringSplitStream< char, std::char_traits<char>,
			   std::allocator<uint8_t> >
      splitStream(source, target, 2);

  std::vector<IString> result = splitStream.toVector();
  EXPECT_EQ(TRUTH, result);
}
