#include <pistis/util/IString.hpp>
#include <pistis/testing/Allocator.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <vector>

using namespace pistis::util;
namespace pt = pistis::testing;

namespace {
  typedef pt::Allocator<uint8_t> TestAllocator;
  typedef ImmutableString<char, std::char_traits<char>, TestAllocator>
          TestIString;
  typedef ImmutableString<wchar_t, std::char_traits<wchar_t>, TestAllocator>
          TestWIString;

  template <typename T, typename U>
  void checkSameType(T* t = 0, U* u = 0) {
    static_assert(std::is_same<T, U>::value, "Not the same");
  }
}

TEST(IStringTests, CreateWithDefaultConstructor) {
  const TestAllocator allocator("TEST");
  TestIString s(allocator);

  EXPECT_EQ("TEST", s.allocator().name());
  EXPECT_EQ(0, s.size());
  EXPECT_TRUE(s.begin() == s.end());
  EXPECT_TRUE(s.cbegin() == s.cend());
}

TEST(IStringTests, CreateFromRange) {
  const std::vector<char> TEXT{ 'H', 'e', 'l', 'l', 'o' };
  const TestAllocator allocator("TEST");
  TestIString s(TEXT.begin(), TEXT.end(), allocator);
  std::vector<char> chars;

  EXPECT_EQ("TEST", s.allocator().name());
  ASSERT_EQ(TEXT.size(), s.size());
  ASSERT_EQ(TEXT.size(), s.end() - s.begin());
  EXPECT_NE((const char*)0, s.data());

  std::copy(s.begin(), s.end(), std::back_inserter(chars));
  EXPECT_EQ(TEXT, chars);

  chars.clear();
  std::copy(s.data(), s.data() + s.size(), std::back_inserter(chars));
  EXPECT_EQ(TEXT, chars);
}

TEST(IStringTests, CreateFromCharArray) {
  const TestAllocator allocator("TEST");
  const char TEXT[] = "Moo!";
  // Note: Don't do this for real.  It's ok for testing, but when an IString
  //       is fed a const array of characters of the same type as the
  //       string's characters, it assumes the array is statically allocated
  //       and just points to the start and end of the input without
  //       allocating space for it.  This behavior makes the construction
  //       of IString("foo") constant time in the length of the string, but
  //       comes at the cost of not handling block scoped arrays of const
  //       chars once that array goes out-of-scope.  In real code, copy
  //       the block-scoped array into the IString like this:
  //       IString(sizeof(TEXT) - 1, TEXT);
  TestIString s(TEXT, allocator);

  EXPECT_EQ("TEST", s.allocator().name());
  EXPECT_EQ(sizeof(TEXT) - 1, s.size());
  EXPECT_EQ(TEXT, s.data());

  std::string chars(s.begin(), s.end());
  EXPECT_EQ(TEXT, chars);
}

TEST(IStringTests, CreateFromStdString) {
  const TestAllocator allocator("TEST");
  const std::string TEXT("Cows rule!");
  TestIString s(TEXT, allocator);
  
  EXPECT_EQ("TEST", s.allocator().name());
  EXPECT_EQ(TEXT.size(), s.size());
  EXPECT_EQ(TEXT, s);
}

TEST(IStringTests, CreateFromCopy) {
  const TestAllocator allocator("TEST");
  const std::string TEXT("Cows rule!");
  TestIString s(TEXT, allocator);
  
  ASSERT_EQ("TEST", s.allocator().name());
  ASSERT_EQ(TEXT.size(), s.size());
  ASSERT_EQ(TEXT, s);
  {
    TestIString copy(s);

    EXPECT_EQ("TEST", copy.allocator().name());
    EXPECT_FALSE(copy.allocator().moved());
    EXPECT_EQ(TEXT.size(), copy.size());
    EXPECT_EQ(TEXT, copy);
  }

  EXPECT_EQ("TEST", s.allocator().name());
  EXPECT_EQ(TEXT.size(), s.size());
  EXPECT_EQ(TEXT, s);
}

TEST(IStringTests, CreateByMove) {
  const TestAllocator allocator("TEST");
  const std::string TEXT("Cows rule!");
  TestIString s(TEXT, allocator);
  
  ASSERT_EQ("TEST", s.allocator().name());
  ASSERT_EQ(TEXT.size(), s.size());
  ASSERT_EQ(TEXT, s);
  {
    TestIString copy(std::move(s));

    EXPECT_EQ("TEST", copy.allocator().name());
    EXPECT_TRUE(copy.allocator().moved());
    EXPECT_EQ(TEXT.size(), copy.size());
    EXPECT_EQ(TEXT, copy);
  }

  EXPECT_EQ(0, s.size());
}

TEST(IStringTests, CreateFromRangeOfOtherCharType) {
  const TestAllocator allocator("TEST");
  const std::vector<char> TEXT{'C', 'u', 't', 'e', ' ',
                               'K', 'i', 't', 't', 'e', 'n', 's' };
  TestWIString s(TEXT.begin(), TEXT.end(), allocator);

  EXPECT_EQ("TEST", s.allocator().name());
  ASSERT_EQ(TEXT.size(), s.size());
  ASSERT_EQ(TEXT.size(), s.end() - s.begin());

  std::vector<wchar_t> chars;
  std::vector<wchar_t> trueChars;
  std::copy(TEXT.begin(), TEXT.end(), std::back_inserter(trueChars));
  std::copy(s.begin(), s.end(), std::back_inserter(chars));
  EXPECT_EQ(trueChars, chars);

  chars.clear();
  std::copy(s.data(), s.data() + s.size(), std::back_inserter(chars));
  EXPECT_EQ(trueChars, chars);
}

TEST(IStringTests, CreateFromArrayOfOtherCharType) {
  const TestAllocator allocator("TEST");
  // See CreateFromArray for why creating a string from a locally-allocated
  // character array is dangerous and should be avoided in application code.
  const char TEXT[] = "Cows are cool";
  const wchar_t TRUTH[] = L"Cows are cool";
  TestWIString s(TEXT, allocator);

  EXPECT_EQ("TEST", s.allocator().name());
  EXPECT_EQ(sizeof(TEXT) - 1, s.size());
  EXPECT_NE((const void*)TEXT, (const void*)s.data());

  std::wstring chars(s.begin(), s.end());
  EXPECT_EQ(TRUTH, chars);
}

TEST(IStringTests, CreateFromStdStringOfOtherCharType) {
  const TestAllocator allocator("TEST");
  const std::string TEXT("Cows are cool");
  const std::wstring TRUTH(L"Cows are cool");
  TestWIString s(TEXT, allocator);

  EXPECT_EQ("TEST", s.allocator().name());
  EXPECT_EQ(TEXT.size(), s.size());
  EXPECT_EQ(TRUTH, s);
}

TEST(IStringTests, CreateByCopyingStringOfOtherCharType) {
  const TestAllocator allocator("TEST");
  const std::string TEXT("Cows are cool");
  TestIString s(TEXT, allocator);
  
  ASSERT_EQ("TEST", s.allocator().name());
  ASSERT_EQ(TEXT.size(), s.size());
  ASSERT_EQ(TEXT, s);
  {
    const std::wstring TRUTH(L"Cows are cool");
    TestWIString copy(s);

    EXPECT_EQ("TEST", copy.allocator().name());
    EXPECT_FALSE(copy.allocator().moved());
    EXPECT_EQ(TRUTH.size(), copy.size());
    EXPECT_EQ(TRUTH, copy);
  }

  EXPECT_EQ("TEST", s.allocator().name());
  EXPECT_EQ(TEXT.size(), s.size());
  EXPECT_EQ(TEXT, s);
}

TEST(IStringTests, Hash) {
  std::hash<IString> h;
  IString s("abcde");

  EXPECT_EQ((size_t)210706217108, h(s));
}

TEST(IStringTests, Compare) {
  IString s1("arr");
  IString s2("arrest");
  IString s3("axe");
  IString s4("arrest");

  EXPECT_EQ(0, s1.cmp(s1));
  EXPECT_EQ(0, s2.cmp(s4));

  EXPECT_GT(0, s1.cmp(s2));
  EXPECT_GT(0, s1.cmp(s3));
  EXPECT_GT(0, s2.cmp(s3));
  
  EXPECT_LT(0, s2.cmp(s1));
  EXPECT_LT(0, s3.cmp(s1));
  EXPECT_LT(0, s3.cmp(s2));
}

TEST(IStringTests, CompareWithStdString) {
  IString s1("arr");
  IString s2("arrest");

  EXPECT_EQ(0, s1.cmp(std::string("arr")));

  EXPECT_GT(0, s1.cmp(std::string("arrest")));
  EXPECT_GT(0, s1.cmp(std::string("axe")));
  
  EXPECT_LT(0, s2.cmp(std::string("arr")));
}

TEST(IStringTests, CompareWithCharArray) {
  IString s1("arr");
  IString s2("arrest");

  EXPECT_EQ(0, s1.cmp("arr"));

  EXPECT_GT(0, s1.cmp("arrest"));
  EXPECT_GT(0, s1.cmp("axe"));
  
  EXPECT_LT(0, s2.cmp("arr"));
}

TEST(IStringTests, CompareWithCString) {
  const char* t1 = "arr";
  const char* t2 = "arrest";
  const char* t3 = "axe";
  IString s1("arr");
  IString s2("arrest");

  EXPECT_EQ(0, s1.cmp(t1));

  EXPECT_GT(0, s1.cmp(t2));
  EXPECT_GT(0, s1.cmp(t3));
  
  EXPECT_LT(0, s2.cmp(t1));
}

TEST(IStringTests, Substr) {
  IString s("cows are cool and penguins are cute");
  EXPECT_EQ("cool and penguins", s.substr(9, 26));
  EXPECT_EQ("", s.substr(20, 20));
  EXPECT_EQ("", s.substr(10, 5));
}

TEST(IStringTests, Shrink) {
  IString s("cows are cool and penguins are cute");
  IString ss(s.substr(9, 26));
  EXPECT_EQ(s.data() + 9, ss.data());

  ss = ss.shrink();
  EXPECT_EQ("cool and penguins", ss);
  EXPECT_NE(s.data() + 9, ss.data());
}

TEST(IStringTests, FindChar) {
  IString s("adcbedcba");

  EXPECT_EQ(0, s.find('a'));
  EXPECT_EQ(3, s.find('b'));
  EXPECT_EQ(2, s.find('c'));
  EXPECT_EQ(1, s.find('d'));
  EXPECT_EQ(4, s.find('e'));
  EXPECT_EQ(IString::NPOS, s.find('z'));

  EXPECT_EQ(6, s.find('c', 3));
  EXPECT_EQ(5, s.find('d', 2, 7));
  EXPECT_EQ(8, s.find('a', 2));
  EXPECT_EQ(IString::NPOS, s.find('a', 2, 7));

  EXPECT_EQ(IString::NPOS, s.find('c', 2, 2));
  EXPECT_EQ(IString::NPOS, s.find('c', 2, 1));
}

TEST(IStringTests, FindImmutableString) {
  IString s("I love love cows, oh yes I do! I so love cows!");
  IString target("love cows");
  IString tiny("love cow");
  IString y("y");
  IString empty;

  EXPECT_EQ( 7, s.find(target));
  EXPECT_EQ(36, s.find(target, 10));
  EXPECT_EQ(IString::NPOS, s.find(target, 0, 10));
  EXPECT_EQ(36, s.find(IString("love cows!")));
  EXPECT_EQ(IString::NPOS, tiny.find(target));
  EXPECT_EQ(21, s.find(y));
  EXPECT_EQ(IString::NPOS, s.find(IString("z")));
  EXPECT_EQ(IString::NPOS, s.find(empty));
  EXPECT_EQ(IString::NPOS, s.find(target, 35, 10));
}

TEST(IStringTests, FindStdString) {
  IString s("I love love cows, oh yes I do! I so love cows!");
  std::string target("love cows");
  std::string missing("not");

  EXPECT_EQ( 7, s.find(target));
  EXPECT_EQ(36, s.find(target, 10));
  EXPECT_EQ(IString::NPOS, s.find(missing));
}

TEST(IStringTests, FindCharArray) {
  IString s("I love love cows, oh yes I do! I so love cows!");

  EXPECT_EQ( 7, s.find("love cows"));
  EXPECT_EQ(36, s.find("love cows", 10));
  EXPECT_EQ(25, s.find("I", 20, 30));
  EXPECT_EQ(IString::NPOS, s.find("moo"));
}

TEST(IStringTests, FindCString) {
  IString s("I love love cows, oh yes I do! I so love cows!");
  const char* target = "love cows";
  const char* oneLetter = "I";
  const char* missing = "not";

  EXPECT_EQ( 7, s.find(target));
  EXPECT_EQ(36, s.find(target, 10));
  EXPECT_EQ(25, s.find(oneLetter, 20, 30));
  EXPECT_EQ(IString::NPOS, s.find(missing));
}

TEST(IStringTests, FindLastChar) {
  IString s("adcbedcba");

  EXPECT_EQ(8, s.findLast('a'));
  EXPECT_EQ(7, s.findLast('b'));
  EXPECT_EQ(6, s.findLast('c'));
  EXPECT_EQ(5, s.findLast('d'));
  EXPECT_EQ(4, s.findLast('e'));
  EXPECT_EQ(IString::NPOS, s.findLast('z'));

  EXPECT_EQ(2, s.findLast('c', 0, 3));
  EXPECT_EQ(1, s.findLast('d', 1, 5));
  EXPECT_EQ(0, s.findLast('a', 0, 2));
  EXPECT_EQ(IString::NPOS, s.findLast('a', 2, 7));

  EXPECT_EQ(IString::NPOS, s.findLast('c', 2, 2));
  EXPECT_EQ(IString::NPOS, s.findLast('c', 2, 1));
}

TEST(IStringTests, FindLastImmutableString) {
  IString s("I love love cows, oh yes I do! I so love cows!");
  IString target("love cows");
  IString tiny("love cow");
  IString oneLetter("s");
  IString empty;

  EXPECT_EQ(36, s.findLast(target));
  EXPECT_EQ( 7, s.findLast(target, 0, 16));
  EXPECT_EQ(IString::NPOS, s.findLast(target, 0, 10));
  EXPECT_EQ( 0, s.findLast(IString("I love")));
  EXPECT_EQ(36, s.findLast(IString("love cows!")));
  EXPECT_EQ(IString::NPOS, s.findLast(IString("not")));
  EXPECT_EQ(IString::NPOS, tiny.findLast(target));
  EXPECT_EQ(44, s.findLast(oneLetter));
  EXPECT_EQ(IString::NPOS, s.findLast(IString("z")));
  EXPECT_EQ(IString::NPOS, s.findLast(empty));
  EXPECT_EQ(IString::NPOS, s.findLast(target, 35, 10));
}

TEST(IStringTests, FindLastStdString) {
  IString s("I love love cows, oh yes I do! I so love cows!");
  std::string target("love cows");
  std::string missing("not");

  EXPECT_EQ(36, s.findLast(target));
  EXPECT_EQ( 7, s.findLast(target, 0, 16));
  EXPECT_EQ(IString::NPOS, s.findLast(missing));
}

TEST(IStringTests, FindLastCharArray) {
  IString s("I love love cows, oh yes I do! I so love cows!");

  EXPECT_EQ(36, s.findLast("love cows"));
  EXPECT_EQ( 7, s.findLast("love cows", 0, 16));
  EXPECT_EQ(IString::NPOS, s.findLast("love cows", 0, 10));
  EXPECT_EQ(25, s.findLast("I", 20, 30));
  EXPECT_EQ(IString::NPOS, s.findLast("moo"));
}

TEST(IStringTests, FindLastCString) {
  IString s("I love love cows, oh yes I do! I so love cows!");
  const char* target = "love cows";
  const char* oneLetter = "I";
  const char* missing = "not";

  EXPECT_EQ(36, s.findLast(target));
  EXPECT_EQ( 7, s.findLast(target, 0, 16));
  EXPECT_EQ(25, s.findLast(oneLetter, 20, 30));
  EXPECT_EQ(IString::NPOS, s.findLast(missing));
}

TEST(IStringTests, FindFirstOf) {
  IString s("I love love cows, oh yes I do! I so love cows!");
  IString target("lco");
  IString oneChar("s");
  IString doesNotOccur("zxu");
  IString empty;

  EXPECT_EQ(2, s.findFirstOf(target));
  EXPECT_EQ(28, s.findFirstOf(target, 20, 30));
  EXPECT_EQ(23, s.findFirstOf(oneChar, 20, 30));
  EXPECT_EQ(33, s.findFirstOf(oneChar, 30));
  EXPECT_EQ(IString::NPOS, s.findFirstOf(doesNotOccur));
  EXPECT_EQ(IString::NPOS, s.findFirstOf(empty));
  EXPECT_EQ(IString::NPOS, s.findFirstOf(target, 20, 28));
  EXPECT_EQ(IString::NPOS, s.findFirstOf(target, 30, 20));

  const char* ctarget = "lco";
  EXPECT_EQ( 2, s.findFirstOf(std::string("lco")));
  EXPECT_EQ( 2, s.findFirstOf("lco"));
  EXPECT_EQ( 2, s.findFirstOf(ctarget));
	    
  EXPECT_EQ(28, s.findFirstOf(std::string("lco"), 20, 30));
  EXPECT_EQ(28, s.findFirstOf("lco", 20, 30));
  EXPECT_EQ(28, s.findFirstOf(ctarget, 20, 30));
}

TEST(IStringTests, FindLastOf) {
  IString s("I love love cows, oh yes I do! I so love cows!");
  IString target("wl");
  IString oneChar("I");
  IString doesNotOccur("zxu");
  IString empty;

  EXPECT_EQ(43, s.findLastOf(target));
  EXPECT_EQ( 7, s.findLastOf(target, 1, 9));
  EXPECT_EQ( 0, s.findLastOf(oneChar, 0, 16));
  EXPECT_EQ(25, s.findLastOf(oneChar, 0, 31));
  EXPECT_EQ(IString::NPOS, s.findLastOf(doesNotOccur));
  EXPECT_EQ(IString::NPOS, s.findLastOf(empty));
  EXPECT_EQ(IString::NPOS, s.findLastOf(target, 20, 28));
  EXPECT_EQ(IString::NPOS, s.findLastOf(target, 30, 20));

  const char* ctarget = "wl";
  EXPECT_EQ(43, s.findLastOf(std::string("wl")));
  EXPECT_EQ(43, s.findLastOf("wl"));
  EXPECT_EQ(43, s.findLastOf(ctarget));
	    
  EXPECT_EQ( 7, s.findLastOf(std::string("wl"), 1, 9));
  EXPECT_EQ( 7, s.findLastOf("wl", 1, 9));
  EXPECT_EQ( 7, s.findLastOf(ctarget, 1, 9));
}

TEST(IStringTests, Insert) {
  IString s("abcdef");
  EXPECT_EQ(IString("abcZdef"), s.insert(3, 'Z'));

  checkSameType<WIString, decltype(s.insert(3, wchar_t('Z')))>();
  EXPECT_EQ(WIString("abcZdef"), s.insert(3, wchar_t('Z')));

  EXPECT_EQ("abZYcdef", s.insert(2, IString("ZY")));
  EXPECT_EQ("ZYabcdef", s.insert(0, IString("ZY")));
  EXPECT_EQ("abcdefZY", s.insert(s.size(), IString("ZY")));

  const char* zy = "ZY";
  EXPECT_EQ("abcZYdef", s.insert(3, std::string("ZY")));
  EXPECT_EQ("aZYbcdef", s.insert(1, "ZY"));
  EXPECT_EQ("abcdefZY", s.insert(s.size(), zy));

  checkSameType<WIString, decltype(s.insert(3, WIString("ZY")))>();
  EXPECT_EQ(WIString("abcZYdef"), s.insert(3, WIString("ZY")));
}

TEST(IStringTests, Append) {
  IString s("abcdef");
  EXPECT_EQ(IString("abcdefZ"), s.append('Z'));

  checkSameType<WIString, decltype(s.append(wchar_t('Z')))>();
  EXPECT_EQ(WIString("abcdefZ"), s.append(wchar_t('Z')));

  const char* yz = "YZ";
  EXPECT_EQ("abcdefZY", s.append(IString("ZY")));
  EXPECT_EQ("abcdefMOO", s.append(std::string("MOO")));
  EXPECT_EQ("abcdefCOWS", s.append("COWS"));
  EXPECT_EQ("abcdefYZ", s.append(yz));

  checkSameType<WIString, decltype(s.append(WIString("QQQ")))>();
  EXPECT_EQ(WIString("abcdefQQQ"), s.append(WIString("QQQ")));
}

TEST(IStringTests, ReplaceChar) {
  IString s("i love cows.  i love penguins.  i love kitties.");
  checkSameType<IString, decltype(s.replace('i', 'I'))>();
  EXPECT_EQ(IString("I love cows.  I love penguIns.  I love kIttIes."),
	    s.replace('i', 'I'));

  checkSameType<IString, decltype(s.replace(L's', 'S'))>();
  EXPECT_EQ(IString("i love cowS.  i love penguinS.  i love kittieS."),
	    s.replace(L's', 'S'));

  checkSameType<WIString, decltype(s.replace('o', L'O'))>();
  EXPECT_EQ(WIString("i lOve cOws.  i lOve penguins.  i lOve kitties."),
	    s.replace('o', L'O'));
}

TEST(IStringTests, ReplaceWithIString) {
  IString s("i love cows.  cows are the bestbest.  cows");
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(IString("cows"), IString("penguins")));
  EXPECT_EQ(IString("i love cows.  cows are the moomoo.  cows"),
	    s.replace(IString("best"), IString("moo")));
  EXPECT_EQ(IString("i really love cows.  cows are the bestbest.  cows"),
	    s.replace(IString("i love"), IString("i really love")));

  const char* cows = "cows";
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(std::string("cows"), IString("penguins")));
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace("cows", IString("penguins")));
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(cows, IString("penguins")));


  checkSameType<IString,
		decltype(s.replace(WIString("cows"), IString("penguins")))>();
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(WIString("cows"), IString("penguins")));

  checkSameType<WIString, decltype(s.replace("cows", WIString("penguins")))>();
  EXPECT_EQ(WIString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace("cows", WIString("penguins")));
}

TEST(IStringTests, ReplaceWithStdString) {
  IString s("i love cows.  cows are the bestbest.  cows");
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(IString("cows"), std::string("penguins")));
  EXPECT_EQ(IString("i love cows.  cows are the moomoo.  cows"),
	    s.replace(IString("best"), std::string("moo")));
  EXPECT_EQ(IString("i really love cows.  cows are the bestbest.  cows"),
	    s.replace(IString("i love"), std::string("i really love")));

  const char* cows = "cows";
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(std::string("cows"), std::string("penguins")));
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace("cows", std::string("penguins")));
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(cows, std::string("penguins")));


  checkSameType<IString, decltype(s.replace(WIString("cows"),
					    std::string("penguins")))>();
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(WIString("cows"), std::string("penguins")));

  checkSameType<WIString, decltype(s.replace("cows",
					     std::wstring(L"penguins")))>();
  EXPECT_EQ(WIString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace("cows", WIString("penguins")));
}

TEST(IStringTests, ReplaceWithCharArray) {
  IString s("i love cows.  cows are the bestbest.  cows");
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(IString("cows"), "penguins"));
  EXPECT_EQ(IString("i love cows.  cows are the moomoo.  cows"),
	    s.replace(IString("best"), "moo"));
  EXPECT_EQ(IString("i really love cows.  cows are the bestbest.  cows"),
	    s.replace(IString("i love"), "i really love"));

  const char* cows = "cows";
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(std::string("cows"), "penguins"));
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace("cows", "penguins"));
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(cows, "penguins"));


  checkSameType<IString, decltype(s.replace(WIString("cows"), "penguins"))>();
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(WIString("cows"), "penguins"));

  checkSameType<WIString, decltype(s.replace("cows",
					     std::wstring(L"penguins")))>();
  EXPECT_EQ(WIString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace("cows", L"penguins"));
}

TEST(IStringTests, ReplaceWithCString) {
  IString s("i love cows.  cows are the bestbest.  cows");
  const char* cows = "cows";
  const char* penguins = "penguins";
  const char* moo = "moo";
  const char* iReallyLove = "i really love";
  
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(IString("cows"), penguins));
  EXPECT_EQ(IString("i love cows.  cows are the moomoo.  cows"),
	    s.replace(IString("best"), moo));
  EXPECT_EQ(IString("i really love cows.  cows are the bestbest.  cows"),
	    s.replace(IString("i love"), iReallyLove));

  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(std::string("cows"), penguins));
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace("cows", penguins));
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(cows, penguins));


  checkSameType<IString, decltype(s.replace(WIString("cows"), penguins))>();
  EXPECT_EQ(IString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace(WIString("cows"), penguins));

  const wchar_t* wide_penguins = L"penguins";
  checkSameType<WIString, decltype(s.replace("cows", wide_penguins))>();
  EXPECT_EQ(WIString("i love penguins.  penguins are the bestbest.  penguins"),
	    s.replace("cows", wide_penguins));
}

TEST(IStringTests, RemoveChar) {
  IString s("i love cows.  i love penguins.");
  checkSameType<IString, decltype(s.remove(' '))>();
  EXPECT_EQ(IString("ilovecows.ilovepenguins."), s.remove(' '));
  EXPECT_EQ(IString("i love cows.ilove penguins."), s.remove(' ', 10, 20));

  checkSameType<IString, decltype(s.remove(L's'))>();
  EXPECT_EQ(IString("i love cow.  i love penguin."), s.remove(L's'));
}

TEST(IStringTests, RemoveString) {
  IString s("i love cows.  i love penguins.");

  checkSameType<IString, decltype(s.remove(IString("love")))>();
  EXPECT_EQ(IString("i  cows.  i  penguins."), s.remove(IString("love")));
  EXPECT_EQ(IString("i love cows.  i  penguins."),
	    s.remove(IString("love"), 10, 20));
  
  checkSameType<IString, decltype(s.remove(WIString("love")))>();
  EXPECT_EQ(IString("i  cows.  i  penguins."), s.remove(WIString("love")));

  const char* love = "love";
  EXPECT_EQ(IString("i  cows.  i  penguins."), s.remove(std::string("love")));
  EXPECT_EQ(IString("i  cows.  i  penguins."), s.remove("love"));
  EXPECT_EQ(IString("i  cows.  i  penguins."), s.remove(love));
}

TEST(IStringTests, Strip) {
  EXPECT_EQ(IString("i love cows."), IString("i love cows.").strip());
  EXPECT_EQ(IString("i love cows."), IString("  i love cows.   ").strip());  
}

TEST(IStringTests, StartsWith) {
  EXPECT_TRUE(IString("i love cows.").startsWith(IString("i love")));
  EXPECT_FALSE(IString("i love cows.").startsWith(IString("you love")));
  EXPECT_TRUE(IString("i love cows.").startsWith(IString("i love cows.")));
  EXPECT_FALSE(IString("i love cows.").startsWith("i love cows.  moo"));

  const char* iLove = "i love";
  const char* youLove = "you love";
  EXPECT_TRUE(IString("i love cows.").startsWith(std::string("i love")));
  EXPECT_FALSE(IString("i love cows.").startsWith(std::string("you love")));
  EXPECT_TRUE(IString("i love cows.").startsWith("i love"));
  EXPECT_FALSE(IString("i love cows.").startsWith("you love"));
  EXPECT_TRUE(IString("i love cows.").startsWith(iLove));
  EXPECT_FALSE(IString("i love cows.").startsWith(youLove));
}

TEST(IStringTests, EndsWith) {
  EXPECT_TRUE(IString("i love cows.").endsWith(IString("cows.")));
  EXPECT_FALSE(IString("i love cows.").endsWith(IString("penguins.")));
  EXPECT_TRUE(IString("i love cows.").endsWith(IString("i love cows.")));
  EXPECT_FALSE(IString("i love cows.").endsWith("i love cows.  moo"));

  const char* cows = "cows.";
  const char* penguins = "penguins.";
  EXPECT_TRUE(IString("i love cows.").endsWith(std::string("cows.")));
  EXPECT_FALSE(IString("i love cows.").endsWith(std::string("penguins.")));
  EXPECT_TRUE(IString("i love cows.").endsWith("cows."));
  EXPECT_FALSE(IString("i love cows.").endsWith("penguins."));
  EXPECT_TRUE(IString("i love cows.").endsWith(cows));
  EXPECT_FALSE(IString("i love cows.").endsWith(penguins));
}

TEST(IStringTests, IsCase) {
  EXPECT_TRUE(IString("ABCDEF").isUpperCase());
  EXPECT_FALSE(IString("ABCDeF").isUpperCase());
  EXPECT_TRUE(IString("abcdef").isLowerCase());
  EXPECT_FALSE(IString("abcdeF").isLowerCase());
}

TEST(IStringTests, All) {
  const auto isDigit = [](char c) { return std::isdigit(c); };
  EXPECT_TRUE(IString("1239842341").all(isDigit));
  EXPECT_FALSE(IString("12345a").all(isDigit));
  EXPECT_TRUE(IString("12345a").all(isDigit, 2, 4));
}

TEST(IStringTests, Any) {
  const auto isDigit = [](char c) { return std::isdigit(c); };
  EXPECT_TRUE(IString("abcd9ef").any(isDigit));
  EXPECT_FALSE(IString("zqsdfsa").any(isDigit));
  EXPECT_TRUE(IString("abc4def").any(isDigit, 2, 4));
  EXPECT_FALSE(IString("abc4def").any(isDigit, 4, 6));
}

TEST(IStringTests, AssignByCopyingIString) {
  const TestAllocator allocator("TEST");
  const TestAllocator otherAllocator("OTHER");
  TestIString src("Hello", allocator);
  TestIString dest("There!", otherAllocator);

  EXPECT_EQ("TEST", src.allocator().name());  
  EXPECT_EQ(5, src.size());
  EXPECT_EQ("Hello", std::string(src.begin(), src.end()));

  EXPECT_EQ("OTHER", dest.allocator().name());
  EXPECT_EQ(6, dest.size());
  EXPECT_EQ("There!", std::string(dest.begin(), dest.end()));

  dest = src;

  EXPECT_EQ("TEST", dest.allocator().name());
  EXPECT_FALSE(dest.allocator().moved());
  EXPECT_EQ(5, dest.size());
  EXPECT_EQ("Hello", std::string(dest.begin(), dest.end()));
  
  EXPECT_EQ("TEST", src.allocator().name());  
  EXPECT_EQ(5, src.size());
  EXPECT_EQ("Hello", std::string(src.begin(), src.end()));
}

TEST(IStringTests, AssignByCopyingIStringOfOtherType) {
  const TestAllocator allocator("TEST");
  const TestAllocator otherAllocator("OTHER");
  TestIString src("Hello", allocator);
  TestWIString dest(L"There!", otherAllocator);

  EXPECT_EQ("TEST", src.allocator().name());  
  EXPECT_EQ(5, src.size());
  EXPECT_EQ("Hello", std::string(src.begin(), src.end()));

  EXPECT_EQ("OTHER", dest.allocator().name());
  EXPECT_EQ(6, dest.size());
  EXPECT_EQ(L"There!", std::wstring(dest.begin(), dest.end()));

  dest = src;

  EXPECT_EQ("OTHER", dest.allocator().name());
  EXPECT_EQ(5, dest.size());
  EXPECT_EQ(L"Hello", std::wstring(dest.begin(), dest.end()));
  
  EXPECT_EQ("TEST", src.allocator().name());  
  EXPECT_EQ(5, src.size());
  EXPECT_EQ("Hello", std::string(src.begin(), src.end()));
}

TEST(IStringTests, AssignByCopyingStdString) {
  const TestAllocator allocator("TEST");
  TestIString s("Hello", allocator);

  EXPECT_EQ("TEST", s.allocator().name());  
  EXPECT_EQ(5, s.size());
  EXPECT_EQ("Hello", std::string(s.begin(), s.end()));

  s = std::string("Sir");
  EXPECT_EQ("TEST", s.allocator().name());
  EXPECT_EQ(3, s.size());
  EXPECT_EQ("Sir", std::string(s.begin(), s.end()));

  s = std::string("");
  EXPECT_EQ("TEST", s.allocator().name());
  EXPECT_EQ(0, s.size());
  EXPECT_EQ("", std::string(s.begin(), s.end()));
  
  TestWIString ws("Hello", allocator);
  EXPECT_EQ("TEST", ws.allocator().name());  
  EXPECT_EQ(5, ws.size());
  EXPECT_EQ(L"Hello", std::wstring(ws.begin(), ws.end()));

  ws = std::string("Madam,");
  EXPECT_EQ("TEST", ws.allocator().name());
  EXPECT_EQ(6, ws.size());
  EXPECT_EQ(L"Madam,", std::wstring(ws.begin(), ws.end()));
}

TEST(IStringTests, AssingByCopyingCharArray) {
  const TestAllocator allocator("TEST");
  TestIString s("Hello", allocator);

  EXPECT_EQ("TEST", s.allocator().name());  
  EXPECT_EQ(5, s.size());
  EXPECT_EQ("Hello", std::string(s.begin(), s.end()));

  s = "Sir";
  EXPECT_EQ("TEST", s.allocator().name());
  EXPECT_EQ(3, s.size());
  EXPECT_EQ("Sir", std::string(s.begin(), s.end()));

  TestWIString ws("Hello", allocator);
  EXPECT_EQ("TEST", ws.allocator().name());  
  EXPECT_EQ(5, ws.size());
  EXPECT_EQ(L"Hello", std::wstring(ws.begin(), ws.end()));

  ws = std::string("Madam,");
  EXPECT_EQ("TEST", ws.allocator().name());
  EXPECT_EQ(6, ws.size());
  EXPECT_EQ(L"Madam,", std::wstring(ws.begin(), ws.end()));
}

TEST(IStringTests, AssignByMove) {
  const TestAllocator allocator("TEST");
  const TestAllocator otherAllocator("OTHER");
  TestIString src("Hello", allocator);
  TestIString dest("There!", otherAllocator);

  EXPECT_EQ("TEST", src.allocator().name());  
  EXPECT_EQ(5, src.size());
  EXPECT_EQ("Hello", std::string(src.begin(), src.end()));

  EXPECT_EQ("OTHER", dest.allocator().name());
  EXPECT_EQ(6, dest.size());
  EXPECT_EQ("There!", std::string(dest.begin(), dest.end()));

  dest = std::move(src);

  EXPECT_EQ("TEST", dest.allocator().name());
  EXPECT_TRUE(dest.allocator().moved());
  EXPECT_EQ(5, dest.size());
  EXPECT_EQ("Hello", std::string(dest.begin(), dest.end()));
  
  EXPECT_EQ(0, src.size());
  EXPECT_EQ("", std::string(src.begin(), src.end()));
}

TEST(IStringTests, EqualityOperator) {
  IString s("Cows go moo");
  IString same(std::string("Cows go moo"));
  IString different("Penguins are cute");

  EXPECT_TRUE(s == same);
  EXPECT_FALSE(s == different);

  EXPECT_TRUE(s == std::string("Cows go moo"));
  EXPECT_TRUE(std::string("Cows go moo") == s);
  EXPECT_FALSE(s == std::string("Penguins are cute"));

  EXPECT_TRUE(s == "Cows go moo");
  EXPECT_TRUE("Cows go moo" == s);
  EXPECT_FALSE("Penguins are cute" == s);

  WIString wide_same(L"Cows go moo");
  WIString wide_different(L"Penguins are cute");
  EXPECT_TRUE(s == wide_same);
  EXPECT_TRUE(wide_same == s);
  EXPECT_FALSE(s == wide_different);
  EXPECT_FALSE(wide_different == s);

  EXPECT_TRUE(s == std::wstring(L"Cows go moo"));
  EXPECT_TRUE(std::wstring(L"Cows go moo") == s);
  EXPECT_FALSE(s == std::wstring(L"Penguins are cute"));

  EXPECT_TRUE(s == L"Cows go moo");
  EXPECT_TRUE(L"Cows go moo" == s);
  EXPECT_FALSE(L"Penguins are cute" == s);
}

TEST(IStringTests, InequalityOperator) {
  IString s("Cows go moo");
  IString same(std::string("Cows go moo"));
  IString different("Penguins are cute");

  EXPECT_FALSE(s != same);
  EXPECT_TRUE(s != different);

  EXPECT_FALSE(s != std::string("Cows go moo"));
  EXPECT_FALSE(std::string("Cows go moo") != s);
  EXPECT_TRUE(s != std::string("Penguins are cute"));

  EXPECT_FALSE(s != "Cows go moo");
  EXPECT_FALSE("Cows go moo" != s);
  EXPECT_TRUE("Penguins are cute" != s);

  WIString wide_same(L"Cows go moo");
  WIString wide_different(L"Penguins are cute");
  EXPECT_FALSE(s != wide_same);
  EXPECT_FALSE(wide_same != s);
  EXPECT_TRUE(s != wide_different);
  EXPECT_TRUE(wide_different != s);

  EXPECT_FALSE(s != std::wstring(L"Cows go moo"));
  EXPECT_FALSE(std::wstring(L"Cows go moo") != s);
  EXPECT_TRUE(s != std::wstring(L"Penguins are cute"));

  EXPECT_FALSE(s != L"Cows go moo");
  EXPECT_FALSE(L"Cows go moo" != s);
  EXPECT_TRUE(L"Penguins are cute" != s);
}

TEST(IStringTests, LessThanOperator) {
  IString s("abcdef");
  IString equal(std::string("abcdef"));
  IString less(std::string("abccef"));

  EXPECT_FALSE(s < s);
  EXPECT_FALSE(s < equal);
  EXPECT_TRUE(less < s);
  EXPECT_FALSE(s < less);

  EXPECT_TRUE(std::string("abccef") < s);
  EXPECT_FALSE(std::string("abcdef") < s);
  EXPECT_TRUE(s < std::string("abceef"));
  EXPECT_FALSE(s < std::string("abccef"));

  EXPECT_TRUE("abccef" < s);
  EXPECT_FALSE("abcdef" < s);
  EXPECT_TRUE(s < "abceef");
  EXPECT_FALSE(s < "abccef");

  WIString wide_equal(std::wstring(L"abcdef"));
  WIString wide_less(std::wstring(L"abccef"));
  WIString wide_greater(std::wstring(L"abcdeg"));

  EXPECT_TRUE(s < wide_greater);
  EXPECT_FALSE(s < wide_equal);
  EXPECT_FALSE(wide_equal < s);
  EXPECT_TRUE(wide_less < s);

  EXPECT_TRUE(s < std::wstring(L"abcdeg"));
  EXPECT_FALSE(s < std::wstring(L"abcdef"));
  EXPECT_TRUE(std::wstring(L"abccef") < s);
  EXPECT_FALSE(std::wstring(L"abcdeg") < s);

  EXPECT_TRUE(s < L"abcdeg");
  EXPECT_FALSE(s < L"abcdef");
  EXPECT_TRUE(L"abccef" < s);
  EXPECT_FALSE(L"abcdeg" < s);
}

TEST(IStringTests, LessThanOrEqualOperator) {
  IString s("abcdef");
  IString equal(std::string("abcdef"));
  IString less(std::string("abccef"));

  EXPECT_TRUE(s <= s);
  EXPECT_TRUE(s <= equal);
  EXPECT_TRUE(less <= s);
  EXPECT_FALSE(s <= less);

  EXPECT_TRUE(std::string("abccef") <= s);
  EXPECT_TRUE(std::string("abcdef") <= s);
  EXPECT_TRUE(s <= std::string("abceef"));
  EXPECT_FALSE(s <= std::string("abccef"));

  EXPECT_TRUE("abccef" <= s);
  EXPECT_TRUE("abcdef" <= s);
  EXPECT_TRUE(s <= "abceef");
  EXPECT_FALSE(s <= "abccef");

  WIString wide_equal(std::wstring(L"abcdef"));
  WIString wide_less(std::wstring(L"abccef"));
  WIString wide_greater(std::wstring(L"abcdeg"));

  EXPECT_TRUE(s <= wide_greater);
  EXPECT_TRUE(s <= wide_equal);
  EXPECT_FALSE(wide_greater <= s);
  EXPECT_TRUE(wide_less <= s);

  EXPECT_TRUE(s <= std::wstring(L"abcdeg"));
  EXPECT_TRUE(s <= std::wstring(L"abcdef"));
  EXPECT_TRUE(std::wstring(L"abccef") <= s);
  EXPECT_FALSE(std::wstring(L"abcdeg") <= s);

  EXPECT_TRUE(s <= L"abcdeg");
  EXPECT_TRUE(s <= L"abcdef");
  EXPECT_TRUE(L"abccef" <= s);
  EXPECT_FALSE(L"abcdeg" <= s);
}

TEST(IStringTests, GreaterThanOperator) {
  IString s("abcdef");
  IString equal(std::string("abcdef"));
  IString less(std::string("abccef"));

  EXPECT_FALSE(s > s);
  EXPECT_FALSE(s > equal);
  EXPECT_FALSE(less > s);
  EXPECT_TRUE(s > less);

  EXPECT_TRUE(std::string("abceef") > s);
  EXPECT_FALSE(std::string("abcdef") > s);
  EXPECT_FALSE(s > std::string("abceef"));
  EXPECT_TRUE(s > std::string("abccef"));

  EXPECT_TRUE("abcdeg" > s);
  EXPECT_FALSE("abcdef" > s);
  EXPECT_TRUE(s > "abccef");
  EXPECT_FALSE(s > "abddef");

  WIString wide_equal(std::wstring(L"abcdef"));
  WIString wide_less(std::wstring(L"abccef"));
  WIString wide_greater(std::wstring(L"abcdeg"));

  EXPECT_TRUE(s > wide_less);
  EXPECT_FALSE(s > wide_greater);
  EXPECT_FALSE(wide_equal > s);
  EXPECT_TRUE(wide_greater > s);

  EXPECT_TRUE(s > std::wstring(L"abcceg"));
  EXPECT_FALSE(s > std::wstring(L"abcdef"));
  EXPECT_TRUE(std::wstring(L"abceef") > s);
  EXPECT_FALSE(std::wstring(L"abb") > s);

  EXPECT_TRUE(s > L"abcdee");
  EXPECT_FALSE(s > L"abcdef");
  EXPECT_TRUE(L"abcdgg" > s);
  EXPECT_FALSE(L"abcdee" > s);
}

TEST(IStringTests, GreaterThanOrEqualOperator) {
  IString s("abcdef");
  IString equal(std::string("abcdef"));
  IString less(std::string("abccef"));

  EXPECT_TRUE(s >= s);
  EXPECT_TRUE(s >= equal);
  EXPECT_FALSE(less >= s);
  EXPECT_TRUE(s >= less);

  EXPECT_TRUE(std::string("abceef") >= s);
  EXPECT_TRUE(std::string("abcdef") >= s);
  EXPECT_FALSE(std::string("abccef") >= s);
  EXPECT_FALSE(s >= std::string("abceef"));
  EXPECT_TRUE(s >= std::string("abcdef"));
  EXPECT_TRUE(s >= std::string("abccef"));

  EXPECT_TRUE("abcdeg" >= s);
  EXPECT_TRUE("abcdef" >= s);
  EXPECT_FALSE("abcde" >= s);
  EXPECT_TRUE(s >= "abccef");
  EXPECT_TRUE(s >= "abcdef");
  EXPECT_FALSE(s >= "abcdefg");

  WIString wide_equal(std::wstring(L"abcdef"));
  WIString wide_less(std::wstring(L"abccef"));
  WIString wide_greater(std::wstring(L"abcdeg"));

  EXPECT_TRUE(s >= wide_less);
  EXPECT_TRUE(s >= wide_equal);
  EXPECT_FALSE(s >= wide_greater);
  EXPECT_FALSE(wide_less >= s);
  EXPECT_TRUE(wide_equal >= s);
  EXPECT_TRUE(wide_greater > s);

  EXPECT_TRUE(s >= std::wstring(L"abcceg"));
  EXPECT_TRUE(s >= std::wstring(L"abcdef"));
  EXPECT_FALSE(s >= std::wstring(L"abcdefg"));
  EXPECT_TRUE(std::wstring(L"abceef") >= s);
  EXPECT_TRUE(std::wstring(L"abcdef") >= s);
  EXPECT_FALSE(std::wstring(L"abb") >= s);

  EXPECT_TRUE(s >= L"abcdee");
  EXPECT_TRUE(s >= L"abcdef");
  EXPECT_FALSE(s >= L"abcdeg");
  EXPECT_TRUE(L"abcdgg" >= s);
  EXPECT_TRUE(L"abcdef" >= s);
  EXPECT_FALSE(L"abcdee" >= s);
}

TEST(IStringTests, ConcatenateWithPlus) {
  IString s("cows go");

  checkSameType<IString, decltype(s + IString(" moo"))>();
  EXPECT_EQ("cows go moo", s + IString(" moo"));
  checkSameType<IString, decltype(s + std::string(" mu"))>();
  EXPECT_EQ("cows go mu", s + std::string(" mu"));
  checkSameType<IString, decltype(s + " waka waka")>();
  EXPECT_EQ("cows go waka waka", s + " waka waka");

  checkSameType<IString, decltype(IString("zen ") + s)>();
  EXPECT_EQ("zen cows go", IString("zen ") + s);
  checkSameType<IString, decltype(std::string("brown ") + s)>();
  EXPECT_EQ("brown cows go", std::string("brown ") + s);
  checkSameType<IString, decltype("big " + s)>();
  EXPECT_EQ("big cows go", "big " + s);

  WIString ws("love penguins");

  checkSameType<WIString, decltype(ws + IString(" a lot"))>();
  EXPECT_EQ("love penguins a lot", ws + IString(" a lot"));
  checkSameType<WIString, decltype(ws + std::string(" so much"))>();
  EXPECT_EQ("love penguins so much", ws + std::string(" so much"));
  checkSameType<WIString, decltype(ws + " forever")>();
  EXPECT_EQ("love penguins forever", ws + " forever");

  checkSameType<WIString, decltype(IString("i ") + ws)>();
  EXPECT_EQ("i love penguins", IString("i ") + ws);
  checkSameType<WIString, decltype(std::string("you ") + ws)>();
  EXPECT_EQ("you love penguins", std::string("you ") + ws);
  checkSameType<WIString, decltype("we " + ws)>();
  EXPECT_EQ("we love penguins", "we " + ws);
}

TEST(IStringTests, WriteToOstream) {
  IString s("cows go moo");
  std::ostringstream msg;

  msg << s;
  EXPECT_EQ("cows go moo", msg.str());
}

TEST(IStringTests, Join) {
  std::vector<std::string> VALUES{ "one", "two", "three" };
  EXPECT_EQ("one, two, three", ", "_is.join(VALUES));
}

TEST(IStringTests, SplitByString) {
  std::vector<IString> truth{"one"_is, "two"_is, "three"_is, "four"_is};
  EXPECT_EQ(truth, "one::two::three::four"_is.split("::"_is).toVector());

  truth = std::vector<IString>{"one"_is, "two::three::four"_is};
  EXPECT_EQ(truth, "one::two::three::four"_is.split("::"_is, 1).toVector());
}

TEST(IStringTests, SplitByRegex) {
  std::vector<IString> truth{"one"_is, "two"_is, "three"_is, "four"_is};
  std::regex sep("\\s+");
  EXPECT_EQ(truth, "one two   three  four"_is.split(sep).toVector());

  truth = std::vector<IString>{"one"_is, "two   three  four"_is};
  EXPECT_EQ(truth, "one two   three  four"_is.split(sep, 1).toVector());
}

TEST(IStringTests, Format) {
  IString pattern("%s %8.6f %+10d");

  EXPECT_EQ("cows 0.625000       +123", pattern.fmt("cows", 0.625, 123));
  EXPECT_EQ("I love to go moo!", "I love to go %s!"_is % IString("moo"));
  EXPECT_EQ("cows:3", "%s:%d"_is % std::make_tuple("cows", 3));
}
