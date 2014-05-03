#ifndef MPADEC_CODEC_H
#define MPADEC_CODEC_H

#include <mp3dec.h>

#include "audio_codec.h"
#include "mpadec_info.h"

#include <fcntl.h>
#include <unistd.h>

template <typename T>
class MPADECCodec : public IAudioCodec<T>
{
public:
  using sample_type = T;

public:
  MPADECCodec(const std::string& aFilename)
    : iFile(open(aFilename.c_str(), O_RDONLY | O_BINARY))
    , iDecoder(mp3dec_init())
  {
    if (!iDecoder)
    {
      close(iFile);
      throw std::bad_alloc();
    }

    setFormat(iConfig);

    apiWrapper(mp3dec_configure, iDecoder, &iConfig);
    apiWrapper(mp3dec_init_file, iDecoder, iFile, 0, FALSE);
    apiWrapper(mp3dec_get_info, iDecoder, iInfo.nativeHandle(), MPADEC_INFO_STREAM);
  }

  virtual ~MPADECCodec()
  {
    cleanup();
  }

  std::size_t decode(sample_type* aSamples, std::size_t aSampleCount)
  {
    unsigned int bufferUsedSize = 0;

    mp3dec_decode(iDecoder,
                  reinterpret_cast<uint8_t*>(aSamples),
                  aSampleCount * sizeof(T),
                  &bufferUsedSize);

    return bufferUsedSize / sizeof(T);
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
  template <typename Method, typename... Args>
  void apiWrapper(Method&& method, Args... arguments)
  {
    int returnValue = MP3DEC_RETCODE_OK;
    if ((returnValue = method(arguments...)) != MP3DEC_RETCODE_OK)
    {
      cleanup();

      if (returnValue == MP3DEC_RETCODE_NOT_MPEG_STREAM)
      {
        throw FormatNotSupportedException();
      }

      throw std::logic_error(mp3dec_error(returnValue));
    }
  }

  void cleanup()
  {
    if (iDecoder)
    {
      mp3dec_uninit(iDecoder);
    }

    close(iFile);
  }

  void setFormat(mpadec_config_t& aConfig);

private:
  int                    iFile = -1;
  mp3dec_t               iDecoder = nullptr;
  mpadec_config_t        iConfig = { MPADEC_CONFIG_FULL_QUALITY, MPADEC_CONFIG_AUTO,
                                     MPADEC_CONFIG_FLOAT, MPADEC_CONFIG_LITTLE_ENDIAN,
                                     MPADEC_CONFIG_REPLAYGAIN_NONE, TRUE, TRUE, TRUE, 0.0 };
  MPADECInfo             iInfo;
};

template <typename T>
void MPADECCodec<T>::setFormat(mpadec_config_t&)
{
  throw std::logic_error(std::string("Sample type ") + typeid(T).name() + " not supported.");
}

template <>
void MPADECCodec<signed short>::setFormat(mpadec_config_t& aConfig)
{
  aConfig.format = MPADEC_CONFIG_16BIT;
}

template <>
void MPADECCodec<signed long>::setFormat(mpadec_config_t& aConfig)
{
  aConfig.format = MPADEC_CONFIG_32BIT;
}

template <>
void MPADECCodec<float>::setFormat(mpadec_config_t& aConfig)
{
  aConfig.format = MPADEC_CONFIG_FLOAT;
}

#endif // MPADEC_CODEC_H
