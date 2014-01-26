#include "yaffut.h"

#include "dsp/analysis/fft.h"
#include "dsp/dsp_math.h"

#include "fft_test_vectors.h"

#include <iomanip>

std::vector<float> createSignal(std::size_t aSize, float aFrequency, float aSamplerate = 44100.0f)
{
  std::vector<float> signal(aSize);
  double theta = 0;
  float deltaTheta = TWO_PI * (aFrequency / aSamplerate);

  for (std::size_t i = 0; i < signal.size(); i++)
  {
    signal[i] = std::sin(theta);
    theta += deltaTheta;
    if (theta > TWO_PI) theta = theta - TWO_PI;
  }

  return signal;
}

FUNC(should_create_fft_object)
{
  const unsigned int signalSize = 512;
  std::shared_ptr<Fft> fft = Fft::create(signalSize);

  EQUAL(fft->getBinSize(), (signalSize / 2) + 1);
}

FUNC(should_calculate_fft_of_signal)
{
  const unsigned int signalSize = 512;
  const float frequency = 440.0f;

  std::vector<float> sine440 = createSignal(signalSize, frequency);
  std::shared_ptr<Fft> fft = Fft::create(signalSize);

  (*fft)(sine440.begin(), sine440.end());

  EQUAL(fft->getBinFromFrequency(frequency), 5.12834453582764f);
  EQUAL(fft->getAmplitudeAtFrequency(frequency), 0.928644418716431f);

  float* amplitude = fft->getAmplitude();
  for (std::size_t i = 0; i < fft->getBinSize(); ++i)
  {
    EQUAL(amplitude[i], fftSine440[i]);
  }
}
