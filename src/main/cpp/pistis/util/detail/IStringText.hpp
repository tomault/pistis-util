#ifndef __PISTIS__UTIL__DETAIL__ISTRINGTEXT_HPP__
#define __PISTIS__UTIL__DETAIL__ISTRINGTEXT_HPP__

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <assert.h>
#include <stdint.h>

namespace pistis {
  namespace util {
    namespace detail {

      template <typename Char>
      struct IStringText {
	std::atomic<uint32_t> refCnt;
	uint32_t size;
	Char text[1];

	IStringText(uint32_t s): refCnt(0), size(s) { }

	template <typename Iterator>
	IStringText(uint32_t s, const Iterator& t): refCnt(0), size(s) {
	  std::copy_n(t, s, text);
	}

	size_t allocationSize() const { return computeAllocationSize(size); }
      
	static size_t computeAllocationSize(size_t n) {
	  return offsetof(IStringText, text) + n * sizeof(Char);
	}
      
	static IStringText* addRef(IStringText* t) {
	  if (t) {
	    ++(t->refCnt);
	  }
	  return t;
	}
      
	static uint32_t removeRef(IStringText* t) {
	  assert(t->refCnt > 0);
	  return --(t->refCnt);
	}      
      };

      template <typename Char, typename Allocator>
      class IStringTextPtr : Allocator {
      public:
	IStringTextPtr(const Allocator& allocator = Allocator()):
	    Allocator(allocator), p_(nullptr) {
	}
      
	IStringTextPtr(IStringText<Char>* p, const Allocator& allocator):
	    Allocator(allocator), p_(IStringText<Char>::addRef(p)) {
	}
      
	IStringTextPtr(const IStringTextPtr& other):
	    Allocator(other.allocator()),
	    p_(IStringText<Char>::addRef(other.get())) {
	}
	IStringTextPtr(IStringTextPtr&& other):
	    Allocator(std::move(other)), p_(other.p_) {
	  other.p_ = nullptr;
	}
	~IStringTextPtr() { release_(); }

	const Allocator& allocator() const {
	  return static_cast<const Allocator&>(*this);
	}
      
	Allocator& allocator() { return static_cast<Allocator&>(*this); }
      
	IStringText<Char>* get() const { return p_; }

	void reset(IStringText<Char>* p = nullptr) {
	  release_();
	  p_ = IStringText<Char>::addRef(p);
	}

	IStringTextPtr& operator=(const IStringTextPtr& other) {
	  if (this != &other) {
	    release_();
	    Allocator::operator=(other);
	    p_ = IStringText<Char>::addRef(other.get());
	  }
	  return *this;
	}

	IStringTextPtr& operator=(IStringTextPtr&& other) {
	  if (this != &other) {
	    release_();
	    Allocator::operator=(std::move(other));
	    p_ = other.p_;
	    other.p_ = nullptr;
	  }
	  return *this;
	}

	operator bool() const { return (bool)p_; }
	IStringText<Char>& operator*() const { return *p_; }
	IStringText<Char>* operator->() const { return p_; }
	bool operator==(const IStringTextPtr& other) const {
	  return p_ == other.p_;
	}
	bool operator!=(const IStringTextPtr& other) const {
	  return p_ != other.p_;
	}

	static IStringTextPtr create(size_t n, const Allocator& allocator) {
	  Allocator newAllocator(allocator);
	  return create(n, newAllocator);
	}
	
	static IStringTextPtr create(size_t n, Allocator& allocator) {
	  const size_t textSize = IStringText<Char>::computeAllocationSize(n);
	  IStringText<Char>* newText =
	       (IStringText<Char>*)allocator.allocate(textSize);
	  try {
	    new(newText) IStringText<Char>(n);
	    try {
	      return IStringTextPtr(newText, allocator);
	    } catch(...) {
	      newText->~IStringText<Char>();
	      throw;
	    }
	  } catch(...) {
	    allocator.deallocate((uint8_t*)newText, textSize);
	    throw;
	  }
	}

	template <typename Iterator>
	static IStringTextPtr create(size_t n, const Iterator& t,
				     Allocator& allocator) {
	  const size_t textSize = IStringText<Char>::computeAllocationSize(n);
	  IStringText<Char>* newText =
	       (IStringText<Char>*)allocator.allocate(textSize);
	  try {
	    new(newText) IStringText<Char>(n, t);
	    try {
	      return IStringTextPtr(newText, allocator);
	    } catch(...) {
	      newText->~IStringText<Char>();
	      throw;
	    }
	  } catch(...) {
	    allocator.deallocate((uint8_t*)newText, textSize);
	    throw;
	  }
	}

      private:
	IStringText<Char>* p_;

	void release_() {
	  if (p_ && !IStringText<Char>::removeRef(p_)) {
	    const size_t allocationSize = p_->allocationSize();
	    p_->~IStringText<Char>();
	    this->deallocate((uint8_t*)p_, allocationSize);
	  }
	}
      };

    }
  }
}
#endif
