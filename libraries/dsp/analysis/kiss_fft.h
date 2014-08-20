#ifndef KISSFFT_H
#define KISSFFT_H

#include "fft.h"

#include "kiss_fftr.h"

#include <vector>
#include <complex>

class KissFft : public Fft
{
public:
  KissFft(unsigned int aSignalSize, fftWindowType windowType);
  virtual ~KissFft();

protected:
  void forward();
  void inverse();

private:
  kiss_fftr_cfg iForwardConfig;
  kiss_fftr_cfg iInverseConfig;
  std::vector<float> iWindowedSignal;
  std::vector<std::complex<float>> cx_out;
  std::vector<std::complex<float>> cx_in;
};

#endif // KISSFFT_H
