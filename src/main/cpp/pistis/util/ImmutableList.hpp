#ifndef __PISTIS__UTIL_IMMUTABLELIST_HPP__
#define __PISTIS__UTIL_IMMUTABLELIST_HPP__

#include <pistis/exceptions/OutOfRangeError.hpp>
#include <pistis/exceptions/IllegalStateError.hpp>
#include <pistis/exceptions/IllegalValueError.hpp>
#include <pistis/util/MappingIterator.hpp>
#include <pistis/util/StlRangeIterator.hpp>
#include <algorithm>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <type_traits>

namespace pistis {
  namespace util {

    /** @brief A list whose content cannot change after its creation.
     *
     *  An ImmutableList implements the STL Container, SequenceContainer
     *  and AllocatorAwareContainer concepts, except for the parts of
     *  those concepts that would violate immutability of the list, such
     *  as operations that add, remove or change items in the list.  These
     *  omissions include the Assignable concept, since assignment would
     *  change the content of the container.
     *
     *  Because ImmutableList instances cannot change after creation, they
     *  are inherently thread-safe and exception-safe.  Copies are made
     *  by sharing the internal list state, and likewise, sublists are made
     *  by sharing the internal state of the parent list but with different
     *  start and end points.  Copying and creating sublists are therefore
     *  O(1) operations.  However, applications should be aware that creating
     *  a small sublist from a large list can cause the internal state of
     *  the large list to persist past the lifetime of the large list.  To
     *  avoid this, applications can use the constructor create an independent
     *  copy.
     *
     *  An ImmutableList differs from a std::array in that its length is
     *  set at runtime instead of compile-time.  An ImmutableList differs
     *  from a const std::vector in that copying and creating sublists are
     *  O(1) operations.
     */
    template <typename Item, typename Allocator = std::allocator<Item> >
    class ImmutableList : public Allocator {
    public:
      typedef Item ItemType;  ///< Type of list's members
      typedef Allocator AllocatorType;  ///< Type of list's allocator

      /** @brief Constant, random-access iterator over list */
      typedef const Item* ConstIterator;
      typedef ConstIterator Iterator; ///< Same as ConstIterator

      /** @name Types required by STL concepts */
      /** @{ */
      typedef Allocator allocator_type;  ///< Same as AllocatorType
      typedef Item value_type;           ///< Same as ItemType
      typedef Item& reference;           ///< Reference to Item
      typedef const Item& const_reference;  ///< Const reference to Item
      typedef ConstIterator const_iterator; ///< Same as ConstIterator
      typedef Iterator iterator;            ///< Same as Iterator
      typedef ssize_t difference_type;   ///< Difference between positions
      typedef size_t size_type;          ///< Type of index/position
      /** @} */
      
    public:
      /** @brief Create an empty list */
      ImmutableList(const Allocator& allocator = Allocator()):
	  Allocator(allocator), members_(), begin_(nullptr), end_(nullptr) {
      }

      /** @brief Create a list with n copies of item */
      ImmutableList(size_t n, const Item& item,
		    const Allocator& allocator = Allocator()):
	  Allocator(allocator),
	  members_(
	      std::allocator_traits<Allocator>::allocate(*this,
							 n * sizeof(Item)),
	      ImmutableList::Cleaner(allocator, n)
	  ),
	  begin_(members_.get()), end_(begin_ + n) {
	for (auto p = begin_; p != end_; ++p) {
	  std::allocator_traits<Allocator>::construct(*this, p, item);
	}
      }

      /** @brief Create a list with n items from the sequence starting at
       *         members.
       */
      template <typename Iterator,
		typename Enabler =
		    typename std::enable_if<!std::is_integral<Iterator>::value,
					    int>::type>
      ImmutableList(size_t n, Iterator members,
		    const Allocator& allocator = Allocator(),
		    Enabler = 0):
	  Allocator(allocator),
	  members_(
	      std::allocator_traits<Allocator>::allocate(*this,
							 n * sizeof(Item)),
	      ImmutableList::Cleaner(allocator, n)
	  ),
	  begin_(members_.get()), end_(begin_ + n) {
	auto i = members;
	for (auto p = begin_; p != end_; ++i,++p) {
	  std::allocator_traits<Allocator>::construct(*this, p, *i);
	}
      }

      /** @brief Create a list from the range bounded by [startOfMembers,
       *         endOfMembers)
       */
      template <typename Iterator,
		typename Enabler =
		    typename std::enable_if<!std::is_integral<Iterator>::value,
		                            int>::type>
      ImmutableList(Iterator startOfMembers, Iterator endOfMembers,
		    const Allocator& allocator = Allocator(),
		    Enabler = 0):
	ImmutableList(std::distance(startOfMembers, endOfMembers),
			startOfMembers, allocator) {
      }

      /** @brief Create a list from the given initializer list */
      ImmutableList(const std::initializer_list<Item>& members,
		    const Allocator& allocator = Allocator()):
	  ImmutableList(members.size(), members.begin(), allocator) {
      }

      /** @brief Create a copy of the given list */
      ImmutableList(const ImmutableList<Item, Allocator>& other):
	  Allocator(other.allocator()), members_(other.members_),
	  begin_(other.begin_), end_(other.end_) {
      }

      /** @brief Create a copy of the given list, using the given allocator */
      ImmutableList(const ImmutableList<Item, Allocator>& other,
		    const Allocator& allocator):
	  Allocator(allocator), members_(other.members_),
	  begin_(other.begin_), end_(other.end_) {
      }

      /** @brief The list's allocator */
      const Allocator& allocator() const {
	return static_cast<const Allocator&>(*this);
      }

      /** @brief Same as allocator().  Required by the STL */
      const Allocator& get_allocator() const { return allocator(); }

      bool empty() const { return begin_ == end_; }
      size_t size() const { return end_ - begin_; }
      size_t maxSize() const { return size_t(-1); }
      ConstIterator begin() const { return ConstIterator(begin_); }
      ConstIterator cbegin() const { return ConstIterator(begin_); }
      ConstIterator end() const { return ConstIterator(end_); }
      ConstIterator cend() const { return ConstIterator(end_); }
      
      const Item& front() const noexcept { return begin_[0]; }
      const Item& back() const noexcept { return end_[-1]; }
      const Item& at(size_t n) const {
	const Item* p = begin_ + n;
	if (p < end_) {
	  return *p;
	} else {
	  std::ostringstream msg;
	  msg << "Index " << n << " is out-of-range in ImmutableList of size "
	      << size();
	  throw std::range_error(msg.str());
	}
      }

      /** @brief Create a list that is a contiguous subset of this one */
      ImmutableList<Item, Allocator> sublist(size_t start, size_t end) const {
	if (start > size()) {
	  std::ostringstream msg;
	  msg << "Sublist start (" << start
	      << ") is past the end of an ImmutableList of length " << size();
	  throw pistis::exceptions::OutOfRangeError(msg.str(), PISTIS_EX_HERE);
	}
	if (end > size()) {
	  std::ostringstream msg;
	  msg << "Sublist end (" << end
	      << ") is past the end of an ImmutableList of length " << size();
	  throw pistis::exceptions::OutOfRangeError(msg.str(), PISTIS_EX_HERE);
	}
	if (end < start) {
	  std::ostringstream msg;
	  msg << "Sublist end (" << end << ") is before the start ("
	      << start << ")";
	  throw pistis::exceptions::IllegalValueError(msg.str(),
						      PISTIS_EX_HERE);
	}
	return ImmutableList<Item, Allocator>(members_, begin_ + start,
					      begin_ + end, allocator());
      }

      ImmutableList<Item, Allocator> sublist(size_t start) const {
	return this->sublist(start, this->size());
      }
      
      ImmutableList<Item, Allocator> butLast(size_t n) const {
	if (n > size()) {
	  throw pistis::exceptions::IllegalValueError("\"n\" is out of range",
						      PISTIS_EX_HERE);
	}
	return sublist(0, size() - n);
      }
      
      template <typename OtherAllocator>
      ImmutableList<Item, Allocator> concat(
	  const ImmutableList<Item, OtherAllocator>& l
      ) const {
	return ImmutableList(size() + l.size(), JoinListsIterator(*this, l),
			     allocator());
      }

      ImmutableList<Item, Allocator> add(const Item& x) const {
	return ImmutableList(size() + 1, AddItemAtEndIterator(*this, x),
			     allocator());
      }

      ImmutableList<Item, Allocator> insert(size_t pos, const Item& x) const {
	return ImmutableList(size() + 1,
			     AddItemAtPositionIterator(*this, x, pos),
			     allocator());
      }

      ImmutableList<Item, Allocator> remove(size_t n) const {
	return ImmutableList(size() - 1, SkipItemAtPositionIterator(*this, n),
			     allocator());
      }

      ImmutableList<Item, Allocator> replace(ConstIterator p,
					     const Item& x) const {
	return ImmutableList(size(), ReplaceItemAtPositionIterator(*this, x, p),
			     allocator());
      }

      ImmutableList<Item, Allocator> replace(size_t n, const Item& x) const {
	return ImmutableList(size(),
			     ReplaceItemAtPositionIterator(*this, x,
							   begin() + n),
			     allocator());
      }
      
      template <typename Function,
		typename Result = decltype((*(Function*)0)(*(Item*)0)),
	        typename ResultAllocator =
	            typename std::allocator_traits<Allocator>
		                ::template rebind_alloc<Result> >
      ImmutableList<Result, ResultAllocator> map(Function f) const {
	return ImmutableList<Result, ResultAllocator>(
	    size(),
	    makeMappingIterator(makeStlRangeIterator(begin(), end()), f),
	    ResultAllocator(allocator())
	);
      }

      template <typename ReducingFunction, typename InitialResult,
		typename Result =
		    decltype((*(ReducingFunction*)0)(
			*(InitialResult*)0, *(Item*)0
		    ))
	       >
      Result reduce(ReducingFunction f, InitialResult initial) const {
	Result result(initial);
	for (auto i = begin(); i != end(); ++i) {
	  result = f(result, *i);
	}
	return result;
      }

      template <typename ReducingFunction>
      auto reduce(ReducingFunction f) ->
	  decltype(f(*(Item*)0, *(Item*)0)) const {
	if (size() < 2) {
	  throw pistis::exceptions::IllegalStateError(
	      "List has fewer than two items", PISTIS_EX_HERE
	  );
	} else {
	  auto result(f(begin_[0], begin_[1]));
	  for (auto p = begin_ + 2; p != end_; ++p) {
	    result = f(result, *p);
	  }
	  return result;
	}
      }
      
      template <typename OtherItem, typename OtherAllocator>
      bool operator==(
	  const ImmutableList<OtherItem, OtherAllocator>& other
      ) const {
	return (size() == other.size()) &&
	       std::equal(begin_, end_, other.begin_);
      }
      
      template <typename OtherItem, typename OtherAllocator>
      bool operator!=(
	  const ImmutableList<OtherItem, OtherAllocator>& other
      ) const {
	return (size() != other.size()) ||
	       !std::equal(begin_, end_, other.begin_);
      }
      
      const Item& operator[](size_t n) const { return begin_[n]; }

    private:
      std::shared_ptr<Item> members_;
      Item* begin_;
      Item* end_;

      /** @brief Create a list from the given internal state */
      ImmutableList(const std::shared_ptr<Item> members, Item* begin,
		    Item* end, const Allocator& allocator):
	  Allocator(allocator), members_(members), begin_(begin), end_(end) {
      }

      /** @brief Destroy all the list members and release the memory
       *         allocated for the list members
       */
      static void cleanUp_(Allocator& allocator, Item* begin, Item* end) {
	for (Item *p = begin; p != end; ++p) {
	  std::allocator_traits<Allocator>::destroy(allocator, p);
	}
	std::allocator_traits<Allocator>::deallocate(allocator, begin,
						     end - begin);
      }

      class Cleaner : public Allocator {
      public:
	Cleaner(const Allocator& allocator, size_t n):
	    Allocator(allocator), n_(n) {
	}

	void operator()(Item* p) { cleanUp_(*this, p, p + n_); }

      private:
	size_t n_;
      };
      
      class OneItemIterator {
      public:
	OneItemIterator(const ImmutableList& source, const Item& item):
	    src_(source), item_(item), p_(source.begin()) {
	}

	const Item& operator*() const { return *p_; }
	OneItemIterator& operator++() { ++p_; return *this; }

      protected:
	const ImmutableList& src_;
	const Item& item_;
	ImmutableList::ConstIterator p_;
      };
      
      class AddItemAtEndIterator : public OneItemIterator {
      public:
	AddItemAtEndIterator(const ImmutableList& source, const Item& item):
	    OneItemIterator(source, item) {
	}

	const Item& operator*() const {
	  return (this->p_ == this->src_.end()) ? this->item_ : *(this->p_);
	}
      };
      
      class AddItemAtPositionIterator : public OneItemIterator {
      public:
	AddItemAtPositionIterator(const ImmutableList& source, const Item& item,
				  size_t position):
	    OneItemIterator(source, item), cnt_(position) {
	}

	const Item& operator*() const {
	  return cnt_ ? *(this->p_) : (this->item_);
	}
	
	AddItemAtPositionIterator& operator++() {
	  if (cnt_) {
	    ++(this->p_);
	  }
	  --cnt_;
	}
	
      private:
	size_t cnt_;
      };

      class SkipItemAtPositionIterator {
      public:
	SkipItemAtPositionIterator(const ImmutableList& source,
				   size_t position):
	    src_(source),
	    p_(position ? source.begin() : source.begin() + 1),
	    skip_(p_ + position) {
	}

	const Item& operator*() const { return *p_; }
	
	SkipItemAtPositionIterator& operator++() {
	  ++p_;
	  if (p_ == skip_) {
	    ++p_;
	  }
	  return *this;
	}

      private:
	const ImmutableList& src_;
	ImmutableList::ConstIterator p_;
	ImmutableList::ConstIterator skip_;
      };

      class ReplaceItemAtPositionIterator : public OneItemIterator {
      public:
	ReplaceItemAtPositionIterator(
	    const ImmutableList& source,
	    const Item& item,
	    const ImmutableList::ConstIterator position
	):
	    OneItemIterator(source, item), replace_(position) {
	}

	const Item& operator*() const {
	  return (this->p_ == replace_) ? this->item_ : *(this->p_);
	}

      private:
	ImmutableList::ConstIterator replace_;
      };

      class JoinListsIterator {
      public:
	JoinListsIterator(const ImmutableList& list1,
			  const ImmutableList& list2):
	    list1_(list1), list2_(list2),
	    p_(list1.size() ? list1_.begin() : list2.begin()) {
	}

	const Item& operator*() const { return *p_; }

	JoinListsIterator& operator++() {
	  ++p_;
	  if (p_ == list1_.end()) {
	    p_ = list2_.begin();
	  }
	  return *this;
	}
	
      private:
	const ImmutableList& list1_;
	const ImmutableList& list2_;
	ImmutableList::ConstIterator p_;
      };
    };

  }
}
    
#endif
