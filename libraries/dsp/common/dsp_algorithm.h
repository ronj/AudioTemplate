#ifndef DSP_ALGORITHM_H
#define DSP_ALGORITHM_H

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>
#include <type_traits>

template <typename I>
using iter_value_type = typename std::iterator_traits<I>::value_type;

template <typename I>
inline iter_value_type<I> mean(I begin, I end)
{
  static_assert(std::is_default_constructible<iter_value_type<I>>::value, "");
  return std::accumulate(begin, end, iter_value_type<I>())
    / static_cast<iter_value_type<I>>(std::distance(begin, end));
}

template <typename I>
inline iter_value_type<I> max_amplitude(I begin, I end)
{
  static_assert(std::is_default_constructible<iter_value_type<I>>::value, "");
  return std::accumulate(begin, end, iter_value_type<I>(),
    [](iter_value_type<I> current, iter_value_type<I> next) {
      return std::max(current, std::abs(next));
    });
}

// inner product: sum of squares
// divided by number: mean
// sqrt: root
template <typename I>
inline iter_value_type<I> rms(I begin, I end)
{
  static_assert(std::is_default_constructible<iter_value_type<I>>::value, "");
  return std::sqrt(std::inner_product(begin, end, begin, iter_value_type<I>())
    / static_cast<iter_value_type<I>>(std::distance(begin, end)));
}

template <typename I>
inline iter_value_type<I> std_dev(I begin, I end)
{
  static_assert(std::is_default_constructible<iter_value_type<I>>::value, "");
  // remove dc offset
  std::vector<iter_value_type<I>> zero_mean(begin, end);
  std::transform(zero_mean.begin(), zero_mean.end(), zero_mean.begin(), std::bind2nd(std::minus<iter_value_type<I>>(), mean(begin, end)));

  // use rms on corrected signal
  return rms(zero_mean.begin(), zero_mean.end());
}

#endif // DSP_ALGORITHM_H
