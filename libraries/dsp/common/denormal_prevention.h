#ifndef DENORMAL_PREVENTION_H
#define DENORMAL_PREVENTION_H

#include <cmath>
#include <limits>
#include <type_traits>

template <typename> struct DenormalConstants;
template <> struct DenormalConstants<float>       { static constexpr float       value = 1e-18f; };
template <> struct DenormalConstants<double>      { static constexpr double      value = 1e-30;  };
template <> struct DenormalConstants<long double> { static constexpr long double value = 1e-30L; };

template <typename T> struct None
{
	inline T prevent_denormals(T in) const { return in; }
};

template <typename T> struct DirectCurrent
{
	static_assert(std::is_floating_point<T>::value, "Denormal prevention can only be done on floating point values.");

	inline T prevent_denormals(T in) const { return in + DenormalConstants<T>::value; }
};

template <typename T> struct AlternatingCurrent
{
	static_assert(std::is_floating_point<T>::value, "Denormal prevention can only be done on floating point values.");

	inline T prevent_denormals(T in) const
	{
		m_denormal_small_value = -m_denormal_small_value;
		return in + m_denormal_small_value;
	}

private:
	mutable T m_denormal_small_value = DenormalConstants<T>::value;
};

template <typename T> struct Quantization
{
	static_assert(std::is_floating_point<T>::value, "Denormal prevention can only be done on floating point values.");

	inline T prevent_denormals(T in) const
	{
		T out = in;

		out += DenormalConstants<T>::value;
		out -= DenormalConstants<T>::value;

		return out;
	}
};

template <typename T>
struct SetToZero
{
	inline T prevent_denormals(T in) const
	{
		if (std::abs(in) < std::numeric_limits<T>::min())
		{
			return 0;
		}

		return in;
	}
};

#ifdef SSE_SUPPORT

#include <xmmintrin.h>

template <typename T>
struct FlushToZero
{
	static_assert(std::is_floating_point<T>::value, "(Hardware) denormal prevention only makes sense on floating point values.");

	FlushToZero() { _mm_setcsr(_mm_getcsr() | 0x8000); }
	~FlushToZero() { _mm_setcsr(_mm_getcsr() & ~0x8000); }

	inline T prevent_denormals(T in) const { return in; }
};

#ifdef SSE3_SUPPORT

template <typename T>
struct DenormalsAreZero
{
	static_assert(std::is_floating_point<T>::value, "(Hardware) denormal prevention only makes sense on floating point values.");

	DenormalsAreZero() { _mm_setcsr(_mm_getcsr() | 0x0040); }
	~DenormalsAreZero() { _mm_setcsr(_mm_getcsr() & ~0x0040); }

	inline T prevent_denormals(T in) const { return in; }
};

#endif // SSE3_SUPPORT
#endif // SSE_SUPPORT

#endif // DENORMAL_PREVENTION_H
