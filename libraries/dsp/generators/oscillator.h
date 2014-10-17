#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <vector>

template <typename T>
using return_type = typename std::result_of<T(std::size_t)>::type;

template <typename WaveformType, std::size_t N = 2048>
class Oscillator
{
public:
  Oscillator(double aFrequency, double aSamplerate = 44100.0)
    : iSamplerate(aSamplerate)
  {
    if (sTable.empty())
    {
      sTable.reserve(N);
      WaveformType w;

      for (std::size_t n = 0; n < N; ++n)
      {
        sTable.push_back(w(n * (1.0 / N)));
      }
    }

    setFrequency(aFrequency);
  }

  inline return_type<WaveformType> operator()()
  {
    while (iTime < 0.0)
    {
      iTime += N;
    }
    while (iTime >= N)
    {
      iTime -= N;
    }

    iIndex = static_cast<std::size_t>(iTime);
    double alpha = iTime - iIndex;
    iTime += iRate;

    return sTable[iIndex] * (1 - alpha) + sTable[std::min(iIndex + 1, sTable.size() - 1)] * alpha;
  }

  void setFrequency(double aFrequency)
  {
    iRate = N * aFrequency / iSamplerate;
  }

  void addTime(double aTime)
  {
    iTime += aTime;
  }

  void addPhase(double aPhase)
  {
    iTime += N * aPhase;
  }

  void addPhaseOffset(double aPhaseOffset)
  {
    iTime += (aPhaseOffset - iPhaseOffset) * N;
    iPhaseOffset = aPhaseOffset;
  }

private:
  static std::vector<return_type<WaveformType>> sTable;

private:
  std::size_t  iIndex = 0;
  double       iTime = 0.0;
  double       iRate = 0.0;
  double       iPhaseOffset = 0.0;
  float        iSamplerate;
};

template <typename WaveformType, std::size_t N>
std::vector<return_type<WaveformType>> Oscillator<WaveformType, N>::sTable;

#endif // OSCILLATOR_H
