#include <algorithm>
#include <iostream>

#include "analysis/FFT.h"

static const double PI = std::atan(1.0f) * 4.0f;

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  std::shared_ptr<Fft> fft = Fft::create();
  std::vector<float> signal(512);
  double theta = 0;
  float deltaTheta = 2.0 * M_PI * (440.0 / 44100.0);

  for (int i = 0; i < signal.size(); i++)
  {
    signal[i] = sin(theta);
    theta += deltaTheta;
    if (theta > 2.0 * M_PI) theta = theta - 2.0 * M_PI;
  }

  fft->setSignal(signal);
  float* amp = fft->getAmplitude();

  for (int i = 0; i < fft->getBinSize(); i++)
  {
    std::cout << amp[i] << ", ";
  }

  std::cout << std::endl;

  std::cout << "Amplitude at 440 Hz: " << fft->getAmplitudeAtFrequency(440, 44100) << std::endl;
}
