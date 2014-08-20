#ifndef SAMPLERATE_AWARE_H
#define SAMPLERATE_AWARE_H

#include <audio/samplerate_observer.h>

class SamplerateControl;

class SamplerateAware
  : public SamplerateObserver
{
public:
  SamplerateAware(SamplerateControl& aSamplerateControl);
  virtual ~SamplerateAware();

  float getSamplerate() const;

private:
  SamplerateControl& iSamplerateControl;
};

#endif // SAMPLERATE_AWARE_H
