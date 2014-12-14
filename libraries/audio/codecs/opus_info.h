#ifndef OPUS_INFO_H
#define OPUS_INFO_H

#include "audio_info.h"

class OpusHead;
class OggOpusFile;

class OpusInfo : public IAudioInfo
{
public:
  unsigned int channels() const;
  unsigned int bitrate() const;
  unsigned int samplerate() const;

  audio_duration duration() const;

  std::string toString() const;

  void setNativeHandle(const OpusHead* aHeader) const;
  void setNativeFileHandle(const OggOpusFile* aFile);

private:
  mutable const OpusHead* iInfo;
  const OggOpusFile* iFile;
};

#endif // !OPUS_INFO_H
