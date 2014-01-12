#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include <cmath>

static const double PI     = std::atan(1.0f) * 4.0f;
static const double TWO_PI = std::atan(1.0f) * 8.0f;

namespace WaveformType {

template <typename T>
struct Sine
{
  constexpr T operator()(T aStep) { return std::sin(TWO_PI * aStep); }
};

template <typename T>
struct Square
{
  constexpr T operator()(T aStep) { return aStep < 0.5 ? 1 : -1; }
};

template <typename T>
struct Saw
{
  constexpr T operator()(T aStep) { return 2 * (aStep - std::round(aStep)); }
};

template <typename T>
struct Triangle
{
  constexpr T operator()(T aStep) { return 1 - 4 * std::abs(std::round(aStep) - aStep); }
};

} //! namespace WaveformType

#endif // WAVEFORMS_H
