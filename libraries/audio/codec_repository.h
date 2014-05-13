#ifndef CODEC_REPOSITORY_H
#define CODEC_REPOSITORY_H

#include <audio/codecs/mpadec_codec.h>
#include <audio/codecs/sndfile_codec.h>

#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class NoCodecForFiletype : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Format is not supported by any of the registered codecs.";
  }
};

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

    throw NoCodecForFiletype();
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
