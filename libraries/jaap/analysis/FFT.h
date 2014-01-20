#ifndef FFT_H
#define FFT_H

#include <vector>
#include <cmath>
#include <memory>

enum fftWindowType {
	OF_FFT_WINDOW_RECTANGULAR,
	OF_FFT_WINDOW_BARTLETT,
	OF_FFT_WINDOW_HANN,
	OF_FFT_WINDOW_HAMMING,
	OF_FFT_WINDOW_SINE};

enum fftImplementation {
	OF_FFT_BASIC,
	OF_FFT_FFTW};

class Fft {
public:
	static std::shared_ptr<Fft> create(unsigned int aSignalSize = 512,
		fftWindowType windowType = OF_FFT_WINDOW_HAMMING,
		fftImplementation implementation = OF_FFT_BASIC);

	Fft(unsigned int aSignalSize, fftWindowType windowType);
	virtual ~Fft();

	void setSignal(const std::vector<float>& signal);
	void setSignal(const float* signal);
	void setCartesian(float* real, float* imag = nullptr);
	void setPolar(float* amplitude, float* phase = nullptr);

	int getSignalSize();
	float* getSignal();
	void clampSignal();

	int getBinSize();
	float* getReal();
	float* getImaginary();
	float* getAmplitude();
	float* getPhase();

	float getAmplitudeAtBin(float bin);
	float getBinFromFrequency(float frequency, float sampleRate = 44100);
	float getAmplitudeAtFrequency(float frequency, float sampleRate = 44100);

protected:
	virtual void forward() = 0;
	virtual void inverse() = 0;

	void clear();

	// time domain data and methods
	fftWindowType windowType;
	float windowSum;
	float *window, *inverseWindow;

	void setWindowType(fftWindowType windowType);

	inline void runWindow(float* signal) {
		if(windowType != OF_FFT_WINDOW_RECTANGULAR)
			for(int i = 0; i < signalSize; i++)
				signal[i] *= window[i];
	}

	inline void runInverseWindow(float* signal) {
		if(windowType != OF_FFT_WINDOW_RECTANGULAR)
			for(int i = 0; i < signalSize; i++)
				signal[i] *= inverseWindow[i];
	}

	float *signal;
	bool signalUpdated, signalNormalized;
	void prepareSignal();
	void updateSignal();
	void normalizeSignal();
	void copySignal(const float* signal);

	// frequency domain data and methods
	int signalSize, binSize;

	float *real, *imag;
	bool cartesianUpdated, cartesianNormalized;
	void prepareCartesian();
	void updateCartesian();
	void normalizeCartesian();
	void copyReal(float* real);
	void copyImaginary(float* imag);

	float *amplitude, *phase;
	bool polarUpdated, polarNormalized;
	void preparePolar();
	void updatePolar();
	void normalizePolar();
	void copyAmplitude(float* amplitude);
	void copyPhase(float* phase);

	void clearUpdates();

	inline float cartesianToAmplitude(float x, float y) {
		return std::sqrt(x * x + y * y);
	}

	inline float cartesianToPhase(float x, float y) {
		return std::atan2(y, x);
	}
};


#endif // FFT_H
