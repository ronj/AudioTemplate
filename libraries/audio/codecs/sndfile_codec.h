#ifndef SNDFILE_CODEC_H
#define SNDFILE_CODEC_H

#include <sndfile.h>

#include "audio_codec.h"
#include "sndfile_info.h"

template <typename T>
class SndFileCodec : public IAudioCodec<T>
{
public:
  using sample_type = T;

public:
  SndFileCodec(const std::string& aFilename)
  {
    iSndFile = sf_open(aFilename.c_str(), SFM_READ, iInfo.nativeHandle());
  }

  virtual ~SndFileCodec()
  {
    sf_close(iSndFile);
  }

  std::size_t decode(sample_type* aSamples, std::size_t aSampleCount)
  {
    return sf_read_float(iSndFile, aSamples, aSampleCount);
  }

  std::size_t encode(const sample_type*, std::size_t)
  {
    throw std::logic_error("Not implemented");
  }

  const IAudioInfo& info() const
  {
    return iInfo;
  }

private:
  SNDFILE*    iSndFile = nullptr;
  SndFileInfo iInfo;
};

#endif // SNDFILE_CODEC_H
