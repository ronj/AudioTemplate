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
#if CFG_HAS_STRING_LITERALS
  float deltaTheta = 2_pi * (440.0 / 44100.0);
#else
  float deltaTheta = 2.0 * pi<double>() * (440.0 / 44100.0);
#endif

  for (int i = 0; i < signal.size(); i++)
  {
    signal[i] = sin(theta);
    theta += deltaTheta;
#if CFG_HAS_STRING_LITERALS
    if (theta > 2_pi) theta = theta - 2_pi;
#else
	if (theta > 2 * pi<double>()) theta = theta - 2.0 * pi<double>();
#endif
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
