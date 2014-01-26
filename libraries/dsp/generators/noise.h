#ifndef NOISE_H
#define NOISE_H

#include <random>

template <typename RandomGenerator = std::default_random_engine>
class WhiteNoise
{
public:
  double operator()()
  {
    return iNormalDistribution(iRandom);
  }

private:
  RandomGenerator iRandom;
  std::normal_distribution<> iNormalDistribution = std::normal_distribution<>(0, 0.25);
};

template <typename RandomGenerator = std::default_random_engine>
class BrownNoise
{
public:
  double operator()()
  {
    while (true)
    {
      double r = iNoise();
      iTemp += r;
      if (iTemp < -8.0 || iTemp > 8.0) iTemp -= r;
      else break;
    }

    return iTemp * 0.0625;
  }

private:
  WhiteNoise<RandomGenerator> iNoise;
  double iTemp = 0.0;
};

template <std::size_t HistorySize = 16, typename RandomGenerator = std::default_random_engine>
class PinkNoise
{
public:
  double operator()()
  {
    unsigned long k = ctz(iIndex);
    k = k & (HistorySize - 1);
    double previousr = iHistory[k];

    while (true)
    {
      double r = iNoise();
      iHistory[k] = r;
      r -= previousr;
      iTemp += r;
      if (iTemp < -4.0 || iTemp > 4.0) iTemp -= r;
      else break;
    }

    iIndex++;
    return (iNoise() + iTemp) * 0.125;
  }

private:
  int ctz(int num)
  {
    int i = 0;
    while (((num >> i) & 1) == 0 && i < (int)sizeof(int)) i++;
    return i;
  }

private:
  WhiteNoise<RandomGenerator> iNoise;
  double iTemp = 0.0;
  std::array<float, HistorySize> iHistory;
  unsigned long iIndex = 0;
};

#endif // NOISE_H
