#include <pistis/util/IStringBuilder.hpp>
#include <pistis/testing/Allocator.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <stdint.h>

using namespace pistis::util;

namespace {
  typedef pistis::testing::Allocator<uint8_t> TestAllocator;
  typedef ImmutableStringBuilder<char, std::char_traits<char>,
				 TestAllocator> TestBuilder;
}

TEST(ImmutableStringBuilder, AppendChar) {
  TestAllocator allocator("TEST");
  TestBuilder builder(allocator);

  EXPECT_EQ("TEST", builder.allocator().name());
  EXPECT_EQ(0, builder.allocated());
  EXPECT_EQ(0, builder.size());

  EXPECT_EQ("Z", builder.append('Z').done());
}

TEST(ImmutableStringBuilder, AppendString) {
  TestAllocator allocator("TEST");
  TestBuilder builder(16, allocator);

  EXPECT_EQ("TEST", builder.allocator().name());
  EXPECT_EQ(16, builder.allocated());
  EXPECT_EQ(0, builder.size());

  builder.append(IString("Hello ")).append(std::string("cute "))
         .append("").append("penguin!");
  EXPECT_EQ(32, builder.allocated());
  EXPECT_EQ(19, builder.size());
  EXPECT_EQ("Hello cute penguin!", builder.done());
}

TEST(ImmutableStringBuilder, AppendRange) {
  IStringBuilder builder;
  std::vector<char> chars{
      'C', 'o', 'w', 's', ' ', 'g', 'o', ' ', 'M', 'o', 'o'
  };

  builder.append(chars.begin(), chars.end());
  EXPECT_EQ("Cows go Moo", builder.done());
}

TEST(ImmutableStringBuilder, AppendInt) {
  IStringBuilder builder;
  builder.append((int16_t)32767).append(' ').append((int32_t)-1000000)
         .append(' ').append((int64_t)10000000000);
  EXPECT_EQ("32767 -1000000 10000000000", builder.done());

  builder.addSign().append((int16_t)32767).append(' ').append((int32_t)-1000000)
         .append(' ').noSign().append((int64_t)10000000000);
  EXPECT_EQ("+32767 -1000000 10000000000", builder.done());
}

TEST(ImmutableStringBuilder, AppendIntInDifferentBases) {
  IStringBuilder builder;

  builder.append(127).append(' ').oct().append(190).append(' ').append(190.5)
         .append(' ').hex().append(0xDEADBEEF).append(' ').append(0.25)
         .append(' ').hexUpper().append(0xFEEDBEAD).append(' ').append(1.125)
         .append(' ').dec().append(32767);
  EXPECT_EQ("127 276 190.5 deadbeef 0.25 FEEDBEAD 1.125 32767",
	    builder.done());
}

TEST(ImmutableStringBuilder, AppendFloat) {
  IStringBuilder builder;
  builder.append((float)16.5).append(' ').append((double)-172.25).append(' ')
         .append((long double)0.3125);
  EXPECT_EQ("16.5 -172.25 0.3125", builder.done());

  builder.addSign().append((float)16.5).append(' ').noSign()
         .append((double)-172.25).append(' ').append((long double)0.3125);
  EXPECT_EQ("+16.5 -172.25 0.3125", builder.done());  
}

TEST(ImmutableStringBuilder, AppendFloatWithFixedPrecision) {
  IStringBuilder builder;
  builder.precision(6).append((float)16.5).append(' ').append((double)-172.25)
         .append(' ').precision(0).append((long double)0.3125);
  EXPECT_EQ("16.500000 -172.250000 0", builder.done());
}

TEST(ImmutableStringBuilder, AppendFloatInExponentialFormat) {
  IStringBuilder builder;
  builder.append((float)16.5).append(' ').exp().append((double)-172.25)
         .append(' ').expUpper().append((long double)0.3125).append(' ')
         .dec().append(1.125);
  EXPECT_EQ("16.5 -1.722500e+02 3.125000E-01 1.125", builder.done());
}

TEST(ImmutableStringBuilder, AppendFloatInGeneralFormat) {
  IStringBuilder builder;
  std::string truth(
      "16.5 -172.250 0.312500 1.50000e-05 1.87500e+08 -1.72250e+02 1.125"
  );
  
  builder.append((float)16.5).append(' ').expGeneral().precision(6)
         .append((double)-172.25).append(' ').append((long double)0.3125)
         .append(' ').append(1.5e-05).append(' ').append(1.875e+8)
         .append(' ').noPrecision().append(-172.25).append(' ').dec()
         .append(1.125);
  EXPECT_EQ(truth, builder.done());

  builder.expGeneralUpper().precision(3).append(1250.0);
  EXPECT_EQ("1.25E+03", builder.done());
}

TEST(ImmutableStringBuilder, AppendStringWithFixedWidth) {
  IStringBuilder builder;
  builder.width(10).append("abc").width(5).append("abcde").width(1)
         .append("abc").width(6).append("zyx").noWidth().append("q");
  EXPECT_EQ("       abcabcdeabc   zyxq", builder.done());
}

TEST(ImmutableStringBuilder, Justify) {
  IStringBuilder builder;
  builder.width(10).append("abc").noWidth().append('|').width(10).center()
         .append("moo").noWidth().append('|').width(10).left().append("hello")
         .noWidth().append('|').width(10).right().append("penguin")
         .noWidth().append('|');
  EXPECT_EQ("       abc|    moo   |hello     |   penguin|", builder.done());
}

TEST(ImmutableStringBuilder, Pad) {
  IStringBuilder builder;
  builder.width(10).append("abc").pad('*').append("ABC").padWithDefault()
         .append("moo");
  EXPECT_EQ("       abc*******ABC       moo", builder.done());
}

TEST(ImmutableStringBuilder, StreamingOperator) {
  IStringBuilder builder;
  builder << IString("Hello ") << 123 << " " << std::string("world")
	  << ' ' << 1.125;
  EXPECT_EQ("Hello 123 world 1.125", builder.done());
}

TEST(ImmutableStringBuilder, Manipulators) {
  IStringBuilder builder;
  builder << "Hello |" << ism::width(10) << "cow" << ism::noWidth << " penguin";

  EXPECT_EQ("Hello |       cow penguin", builder.done());

  builder << ism::width(10) << ism::precision(5) << ism::addSign << 1.125
	  << ism::noPrecision << ism::noSign << 1.125;
  EXPECT_EQ("  +1.12500     1.125", builder.done());

  builder << ism::hex << 0xDEADBEEF << " " << ism::hexUpper << 0xBEEFFEED
	  << " " << ism::oct << 190 << " " << ism::dec << 190;
  EXPECT_EQ("deadbeef BEEFFEED 276 190", builder.done());

  builder << ism::width(10) << "abc" << ism::noWidth << '|' << ism::width(10)
	  << ism::center << "ABC" << ism::noWidth << '|' << ism::width(10)
	  << ism::left << ism::pad('*') << "Moo" << ism::noWidth << '|';
  EXPECT_EQ("       abc|    ABC   |Moo*******|", builder.done());

  builder << 172.25 << ' ' << ism::precision(5) << ism::expf << 172.25 << ' '
	  << ism::expfUpper << 172.25 << ' ' << ism::expg << 15.25 << ' '
	  << 125000.0 << ' ' << ism::expgUpper << 3.75e-6;
  EXPECT_EQ("172.25 1.72250e+02 1.72250E+02 15.250 1.2500e+05 3.7500E-06",
	    builder.done());
}
