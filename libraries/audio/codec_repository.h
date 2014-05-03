#ifndef CODEC_REPOSITORY_H
#define CODEC_REPOSITORY_H

#include "codecs/mpadec_codec.h"
#include "codecs/sndfile_codec.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>

template <typename T>
class CodecRepository
{
public:
  CodecRepository()
  {
    registerCodec<SndFileCodec<T>>();
    registerCodec<MPADECCodec<T>>();
  }

  std::shared_ptr<IAudioCodec<T>> open(const std::string& aFilename)
  {
    for (auto codec : iRegisteredCodecs)
    {
      try
      {
        return codec(aFilename);
      }
      catch (FormatNotSupportedException& e)
      {
        continue;
      }
    }
  }

private:
  template <typename Codec>
  void registerCodec()
  {
    iRegisteredCodecs.push_back(std::bind(&CodecRepository::tryOpen<Codec>, this, std::placeholders::_1));
  }

  template <typename Codec>
  std::shared_ptr<IAudioCodec<T>> tryOpen(const std::string& aFilename)
  {
    return std::make_shared<Codec>(aFilename);
  }

private:
  std::vector<std::function<std::shared_ptr<IAudioCodec<T>>(const std::string&)>> iRegisteredCodecs;
};

#endif // CODEC_REPOSITORY_H
