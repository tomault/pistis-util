/** @file NameMapTests.cpp
 *
 *  Unit tests for pistis::util::NameMap
 */
#include <pistis/util/NameMap.hpp>
#include <gtest/gtest.h>
#include <sstream>

using namespace pistis::exceptions;
using namespace pistis::util;

namespace {
  class ZeroedInt {
  public:
    ZeroedInt(): v_(0) { }
    ZeroedInt(int v): v_(v) { }
  
    operator int() const { return v_; }
    ZeroedInt& operator=(int v) { v_= v; return *this; }
    ZeroedInt& operator=(const ZeroedInt&)= default;

  private:
    int v_;
  };

  std::ostream& operator<<(std::ostream& out, const ZeroedInt& v) {
    return out << (int)v;
  }

  struct ZeroedIntHasher {
    int operator()(const ZeroedInt& v) const { return (int)v; }
  };
}

TEST(NameMapTests, SimpleConstruction) {
  NameMap<int> map("int");

  EXPECT_EQ(map.valueTypeName(), "int");
  EXPECT_EQ(map.nameCount(), 0);
  EXPECT_EQ(map.valueCount(), 0);
}

TEST(NameMapTests, ConstructFromPairs) {
  static const std::vector<std::pair<int, std::string>> MAPPING{ { 1, "A"}, {4, "B" }, {20, "D" }, { 4, "b" }, { 4, ":B:" }, { 20, "d" } };
  NameMap<int> map("int", MAPPING.begin(), MAPPING.end());
  
  EXPECT_EQ(map.valueTypeName(), "int");
  EXPECT_EQ(map.nameCount(), 6);
  EXPECT_EQ(map.valueCount(), 3);

  EXPECT_EQ(map.get("A"), 1);
  EXPECT_EQ(map.get("B"), 4);
  EXPECT_EQ(map.get("b"), 4);
  EXPECT_EQ(map.get(":B:"), 4);
  EXPECT_EQ(map.get("D"), 20);
  EXPECT_EQ(map.get("d"), 20);

  EXPECT_EQ(map.getName(1), "A");
  EXPECT_EQ(map.getName(4), "B");
  EXPECT_EQ(map.getName(20), "D");
  EXPECT_EQ(map.getName(-1), "");
}

TEST(NameMapTests, ConstructFromInitializerList) {
  NameMap<int> map("int", { { 1, "A"}, {4, "B" }, {20, "D" }, { 4, "b" }, { 4, ":B:" }, { 20, "d" } });
  
  EXPECT_EQ(map.valueTypeName(), "int");
  EXPECT_EQ(map.nameCount(), 6);
  EXPECT_EQ(map.valueCount(), 3);

  EXPECT_EQ(map.get("A"), 1);
  EXPECT_EQ(map.get("B"), 4);
  EXPECT_EQ(map.get("b"), 4);
  EXPECT_EQ(map.get(":B:"), 4);
  EXPECT_EQ(map.get("D"), 20);
  EXPECT_EQ(map.get("d"), 20);

  EXPECT_EQ(map.getName(1), "A");
  EXPECT_EQ(map.getName(4), "B");
  EXPECT_EQ(map.getName(20), "D");
  EXPECT_EQ(map.getName(-1), "");
}

TEST(NameMapTests, AddRValue) {
  NameMap<int> m("int");

  m.add("A", 1);
  m.add("B", 2);
  m.add("C", 1);

  EXPECT_EQ(m.nameCount(), 3);
  EXPECT_EQ(m.valueCount(), 2);
  EXPECT_EQ(m.get("A"), 1);
  EXPECT_EQ(m.get("B"), 2);
  EXPECT_EQ(m.get("C"), 1);
  EXPECT_EQ(m.getName(1), "A");
  EXPECT_EQ(m.getName(2), "B");
  EXPECT_EQ(m.getName(3), "");
}

TEST(NameMapTests, AllNamesFor) {
  static const std::vector<std::pair<int, std::string>> MAPPING{ { 1, "A"}, {4, "B" }, {20, "D" }, { 4, "b" }, { 4, ":B:" }, { 20, "d" } };
  static const std::vector<std::string> NAMES_FOR_1{ "A" };
  static const std::vector<std::string> NAMES_FOR_4{ "B", "b", ":B:" };
  static const std::vector<std::string> NAMES_FOR_20{ "D", "d" };
  static const std::vector<std::string> EMPTY_LIST;
  NameMap<int> map("int", MAPPING.begin(), MAPPING.end());
  
  EXPECT_EQ(map.valueTypeName(), "int");
  EXPECT_EQ(map.nameCount(), 6);
  EXPECT_EQ(map.valueCount(), 3);

  EXPECT_TRUE(map.allNamesFor(1) == NAMES_FOR_1);
  EXPECT_TRUE(map.allNamesFor(4) == NAMES_FOR_4);
  EXPECT_TRUE(map.allNamesFor(20) == NAMES_FOR_20);
  EXPECT_TRUE(map.allNamesFor(-1) == EMPTY_LIST);
}

TEST(NameMapTests, Get) {
  NameMap<ZeroedInt, ZeroedIntHasher> map("int", { 
    { ZeroedInt(1), "A" }, { ZeroedInt(4), "B" }, { ZeroedInt(20), "D" }, 
    { ZeroedInt(4), "b" }, { ZeroedInt(4), ":B:" }, { ZeroedInt(20), "d" }
  });
  ZeroedInt v;
  auto firstCharOfName=
    [&map](const NameMap<ZeroedInt, ZeroedIntHasher>* m,
	   const std::string& name) -> ZeroedInt { 
      return (m == &map) ? (int)name[0] : 0;
    };
  
  EXPECT_EQ(map.valueTypeName(), "int");
  EXPECT_EQ(map.nameCount(), 6);
  EXPECT_EQ(map.valueCount(), 3);

  // Get returning ZeroedInt() as default value
  EXPECT_EQ(map.get("A"), 1);
  EXPECT_EQ(map.get("B"), 4);
  EXPECT_EQ(map.get("b"), 4);
  EXPECT_EQ(map.get(":B:"), 4);
  EXPECT_EQ(map.get("D"), 20);
  EXPECT_EQ(map.get("d"), 20);
  EXPECT_EQ(map.get("Z"), 0);

  // Get with default value
  EXPECT_EQ(map.get("A", -1), 1);
  EXPECT_EQ(map.get("Z", -1), -1);

  // Checked get
  EXPECT_TRUE(map.get("D", v));
  EXPECT_EQ(v, 20);
  v= -99;
  EXPECT_FALSE(map.get("Z", v));
  EXPECT_EQ(v, -99);

  // Get with function for default value
  EXPECT_EQ(map.getWithDefaultFn("b", firstCharOfName), 4);
  EXPECT_EQ(map.getWithDefaultFn("z", firstCharOfName), 122);
}

TEST(NameMapTests, GetName) {
  NameMap<int> map("int", { { 1, "A"}, {4, "B" }, {20, "D" }, { 4, "b" }, { 4, ":B:" }, { 20, "d" } });
  std::string name;
  auto convertMissingToString=
    [&map](const NameMap<int>* m, int v) -> std::string {
      std::ostringstream tmp;
      if (m == &map) {
	tmp << v;
      } else {
	tmp << "m = " << m << ", but should be " << &map;
      }
      return tmp.str();
    };
  
  //  get(int) -> std::string verified in construction tests
  EXPECT_EQ(map.getName(1, "MISSING"), "A");
  EXPECT_EQ(map.getName(4, "MISSING"), "B");
  EXPECT_EQ(map.getName(20, "MISSING"), "D");
  EXPECT_EQ(map.getName(-1, "MISSING"), "MISSING");

  EXPECT_TRUE(map.getName(4, name));
  EXPECT_EQ(name, "B");
  name= "MISSING";
  EXPECT_FALSE(map.getName(-1, name));
  EXPECT_EQ(name, "MISSING");

  EXPECT_EQ(map.getNameWithDefaultFn(20, convertMissingToString), "D");
  EXPECT_EQ(map.getNameWithDefaultFn(-1, convertMissingToString), "-1");
}

TEST(NameMapTests, GetRequired) {
  NameMap<int> map("int", { { 1, "A"}, {4, "B" }, {20, "D" }, { 4, "b" }, { 4, ":B:" }, { 20, "d" } });

  EXPECT_EQ(map.getRequired("A"), 1);
  EXPECT_THROW(map.getRequired("Z"), IllegalValueError);
  EXPECT_EQ(map.getRequiredName(1), "A");
  EXPECT_THROW(map.getRequiredName(-1), IllegalValueError);
}
