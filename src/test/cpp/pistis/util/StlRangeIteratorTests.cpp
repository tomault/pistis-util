#include <pistis/util/StlRangeIterator.hpp>
#include <pistis/testing/Iterators.hpp>
#include <gtest/gtest.h>

using namespace pistis::util;
namespace pti = pistis::testing::iterators;

TEST(StlRangeIteratorTests, BasicOperations) {
  const std::vector<uint32_t> truth{ 5, 2, 1, 9, 6 };
  auto iteratorFactory = [truth]() {
    return makeStlRangeIterator(truth.cbegin(), truth.cend());
  };

  SCOPED_TRACE("RangeIteratorTests::BasicOperations");
  pti::testSimpleIterator(iteratorFactory, truth);
}

TEST(StlRangeIteratorTests, Write) {
  const std::vector<uint32_t> truth{7, 1, 3, 9 };
  const std::vector<uint32_t> dataToWrite{ 3, 2, 8, 5 };
  std::vector<uint32_t> data(truth);
  auto iteratorFactory = [&data]() {
    return makeStlRangeIterator(data.begin(), data.end());
  };

  SCOPED_TRACE("RangeIteratorTests::Write");
  pti::testMutableSimpleIterator(iteratorFactory, truth, dataToWrite);
}
