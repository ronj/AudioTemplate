#include "KissFFT.h"

KissFft::KissFft(unsigned int aSignalSize, fftWindowType windowType)
  : Fft(aSignalSize, windowType)
  , iForwardConfig(kiss_fftr_alloc(aSignalSize, 0, nullptr, nullptr))
  , iInverseConfig(kiss_fftr_alloc(aSignalSize, 1, nullptr, nullptr))
  , iWindowedSignal(aSignalSize)
  , cx_out(binSize)
  , cx_in(binSize)
{
}

KissFft::~KissFft()
{
  kiss_fftr_free(iInverseConfig);
  kiss_fftr_free(iForwardConfig);
}

void KissFft::forward()
{
  std::copy(signal, signal + signalSize, iWindowedSignal.begin());
  runWindow(&iWindowedSignal[0]);
  kiss_fftr(iForwardConfig, &iWindowedSignal[0], reinterpret_cast<kiss_fft_cpx*>(&cx_out[0]));

  for (int i = 0; i < binSize; i++)
  {
    real[i] = std::real(cx_out[i]);
    imag[i] = std::imag(cx_out[i]);
  }

  cartesianUpdated = true;
}

void KissFft::inverse()
{
  for (int i = 0; i < binSize; i++)
  {
    cx_in[i].real(real[i]);
    cx_in[i].imag(imag[i]);
  }

  kiss_fftri(iInverseConfig, reinterpret_cast<kiss_fft_cpx*>(&cx_in[0]), signal);
  runInverseWindow(signal);

  signalUpdated = true;
}
