/** @file ImmutableListTests.cpp
 *
 *  Unit tests for pistis::util::ImmutableList
 */
#include <pistis/util/ImmutableList.hpp>

#include <pistis/exceptions/OutOfRangeError.hpp>
#include <pistis/testing/Allocator.hpp>
#include <gtest/gtest.h>
#include <sstream>
#include <stdint.h>

using namespace pistis::exceptions;
using namespace pistis::util;
namespace pt = pistis::testing;

namespace {
  typedef ::pt::Allocator<uint32_t> TestAllocator;
  typedef ImmutableList<uint32_t, TestAllocator> UInt32List;
  typedef std::vector<uint32_t> TrueList;

  inline std::string toStr(bool v) { return v ? "true" : "false"; }

  template <typename T>
  inline std::unique_ptr<T> make_result(const T& result) {
    return std::unique_ptr<T>(new T(result));
  }
  

  template <typename Item, typename Allocator>
  std::unique_ptr<::testing::AssertionResult> verifyListAccessors(
      std::unique_ptr<::testing::AssertionResult> prior,
      const std::vector<Item>& truth,
      const ImmutableList<Item, Allocator>& list
  ) {
    if (!*prior) {
      return std::move(prior);
    }
    if (truth.empty() != list.empty()) {
      return make_result(
	  ::testing::AssertionFailure()
	    << "truth.empty() != list.empty() [ " << toStr(truth.empty())
	    << " != " << toStr(list.empty()) << " ]"
      );
    }
    if (truth.size() != list.size()) {
      return make_result(
	  ::testing::AssertionFailure()
	      << "truth.size() != list.size() [ " << truth.size() << " != "
	      << list.size() << " ]"
      );
    }

    // list.size() == truth.size() by prior assertion
    if (!list.size()) {
      // Don't check front() and back()
    } else if (truth.front() != list.front()) {
      return make_result(
	  ::testing::AssertionFailure()
	      << "truth.front() != list.front() [ " << truth.front() << " != "
	      << list.front() << " ]"
      );
    } else if (truth.back() != list.back()) {
      return make_result(
	   ::testing::AssertionFailure()
	       << "truth.back() != list.back() [ " << truth.back() << " != "
	       << list.back() << " ]"
      );
    }
    return make_result(::testing::AssertionSuccess());
  }
  
  template <typename ListIterator, typename TruthIterator>
  std::unique_ptr<::testing::AssertionResult> verifyRange(
      std::unique_ptr<::testing::AssertionResult> prior,
      TruthIterator truthBegin,
      ListIterator listBegin, ListIterator listEnd) {
    if (!*prior) {
      return std::move(prior);
    }
    
    auto i = truthBegin;
    uint32_t ndx = 0;
    for (auto j = listBegin; j != listEnd; ++i, ++j, ++ndx) {
      if (*i != *j) {
	return make_result(
	    ::testing::AssertionFailure()
	       << "truth[" << ndx << "] (which is " << *i << " ) != list["
	       << ndx << "] (which is " << *j << ")"
	);
      }
    }
    return make_result(::testing::AssertionSuccess());
  }
  
  template <typename Item, typename Allocator>
  ::testing::AssertionResult verifyList(
      const std::vector<Item>& truth,
      const ImmutableList<Item, Allocator>& list
  ) {
    std::unique_ptr<::testing::AssertionResult> result =
        make_result(::testing::AssertionSuccess());

    result = verifyListAccessors(std::move(result), truth, list);
    result = verifyRange(std::move(result), truth.begin(), list.begin(),
			 list.end());
    result = verifyRange(std::move(result), truth.cbegin(), list.cbegin(),
			 list.cend());
    
    return *result;
  }
}

TEST(ImmutableListTests, CreateEmpty) {
  const TestAllocator allocator("TEST_1");
  UInt32List list(allocator);

  EXPECT_EQ(allocator.name(), list.allocator().name());
  EXPECT_TRUE(verifyList(TrueList(), list));
}

TEST(ImmutableListTests, CreateFromSingleItem) {
  const TestAllocator allocator("TEST_1");
  UInt32List list(3, 16, allocator);
  TrueList truth{ 16, 16, 16 };

  EXPECT_EQ(allocator.name(), list.allocator().name());
  EXPECT_TRUE(verifyList(truth, list));
}

TEST(ImmutableListTests, CreateFromLengthAndIterator) {
  const std::vector<uint32_t> DATA{ 5, 16, 2, 23 };
  const TestAllocator allocator("TEST_1");
  TrueList truth(DATA);
  UInt32List list(DATA.size(), DATA.begin(), allocator);

  EXPECT_EQ(allocator.name(), list.allocator().name());
  EXPECT_TRUE(verifyList(truth, list));
}

TEST(ImmutableListTests, CreateFromRange) {
  const std::vector<uint32_t> DATA{ 5, 16, 2, 23 };
  const TestAllocator allocator("TEST_1");
  TrueList truth(DATA);
  UInt32List list(DATA.begin(), DATA.end(), allocator);

  EXPECT_EQ(allocator.name(), list.allocator().name());
  EXPECT_TRUE(verifyList(truth, list));
}

TEST(ImmutableListTests, CreateFromInitializerList) {
  const TestAllocator allocator("TEST_1");
  TrueList truth{ 7, 4, 9, 22, 27 };
  UInt32List list{ { 7, 4, 9, 22, 27 }, allocator };

  EXPECT_EQ(allocator.name(), list.allocator().name());
  EXPECT_TRUE(verifyList(truth, list));
}

TEST(ImmutableListTests, CreateFromCopy) {
  const TestAllocator allocator("TEST_1");
  const TestAllocator otherAllocator("TEST_2");
  const std::vector<uint32_t> DATA{ 4, 12, 9 };
  const TrueList truth(DATA);
  UInt32List list( DATA.begin(), DATA.end(), allocator);

  ASSERT_EQ(allocator.name(), list.allocator().name());
  ASSERT_TRUE(verifyList(truth, list));

  UInt32List copy(list);
  EXPECT_EQ(allocator.name(), copy.allocator().name());
  EXPECT_TRUE(verifyList(truth, copy));
  EXPECT_EQ(allocator.name(), list.allocator().name());
  EXPECT_TRUE(verifyList(truth, list));

  UInt32List copyWithNewAllocator(list, otherAllocator);
  EXPECT_EQ(otherAllocator.name(), copyWithNewAllocator.allocator().name());
  EXPECT_TRUE(verifyList(truth, copyWithNewAllocator));
  EXPECT_EQ(allocator.name(), list.allocator().name());
  EXPECT_TRUE(verifyList(truth, list));
}

TEST(ImmutableListTests, At) {
  const std::vector<uint32_t> DATA{ 3, 2, 1, 4 };
  UInt32List list(DATA.begin(), DATA.end());

  for (uint32_t i = 0; i != DATA.size(); ++i) {
    EXPECT_EQ(DATA[i], list.at(i));
  }
  EXPECT_THROW(list.at(list.size()), std::range_error);
}

TEST(ImmutableListTests, Sublist) {
  const TestAllocator allocator("TEST_1");
  const TrueList truth{3, 4, 5, 6};
  UInt32List list{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, allocator };
  UInt32List sublist(list.sublist(3, 7));

  EXPECT_EQ(allocator.name(), sublist.allocator().name());
  EXPECT_TRUE(verifyList(truth, sublist));

  EXPECT_TRUE(verifyList(TrueList(), list.sublist(3, 3)));
  EXPECT_TRUE(verifyList(TrueList(), list.sublist(10, 10)));

  EXPECT_THROW(list.sublist(11, 15), OutOfRangeError);
  EXPECT_THROW(list.sublist(10, 11), OutOfRangeError);
  EXPECT_THROW(list.sublist( 7,  3), IllegalValueError);
}

TEST(ImmutableListTests, Map) {
  const TestAllocator allocator("TEST_1");
  const std::vector<std::string> truth{ "5", "2", "7", "13" };
  UInt32List list{ { 5, 2, 7, 13 }, allocator };
  ImmutableList< std::string, pt::Allocator<std::string> > mapped(
      list.map([](uint32_t x) {
	  std::ostringstream tmp;
	  tmp << x;
	  return tmp.str();
      })
  );

  EXPECT_EQ(allocator.name(), mapped.allocator().name());
  EXPECT_TRUE(verifyList(truth, mapped));  
}

TEST(ImmutableListTest, Reduce) {
  UInt32List list{ 5, 2, 7, 13 };
  UInt32List emptyList;
  auto concat = [](const std::string& s, uint32_t x) {
      std::ostringstream tmp;
      tmp << s << ", " << x;
      return tmp.str();
  };
  auto mix = [](uint32_t x, uint32_t y) { return x * 100 + y; };

  EXPECT_EQ("**, 5, 2, 7, 13", list.reduce(concat, "**"));
  EXPECT_EQ("**", emptyList.reduce(concat, "**"));
  EXPECT_EQ(5020713, list.reduce(mix));
  EXPECT_THROW(emptyList.reduce(mix), IllegalStateError);
}

TEST(ImmutableListTests, ListEquality) {
  UInt32List list{ 3, 2, 5, 10, 7 };
  UInt32List same{ 3, 2, 5, 10, 7 };
  UInt32List different{ 3, 2, 9, 10, 7 };

  EXPECT_TRUE(list == same);
  EXPECT_FALSE(list == different);
  EXPECT_TRUE(list != different);
  EXPECT_FALSE(list != same);
}

TEST(ImmutableListTests, RandomAccess) {
  const std::vector<uint32_t> DATA{ 3, 2, 5, 10, 7 };
  UInt32List list(DATA.begin(), DATA.end());

  for (uint32_t i = 0; i < DATA.size(); ++i) {
    EXPECT_EQ(DATA[i], list[i]);
  }
}
