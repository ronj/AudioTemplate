#ifndef MPCDEC_INFO_H
#define MPCDEC_INFO_H

#include "audio_info.h"

#include <mpc/streaminfo.h>

class MPCDECInfo : public IAudioInfo
{
public:
  unsigned int channels() const;
  unsigned int bitrate() const;
  unsigned int samplerate() const;

  audio_duration duration() const;

  std::string toString() const;

  mpc_streaminfo* nativeHandle();

private:
  mutable mpc_streaminfo iInfo;
};

#endif // MPCDEC_INFO_H
