#ifndef SNDFILE_INFO_H
#define SNDFILE_INFO_H

#include "audio_info.h"

#include <sndfile.h>

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

  audio_duration duration() const
  {
    if ((samplerate() < 1) ||
        (iInfo.frames / samplerate()) > 0x7FFFFFFF)
    {
      return std::chrono::duration<float>::zero();
    }

    return audio_duration((1.0f * iInfo.frames) / samplerate());
  }

  std::string toString() const
  {
    std::stringstream ss;

    ss << typeToString() << " "
       << channelsToString() << " " << samplerateToString() << " "
       << bitrateToString() << " " << durationToString();

    return ss.str();
  }

  SF_INFO* nativeHandle()
  {
    return &iInfo;
  }

  void setSndfileHandle(SNDFILE* aHandle)
  {
    iSndfileHandle = aHandle;
  }

private:
  std::string typeToString() const
  {
    switch (iInfo.format & SF_FORMAT_TYPEMASK)
    {
      case SF_FORMAT_WAV:   return "Microsoft WAV format (little endian)";
      case SF_FORMAT_AIFF:  return "Apple/SGI AIFF format (big endian)";
      case SF_FORMAT_AU:    return "Sun/NeXT AU format (big endian)";
      case SF_FORMAT_RAW:   return "RAW PCM data";
      case SF_FORMAT_PAF:   return "Ensoniq PARIS file format;";
      case SF_FORMAT_SVX:   return "Amiga IFF / SVX8 / SV16 format";
      case SF_FORMAT_NIST:  return "Sphere NIST format";
      case SF_FORMAT_VOC:   return "VOC files";
      case SF_FORMAT_IRCAM: return "Berkeley/IRCAM/CARL";
      case SF_FORMAT_W64:   return "Sonic Foundry's 64 bit RIFF/WAV";
      case SF_FORMAT_MAT4:  return "Matlab (tm) V4.2 / GNU Octave 2.0";
      case SF_FORMAT_MAT5:  return "Matlab (tm) V5.0 / GNU Octave 2.1";
      case SF_FORMAT_PVF:   return "Portable Voice Format";
      case SF_FORMAT_XI:    return "Fasttracker 2 Extended Instrument";
      case SF_FORMAT_HTK:   return "HMM Tool Kit format";
      case SF_FORMAT_SDS:   return "Midi Sample Dump Standard";
      case SF_FORMAT_AVR:   return "Audio Visual Research";
      case SF_FORMAT_WAVEX: return "MS WAVE with WAVEFORMATEX";
      case SF_FORMAT_SD2:   return "Sound Designer 2";
      case SF_FORMAT_FLAC:  return "FLAC lossless file format";
      case SF_FORMAT_CAF:   return "Core Audio File format";
      case SF_FORMAT_WVE:   return "Psion WVE format";
      case SF_FORMAT_OGG:   return "Xiph OGG container";
      case SF_FORMAT_MPC2K: return "Akai MPC 2000 sampler";
      case SF_FORMAT_RF64:  return "RF64 WAV file";

      default: return "Unknown format";
    }
  }

private:
  SF_INFO iInfo;
  SNDFILE* iSndfileHandle;
};

#endif // SNDFILE_INFO_H
