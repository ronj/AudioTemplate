#ifndef SAMPLERATEAWARE_H
#define SAMPLERATEAWARE_H

#include "SamplerateObserver.h"

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

#endif // SAMPLERATEAWARE_H
