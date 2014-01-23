#include "FFT.h"

#include "common.h"

#include "KissFFT.h"

#ifdef ALLOW_FFTW
  #include "Fftw.h"
#endif

#include <stdexcept>

std::shared_ptr<Fft> Fft::create(unsigned int aSignalSize, fftWindowType windowType, fftImplementation implementation)
{
  if(implementation == OF_FFT_BASIC) {
    return std::make_shared<KissFft>(aSignalSize, windowType);
  } else if(implementation == OF_FFT_FFTW) {
    #ifdef ALLOW_FFTW
      return std::make_shared<FFTW>(aSignalSize, windowType);
    #else
      throw std::logic_error("FFTW support not enabled");
    #endif
  }
}

Fft::Fft(unsigned int aSignalSize, fftWindowType windowType)
  : signalSize(aSignalSize)
  , binSize((signalSize / 2) + 1)
  , signalNormalized(true)
{
  signal = new float[signalSize];

  cartesianUpdated = true;
  cartesianNormalized = true;
  real = new float[binSize];
  imag = new float[binSize];

  polarUpdated = true;
  polarNormalized = true;
  amplitude = new float[binSize];
  phase = new float[binSize];

  clear();

  window = new float[signalSize];
  inverseWindow = new float[signalSize];
  setWindowType(windowType);
}

int Fft::getBinSize() {
  return binSize;
}

int Fft::getSignalSize() {
  return signalSize;
}

void Fft::setWindowType(fftWindowType windowType) {
  this->windowType = windowType;
  if(windowType == OF_FFT_WINDOW_RECTANGULAR) {
    for(int i = 0; i < signalSize; i++)
      window[i] = 1; // only used for windowSum
  } else if(windowType == OF_FFT_WINDOW_BARTLETT) {
    int half = signalSize / 2;
    for (int i = 0; i < half; i++) {
      window[i] = ((float) i / half);
      window[i + half] = (1 - ((float) i / half));
    }
  } else if(windowType == OF_FFT_WINDOW_HANN) {
    for(int i = 0; i < signalSize; i++)
      window[i] = .5 * (1 - cos((TWO_PI * i) / (signalSize - 1)));
  } else if(windowType == OF_FFT_WINDOW_HAMMING) {
    for(int i = 0; i < signalSize; i++)
      window[i] = .54 - .46 * cos((TWO_PI * i) / (signalSize - 1));
  } else if(windowType == OF_FFT_WINDOW_SINE) {
    for(int i = 0; i < signalSize; i++)
      window[i] = sin((PI * i) / (signalSize - 1));
  }

  windowSum = 0;
  for(int i = 0; i < signalSize; i++)
    windowSum += window[i];

  for(int i = 0; i < signalSize; i++)
    inverseWindow[i] = 1. / window[i];
}

Fft::~Fft() {
  delete [] signal;
  delete [] real;
  delete [] imag;
  delete [] amplitude;
  delete [] phase;
  delete [] window;
  delete [] inverseWindow;
}

void Fft::clear() {
  memset(signal, 0, sizeof(float) * signalSize);
  memset(real, 0, sizeof(float) * binSize);
  memset(imag, 0, sizeof(float) * binSize);
  memset(amplitude, 0, sizeof(float) * binSize);
  memset(phase, 0, sizeof(float) * binSize);
}

void Fft::copySignal(const float* signal) {
  memcpy(this->signal, signal, sizeof(float) * signalSize);
}

void Fft::copyReal(float* real) {
  memcpy(this->real, real, sizeof(float) * binSize);
}

void Fft::copyImaginary(float* imag) {
  if(imag == NULL)
    memset(this->imag, 0, sizeof(float) * binSize);
  else
    memcpy(this->imag, imag, sizeof(float) * binSize);
}

void Fft::copyAmplitude(float* amplitude) {
  memcpy(this->amplitude, amplitude, sizeof(float) * binSize);
}

void Fft::copyPhase(float* phase) {
  if(phase == NULL)
    memset(this->phase, 0, sizeof(float) * binSize);
  else
    memcpy(this->phase, phase, sizeof(float) * binSize);
}

void Fft::prepareSignal() {
  if(!signalUpdated)
    updateSignal();
  if(!signalNormalized)
    normalizeSignal();
}

void Fft::updateSignal() {
  prepareCartesian();
  inverse();
  signalUpdated = true;
  signalNormalized = false;
}

void Fft::normalizeSignal() {
  float normalizer = (float) windowSum / (2 * signalSize);
  for (int i = 0; i < signalSize; i++)
    signal[i] *= normalizer;
  signalNormalized = true;
}

float* Fft::getSignal() {
  prepareSignal();
  return signal;
}

void Fft::clampSignal() {
  prepareSignal();
  for(int i = 0; i < signalSize; i++) {
    if(signal[i] > 1)
      signal[i] = 1;
    else if(signal[i] < -1)
      signal[i] = -1;
  }
}

void Fft::prepareCartesian() {
  if(!cartesianUpdated) {
    if(!polarUpdated)
      forward();
    else
      updateCartesian();
  }
  if(!cartesianNormalized)
    normalizeCartesian();
}

float* Fft::getReal() {
  prepareCartesian();
  return real;
}

float* Fft::getImaginary() {
  prepareCartesian();
  return imag;
}

void Fft::preparePolar() {
  if(!polarUpdated)
    updatePolar();
  if(!polarNormalized)
    normalizePolar();
}

float* Fft::getAmplitude() {
  preparePolar();
  return amplitude;
}

float* Fft::getPhase() {
  preparePolar();
  return phase;
}

float Fft::getAmplitudeAtBin(float bin) {
  float* amplitude = getAmplitude();
  int lowBin = clamp((int)std::floor(bin), 0, binSize - 1);
  int highBin = clamp((int)std::ceil(bin), 0, binSize - 1);
  return map<float>(bin, lowBin, highBin, amplitude[lowBin], amplitude[highBin]);
}

float Fft::getBinFromFrequency(float frequency, float sampleRate) {
  return frequency * binSize / (sampleRate / 2);
}

float Fft::getAmplitudeAtFrequency(float frequency, float sampleRate) {
  return getAmplitudeAtBin(getBinFromFrequency(frequency, sampleRate));
}

void Fft::updateCartesian() {
  for(int i = 0; i < binSize; i++) {
    real[i] = cosf(phase[i]) * amplitude[i];
    imag[i] = sinf(phase[i]) * amplitude[i];
  }
  cartesianUpdated = true;
  cartesianNormalized = polarNormalized;
}

void Fft::normalizeCartesian() {
  float normalizer = 2. / windowSum;
  for(int i = 0; i < binSize; i++) {
    real[i] *= normalizer;
    imag[i] *= normalizer;
  }
  cartesianNormalized = true;
}

void Fft::updatePolar() {
  prepareCartesian();
  for(int i = 0; i < binSize; i++) {
    amplitude[i] = cartesianToAmplitude(real[i], imag[i]);
    phase[i] = cartesianToPhase(real[i], imag[i]);
  }
  polarUpdated = true;
  polarNormalized = cartesianNormalized;
}

void Fft::normalizePolar() {
  float normalizer = 2. / windowSum;
  for(int i = 0; i < binSize; i++)
    amplitude[i] *= normalizer;
  polarNormalized = true;
}

void Fft::clearUpdates() {
  cartesianUpdated = false;
  polarUpdated = false;
  cartesianNormalized = false;
  polarNormalized = false;
  signalUpdated = false;
  signalNormalized = false;
}

void Fft::setSignal(const std::vector<float>& signal) {
  setSignal(&signal[0]);
}

void Fft::setSignal(const float* signal) {
  clearUpdates();
  copySignal(signal);
  signalUpdated = true;
  signalNormalized = true;
}

void Fft::setCartesian(float* real, float* imag) {
  clearUpdates();
  copyReal(real);
  copyImaginary(imag);
  cartesianUpdated = true;
  cartesianNormalized = true;
}

void Fft::setPolar(float* amplitude, float* phase) {
  clearUpdates();
  copyAmplitude(amplitude);
  copyPhase(phase);
  polarUpdated = true;
  polarNormalized = true;
}
