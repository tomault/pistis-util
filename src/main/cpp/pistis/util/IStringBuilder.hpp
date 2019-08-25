#ifndef __PISTIS__UTIL__ISTRINGBUILDER_HPP__
#define __PISTIS__UTIL__ISTRINGBUILDER_HPP__

// ImmutableString uses the ImmutableStringBuilder in its implementation
// and therefore needs to define it.  Since ImmutableStringBuilder needs
// a definition of ImmutableString to be present when its methods are
// instantiated and vice-versa, both IString.hpp and
// IStringBuilder_.hpp should always be included together.
// IString.hpp pulls in ImmutableStringBuilder_.hpp, so including
// IString.hpp achieves this goal.
#include <pistis/util/IString.hpp>
#include <pistis/util/IStringBuilderManipulators.hpp>

#endif
