#include <pistis/util/MappingIterator.hpp>
#include <pistis/util/StlRangeIterator.hpp>
#include <pistis/testing/Iterators.hpp>
#include <gtest/gtest.h>
#include <sstream>

using namespace pistis::util;
namespace pti = pistis::testing::iterators;

TEST(MappingIteratorTests, BasicOperations) {
  const std::vector<uint32_t> input{ 7, 6, 1, 4 };
  const std::vector<std::string> truth{ "7", "6", "1", "4" };
  auto toString = [](uint32_t x) {
    std::ostringstream tmp;
    tmp << x;
    return tmp.str();
  };
  auto iteratorFactory = [input, toString]() {
    return makeMappingIterator(
	makeStlRangeIterator(input.begin(), input.end()), toString
    );
  };

  pti::testSimpleIterator(iteratorFactory, truth);	
}
