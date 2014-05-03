#ifndef SNDFILE_INFO_H
#define SNDFILE_INFO_H

#include <sndfile.h>

#include "audio_info.h"

class SndFileInfo : public IAudioInfo
{
public:
  unsigned int channels() const
  {
    return iInfo.channels;
  }

  unsigned int bitrate() const
  {
    return 0;
  }

  unsigned int samplerate() const
  {
    return iInfo.samplerate;
  }

  unsigned int duration() const
  {
    return 0;
  }

  std::string toString() const
  {
    std::stringstream ss;

    ss << "SNDFile";

    return ss.str();
  }

  SF_INFO* nativeHandle()
  {
    return &iInfo;
  }

private:
  SF_INFO iInfo;
};

#endif // SNDFILE_INFO_H
