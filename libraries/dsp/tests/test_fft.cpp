#include "yaffut.h"

#include <dsp/effects/AudioFFT.h>
#include <dsp/common/dsp_math.h>

#include <algorithm>
#include <iterator>
#include <vector>
#include <cmath>

std::vector<float> createSignal(std::size_t aSize, float aFrequency, float aAmplitude = 1.0f, float aSamplerate = 44100.0f)
{
  std::vector<float> signal(aSize, 0.0f);
  double theta = 0;
  float deltaTheta = 2_pi * (aFrequency / aSamplerate);

  for (std::size_t i = 0; i < aSize; i++)
  {
    signal[i] = std::sin(theta) * aAmplitude;
    theta += deltaTheta;
    if (theta > 2_pi) theta = theta - 2_pi;
  }

  return signal;
}

std::vector<float> univariateKroneckerDelta(std::size_t aSize)
{
  std::vector<float> signal(aSize, 0.0f);
  signal[0] = 1.0f;
  return signal;
}

inline float cartesianToAmplitude(float x, float y)
{
  return std::sqrt(x * x + y * y);
}

inline float cartesianToPhase(float x, float y)
{
  return std::atan2(y, x);
}

struct fft_test {};

TEST(fft_test, should_calculate_fft_and_ifft_of_unit_impulse)
{
  const std::size_t inputSize = 512;
  const std::size_t complexSize = audiofft::AudioFFT::ComplexSize(inputSize);

  audiofft::AudioFFT fft;
  fft.init(inputSize);

  std::vector<float> input = univariateKroneckerDelta(inputSize);

  std::vector<float> re(complexSize, 0.0f);
  std::vector<float> im(complexSize, 0.0f);
  fft.fft(input.data(), re.data(), im.data());

  std::vector<float> amplitude(complexSize, 0.0f);
  for (std::size_t i = 0; i < complexSize; ++i)
  {
    EQUAL(re[i], 1);
    EQUAL(im[i], 0);

    amplitude[i] = cartesianToAmplitude(re[i], im[i]);
    EQUAL(amplitude[i], 1);
  }

  std::vector<float> output(inputSize, 0.0f);
  fft.ifft(output.data(), re.data(), im.data());
  CHECK(input == output);
}

TEST(fft_test, should_test_linearity)
{
  const std::size_t inputSize = 1024;
  const std::size_t complexSize = audiofft::AudioFFT::ComplexSize(inputSize);

  audiofft::AudioFFT fft;
  fft.init(inputSize);

  std::vector<float> signalA = createSignal(inputSize, 440.0f);
  std::vector<float> signalB = createSignal(inputSize, 400.0f);
  std::vector<float> summedSignal(inputSize, 0.0f);
  std::transform(signalA.begin(), signalA.end(), signalB.begin(), summedSignal.begin(), std::plus<float>());

  for (std::size_t i = 0; i < inputSize; ++i)
  {
    EQUAL(signalA[i] + signalB[i], summedSignal[i]);
  }

  std::vector<float> reA(complexSize, 0.0f);
  std::vector<float> imA(complexSize, 0.0f);
  fft.fft(signalA.data(), reA.data(), imA.data());

  std::vector<float> reB(complexSize, 0.0f);
  std::vector<float> imB(complexSize, 0.0f);
  fft.fft(signalB.data(), reB.data(), imB.data());

  std::vector<float> reSum(complexSize, 0.0f);
  std::vector<float> imSum(complexSize, 0.0f);
  fft.fft(summedSignal.data(), reSum.data(), imSum.data());

  for (std::size_t i = 0; i < complexSize; ++i)
  {
    EQUAL(reA[i] + reB[i], reSum[i]);
    EQUAL(imA[i] + imB[i], imSum[i]);
  }
}

TEST(fft_test, should_calculate_fft_of_time_shifted_input)
{
  const std::size_t inputSize = 1024;
  const std::size_t complexSize = audiofft::AudioFFT::ComplexSize(inputSize);
  const std::size_t timeShift = 2;

  audiofft::AudioFFT fft;
  fft.init(inputSize);

  std::vector<float> input = createSignal(inputSize, 1000.0f);
  std::vector<float> timeShiftedInput(input);

  std::rotate(timeShiftedInput.rbegin(), timeShiftedInput.rbegin() + timeShift, timeShiftedInput.rend());

  //CHECK(std::equal(input.begin() + timeShift, input.end(), timeShiftedInput.begin()));
  //CHECK(std::equal(input.begin(), input.begin() + timeShift, timeShiftedInput.begin() + (input.size() - timeShift)));

  std::vector<float> re(complexSize, 0.0f);
  std::vector<float> im(complexSize, 0.0f);
  fft.fft(input.data(), re.data(), im.data());

  std::vector<float> reShift(complexSize, 0.0f);
  std::vector<float> imShift(complexSize, 0.0f);
  fft.fft(timeShiftedInput.data(), reShift.data(), imShift.data());

  std::vector<float> phase(complexSize, 0.0f);
  std::vector<float> timeShiftedPhase(complexSize, 0.0f);

  for (std::size_t i = 0; i < complexSize; ++i)
  {
    phase[i] = cartesianToPhase(re[i], im[i]);
    timeShiftedPhase[i] = cartesianToPhase(reShift[i], imShift[i]);
    std::cout << phase[i] << ", " << timeShiftedPhase[i] << std::endl;
  }
}
