#ifndef __PISTIS__UTIL__ISTRINGBUILDERMANIPULATORS_HPP__
#define __PISTIS__UTIL__ISTRINGBUILDERMANIPULATORS_HPP__

#include <pistis/util/IStringBuilder_.hpp>

namespace pistis {
  namespace util {
    namespace detail {
      
      class ImmutableStringBuilderSetWidthManipulator {
      public:
	ImmutableStringBuilderSetWidthManipulator(uint32_t width): w_(width) { }

	template <typename C, typename T, typename A>
	ImmutableStringBuilder<C, T, A>& operator()(
	    ImmutableStringBuilder<C, T, A>& b
	) const {
	  return b.width(w_);
	}
	
      private:
	uint32_t w_;
      };

      template <typename C, typename T, typename A>
      ImmutableStringBuilder<C, T, A>& operator<<(
	  ImmutableStringBuilder<C, T, A>& b,
	  const ImmutableStringBuilderSetWidthManipulator& m
      ) {
	return m(b);
      }

      template <typename Char>
      class ImmutableStringBuilderSetPadManipulator {
      public:
	ImmutableStringBuilderSetPadManipulator(Char c): c_(c) { }

	template <typename C, typename T, typename A>
	ImmutableStringBuilder<C, T, A>& operator()(
	    ImmutableStringBuilder<C, T, A>& b
	) const {
	  return b.pad(c_);
	}
	
      private:
	Char c_;
      };

      template <typename C, typename T, typename A, typename P>
      ImmutableStringBuilder<C, T, A>& operator<<(
	  ImmutableStringBuilder<C, T, A>& b,
	  const ImmutableStringBuilderSetPadManipulator<P>& m
      ) {
	return m(b);
      }

      class ImmutableStringBuilderSetPrecisionManipulator {
      public:
	ImmutableStringBuilderSetPrecisionManipulator(uint32_t p): p_(p) { }

	template <typename C, typename T, typename A>
	ImmutableStringBuilder<C, T, A>& operator()(
	    ImmutableStringBuilder<C, T, A>& b
	) const {
	  return b.precision(p_);
	}
	
      private:
	uint32_t p_;
      };

      template <typename C, typename T, typename A>
      ImmutableStringBuilder<C, T, A>& operator<<(
	  ImmutableStringBuilder<C, T, A>& b,
	  const ImmutableStringBuilderSetPrecisionManipulator& m
      ) {
	return m(b);
      }

    }

    namespace ism {
      inline auto width(uint32_t n) {
	return detail::ImmutableStringBuilderSetWidthManipulator(n);
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& noWidth(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.noWidth();
      }

      inline auto precision(uint32_t n) {
	return detail::ImmutableStringBuilderSetPrecisionManipulator(n);
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& noPrecision(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.noPrecision();
      }

      template <typename Char>
      inline auto pad(Char c) {
	return detail::ImmutableStringBuilderSetPadManipulator<Char>(c);
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& left(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.left();
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& center(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.center();
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& right(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.right();
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& addSign(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.addSign();
      }
      
      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& noSign(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.noSign();
      }
      
      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& expf(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.exp();
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& expfUpper(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.expUpper();
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& expg(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.expGeneral();
      }
      
      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& expgUpper(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.expGeneralUpper();
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& hex(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.hex();
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& hexUpper(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.hexUpper();
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& oct(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.oct();
      }

      template <typename C, typename T, typename A>
      inline ImmutableStringBuilder<C, T, A>& dec(
	  ImmutableStringBuilder<C, T, A>& b
      ) {
	return b.dec();
      }

    }
  }
}

#endif
