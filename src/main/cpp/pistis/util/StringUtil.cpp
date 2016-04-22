#include "StringUtil.hpp"

using namespace pistis::util;

SplitIterator::SplitIterator():
    _current(), _next(), _end(), _sep(), _done(true) {
  // Intentionally left blank
}

SplitIterator::SplitIterator(const std::string& text,
			     const std::string& separator):
    _current(text.begin()),
    _next(_findFirst(text.begin(), text.end(), separator)),
    _end(text.end()), _sep(separator), _done(_current == _end) {
  // Intentionally left blank
}

SplitIterator::SplitIterator(const std::string::const_iterator& begin,
			     const std::string::const_iterator& end,
			     const std::string& separator):
    _current(begin), _next(_findFirst(begin, end, separator)), _end(end),
    _sep(separator), _done(begin == end) {
  // Intentionally left blank
}

SplitIterator::SplitIterator(const SplitIterator& other):
    _current(other._current), _next(other._next), _end(other._end),
    _sep(other._sep), _done(other._done) {
  // Intentionally left blank
}

SplitIterator::SplitIterator(SplitIterator&& other):
    _current(std::move(other._current)), _next(std::move(other._next)),
    _end(std::move(other._end)), _sep(std::move(other._sep)),
    _done(other._done) {
  // Intentionally left blank
}

SplitIterator& SplitIterator::operator=(const SplitIterator& other) {
  if (this != &other) {
    _current= other._current;
    _next= other._next;
    _end= other._end;
    _sep= other._sep;
  }
  return *this;
}

SplitIterator& SplitIterator::operator=(SplitIterator&& other) {
  if (this != &other) {
    _current= std::move(other._current);
    _next= std::move(other._next);
    _end= std::move(other._end);
    _sep= std::move(other._sep);
    _done= other._done;
  }
  return *this;
}

void SplitIterator::_advance() {
  _current= _next;
  if (_next == _end) {
    _done= true;
  } else {
    _current += _sep.size();
    _next= _findNext(_current, _end, _sep);
  }
}

std::string::const_iterator SplitIterator::_findFirst(
    const std::string::const_iterator& begin,
    const std::string::const_iterator& end,
    const std::string& sep
) {
  return (begin != end) ? _findNext(begin, end, sep) : begin;
}

std::string::const_iterator SplitIterator::_findNext(
    const std::string::const_iterator& begin,
    const std::string::const_iterator& end,
    const std::string& sep
) {
  auto i= begin;
  if (sep.empty()) {
    return ++i;
  } else {
    auto j= i + sep.size();
    while (j <= end) {
      if (std::equal(i, j, sep.begin())) {
	return i;
      }
      ++i; ++j;
    }
    return end;
  }
}
