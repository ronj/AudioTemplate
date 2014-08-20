#include "samplerate_control.h"
#include "samplerate_observer.h"

#include <algorithm>

SamplerateControl::SamplerateControl(float aSamplerate)
  : iSamplerate(aSamplerate)
{
}

void SamplerateControl::setSamplerate(float aSamplerate)
{
  float oldRate = iSamplerate;
  iSamplerate = aSamplerate;
  notifySamplerateObservers(iSamplerate, oldRate);
}

float SamplerateControl::getSamplerate() const
{
  return iSamplerate;
}

void SamplerateControl::addSamplerateObserver(SamplerateObserver& aObserver)
{
  if (std::find(iObservers.begin(), iObservers.end(), &aObserver) == iObservers.end())
  {
    iObservers.push_back(&aObserver);
  }
}

void SamplerateControl::removeSamplerateObserver(SamplerateObserver& aObserver)
{
  iObservers.erase(std::remove(iObservers.begin(), iObservers.end(), &aObserver), iObservers.end());
}

void SamplerateControl::notifySamplerateObservers(float aNewRate, float aOldRate)
{
  for (auto observer : iObservers)
  {
    observer->samplerateChanged(aNewRate, aOldRate);
  }
}
