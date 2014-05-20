#include <algorithm>
#include <iostream>

#include <dsp/analysis/fft.h>
#include <dsp/common/dsp_math.h>

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  std::shared_ptr<Fft> fft = Fft::create();
  std::vector<float> signal(512);
  double theta = 0;
  float deltaTheta = 2_pi * (440.0 / 44100.0);

  for (int i = 0; i < signal.size(); i++)
  {
    signal[i] = sin(theta);
    theta += deltaTheta;
    if (theta > 2_pi) theta = theta - 2_pi;
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
