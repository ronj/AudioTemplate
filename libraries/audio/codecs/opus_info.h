#ifndef OPUS_INFO_H
#define OPUS_INFO_H

#include "audio_info.h"

#include <opus/opusfile.h>

class OpusInfo : public IAudioInfo
{
public:
  unsigned int channels() const;
  unsigned int bitrate() const;
  unsigned int samplerate() const;

  audio_duration duration() const;

  std::string toString() const;

  void setNativeHandle(const OpusHead* aHeader);

private:
  const OpusHead* iInfo;
};

#endif // !OPUS_INFO_H
