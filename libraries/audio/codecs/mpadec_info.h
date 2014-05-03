#ifndef MPADEC_INFO_H
#define MPADEC_INFO_H

#include <mpadec.h>

#include "audio_info.h"

#include <string>
#include <sstream>

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

  unsigned int duration() const
  {
    return iInfo.duration;
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

  std::string channelsToString() const
  {
    return (channels() > 1 ? "stereo" : "mono");
  }

  std::string bitrateToString() const
  {
    return Stringify(bitrate()) + " Kbps";
  }

  std::string samplerateToString() const
  {
    return Stringify(samplerate()) + " Hz";
  }

  std::string durationToString() const
  {
    return Stringify(duration() / 60) + ":" + Stringify(duration() % 60);
  }

private:
  mpadec_info_t iInfo;
};

#endif // MPADEC_INFO_H
