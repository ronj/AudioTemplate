#include "samplerate_aware.h"
#include "samplerate_control.h"

SamplerateAware::SamplerateAware(SamplerateControl& aSamplerateControl)
  : iSamplerateControl(aSamplerateControl)
{
  iSamplerateControl.addSamplerateObserver(*this);
}

SamplerateAware::~SamplerateAware()
{
  iSamplerateControl.removeSamplerateObserver(*this);
}

float SamplerateAware::getSamplerate() const
{
  return iSamplerateControl.getSamplerate();
}
