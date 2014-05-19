#ifndef MPADEC_INFO_H
#define MPADEC_INFO_H

#include "audio_info.h"

#include <mpadec.h>

class MPADECInfo : public IAudioInfo
{
public:
  unsigned int layer() const
  {
    return iInfo.layer;
  }

  unsigned int channels() const
  {
    return iInfo.channels;
  }

  unsigned int bitrate() const
  {
    return iInfo.bitrate;
  }

  unsigned int samplerate() const
  {
    return iInfo.frequency;
  }

  audio_duration duration() const
  {
    return audio_duration(iInfo.duration);
  }

  std::string toString() const
  {
    std::stringstream ss;

    ss << mpegTypeString() << " " << layerToString() << " "
       << channelsToString() << " " << samplerateToString() << " "
       << bitrateToString() << " " << durationToString();

    return ss.str();
  }

  mpadec_info_t* nativeHandle()
  {
    return &iInfo;
  }

private:
  std::string mpegTypeString() const
  {
    if (samplerate() < 16000)
    {
      return "MPEG-2.5";
    }
    else if (samplerate() < 32000)
    {
      return "MPEG-2";
    }

    return "MPEG-1";
  }

  std::string layerToString() const
  {
    switch (layer())
    {
      case 1: return "Layer-I";
      case 2: return "Layer-II";
      case 3: return "Layer-III";
      default: return "Unknown Layer";
    }
  }

private:
  mpadec_info_t iInfo;
};

#endif // MPADEC_INFO_H
