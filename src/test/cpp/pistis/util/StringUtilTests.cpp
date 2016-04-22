/** @file StringUtilTests.cpp
 *
 *  Unit tests for the utilties in pistis/util/StringUtil.hpp.
 */

#include <pistis/util/StringUtil.hpp>
#include <gtest/gtest.h>
#include <sstream>

using namespace pistis::util;

namespace {
  std::string checkSplit(const std::string& input,
			 const std::vector<std::string>& values,
			 const std::vector<std::string>& truth) {
    if (values != truth) {
      std::ostringstream msg;
      msg << "Split of \"" << input << "\" produced \""
	  << join(values.begin(), values.end(), "\", \"")
	  << "\" (" << values.size() << " values); it should have produced \""
	  << join(truth.begin(), truth.end(), "\", \"") << "\" ("
	  << truth.size() << " values)";
      return msg.str();
    }
    return std::string();
  }

  std::string checkSplit(const std::string& input,
			 const std::string& separator,
			 const std::vector<std::string>& truth) {
    std::vector<std::string> values;
    std::copy(SplitIterator(input, separator), SplitIterator(),
	      std::back_inserter(values));
    return checkSplit(input, values, truth);
  }
}

TEST(StringUtilTests, JoinTest) {
  std::vector<std::string> empty;
  std::vector<std::string> oneItem{"one"};
  std::vector<std::string> manyItems{"one", "two", "three"};
  std::ostringstream tmp;

  tmp << join(empty.begin(), empty.end(), ", ") << "\n"
      << join(oneItem.begin(), oneItem.end(), ", ") << "\n"
      << join(manyItems.begin(), manyItems.end(), ", ") << "\n";
  EXPECT_EQ(tmp.str(), "\none\none, two, three\n");

  EXPECT_EQ(joinAsString(empty.begin(), empty.end(), ", "), "");
  EXPECT_EQ(joinAsString(oneItem.begin(), oneItem.end(), ", "), "one");
  EXPECT_EQ(joinAsString(manyItems.begin(), manyItems.end(), ", "),
		    "one, two, three");
}

TEST(StringUtilTests, SplitTest) {
  std::vector<std::string> result;
  std::string errMsg;

  errMsg= checkSplit("", ",", {});
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  errMsg= checkSplit("abc", ",", {"abc"});
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  
  errMsg= checkSplit("ab,c,defg", ",", {"ab", "c", "defg"});
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  errMsg= checkSplit(",a,b,c", ",", {"", "a", "b", "c"});
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  errMsg= checkSplit("a,b,c,", ",", {"a", "b", "c", ""});
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  errMsg= checkSplit(",", ",", {"", ""});
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  errMsg= checkSplit("a,b,,c", ",", { "a", "b", "", "c" });
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  errMsg= checkSplit("", "", {});
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  errMsg= checkSplit("a", "", { "a" });
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  errMsg= checkSplit("abc", "", { "a", "b", "c" });
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  splitTo("a,b,c", ",", std::back_inserter(result));
  errMsg= checkSplit("a,b,c", result, { "a", "b", "c" });
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  result.clear();
  splitTo("a,b,c", ",", 1, std::back_inserter(result));
  errMsg= checkSplit("a,b,c (limit=1)", result, { "a", "b,c" });
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  result.clear();
  splitTo("a,b,c", ",", 0, std::back_inserter(result));
  errMsg= checkSplit("a,b,c (limit=0)", result, { "a,b,c" });
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  result.clear();
  splitTo("a,b,c", ",", -1, std::back_inserter(result));
  errMsg= checkSplit("a,b,c (limit=-1)", result, { "a", "b", "c" });
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  result= split("aa,bbb,cccc,ddddd", ",");
  errMsg= checkSplit("aa,bbb,cccc,ddddd", result,
		     { "aa", "bbb", "cccc", "ddddd" });
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  result= split("aa,bbb,cccc,ddddd", ",", 2);
  errMsg= checkSplit("aa,bbb,cccc,ddddd (limit 2)", result,
		     { "aa", "bbb", "cccc,ddddd" });
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  result= split("aa,bbb,cccc,ddddd", ",", 0);
  errMsg= checkSplit("aa,bbb,cccc,ddddd (limit 0)", result,
		     { "aa,bbb,cccc,ddddd" });
  EXPECT_TRUE(errMsg.empty()) << errMsg;
}

TEST(StringUtilTests, LStripTest) {
  EXPECT_EQ(lstrip("abc"), "abc");
  EXPECT_EQ(lstrip(" \t\nabcdef\n\t "), "abcdef\n\t ");
  EXPECT_EQ(lstrip(" \t\nabcdef\n\t ", "\t "), "\nabcdef\n\t ");
  EXPECT_EQ(lstrip(" \t\n \n\t"), "");
}

TEST(StringUtilTests, RStripTest) {
  EXPECT_EQ(rstrip("abc"), "abc");
  EXPECT_EQ(rstrip(" \t\nabcdef\n\t "), " \t\nabcdef");
  EXPECT_EQ(rstrip(" \t\nabcdef\n\t ", "\t "), " \t\nabcdef\n");
  EXPECT_EQ(rstrip(" \t\n \n\t"), "");
}

TEST(StringUtilTests, StripTest) {
  EXPECT_EQ(strip("abc"), "abc");
  EXPECT_EQ(strip(" \t\nabcdef\n\t "), "abcdef");
  EXPECT_EQ(strip(" \t\nabcdef\n\t ", "\t "), "\nabcdef\n");
  EXPECT_EQ(strip(" \t\n \n\t"), "");
}
