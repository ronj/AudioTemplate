#ifndef SAMPLERATE_OBSERVER_H
#define SAMPLERATE_OBSERVER_H

class SamplerateObserver
{
public:
	virtual ~SamplerateObserver() {}
	virtual void samplerateChanged(float aNewRate, float aOldRate) = 0;
};

#endif // SAMPLERATE_OBSERVER_H
