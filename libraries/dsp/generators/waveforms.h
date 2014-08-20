#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include <dsp/common/dsp_math.h>

namespace WaveformType {

template <typename T>
struct Sine
{
#if CFG_HAS_STRING_LITERALS
  CFG_CONSTEXPR T operator()(T aStep) { return std::sin(2_pi * aStep); }
#else
  CFG_CONSTEXPR T operator()(T aStep) { return std::sin(2.0 * pi<double>() * aStep); }
#endif
};

template <typename T>
struct Square
{
  CFG_CONSTEXPR T operator()(T aStep) { return aStep < 0.5 ? 1 : -1; }
};

template <typename T>
struct Saw
{
  CFG_CONSTEXPR T operator()(T aStep) { return 2 * (aStep - std::round(aStep)); }
};

template <typename T>
struct Triangle
{
  CFG_CONSTEXPR T operator()(T aStep) { return 1 - 4 * std::abs(std::round(aStep) - aStep); }
};

} // namespace WaveformType

#endif // WAVEFORMS_H
