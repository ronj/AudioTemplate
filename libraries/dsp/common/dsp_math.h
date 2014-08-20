#ifndef DSP_MATH_H
#define DSP_MATH_H

#include <algorithm>
#include <cmath>
#include <limits>

#include <common/config.h>

template <typename T>
CFG_CONSTEXPR T pi()
{
  static_assert(std::is_floating_point<T>::value, "PI is only defined for floating point values.");
  return std::atan(T(1.0)) * T(4.0);
}

template <typename T>
CFG_CONSTEXPR T square(T x)
{
  return x * x;
}

template <typename T>
CFG_CONSTEXPR T sinc(T x)
{
  return x == T(0) ? T(1.0) : std::sin(x * pi<T>()) / x * pi<T>();
}

template <typename T>
CFG_CONSTEXPR T sinh(T x)
{
  return (std::exp(x) - std::exp(-x)) / 2;
}

template <typename T>
CFG_CONSTEXPR T clamp(T value, T min, T max)
{
  return std::max(min, std::min(value, max));
}

template <typename T>
T map(T value, T inputMin, T inputMax, T outputMin, T outputMax, bool clamp = false)
{
  if (std::abs(inputMin - inputMax) < std::numeric_limits<float>::epsilon())
  {
    //std::cout << "ofMap(): avoiding possible divide by zero, check inputMin and inputMax: " << inputMin << " " << inputMax << std::endl;
    return outputMin;
  }
  else
  {
    float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

    if (clamp)
    {
      if(outputMax < outputMin)
      {
        if( outVal < outputMax )outVal = outputMax;
        else if( outVal > outputMin )outVal = outputMin;
      }
      else
      {
        if( outVal > outputMax )outVal = outputMax;
        else if( outVal < outputMin )outVal = outputMin;
      }
    }

    return outVal;
  }
}

namespace taylor {

// xx: x^2
//  n: number of taylor series terms
//  k: current term
template <typename T>
CFG_CONSTEXPR T sin(T xx, unsigned int n, unsigned int k)
{
  static_assert(std::is_floating_point<T>::value, "");
  return k == n ? 1 : 1 - sin(xx, n, k + 1) * xx / (2 * k * (2 * k + 1));
}

// sin(x) using n taylor series terms
template <typename T>
CFG_CONSTEXPR T sin(T x, unsigned int n = 10)
{
  static_assert(std::is_floating_point<T>::value, "");
  return x * sin(x * x, n, 1);
}

} //! namespace taylor

#if CFG_HAS_STRING_LITERALS

CFG_CONSTEXPR long double operator "" _pi(long double x)
{ return x * pi<long double>(); }

CFG_CONSTEXPR long double operator "" _pi(unsigned long long int x)
{ return x * pi<long double>(); }

#endif //! CFG_HAS_STRING_LITERALS

#endif // DSP_MATH_H
