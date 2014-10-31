#ifndef IAUDIO_CODEC_H
#define IAUDIO_CODEC_H

#include <cstddef>
#include <exception>

class IAudioInfo;

class FormatNotSupportedException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Format is not supported by this codec.";
  }
};

class IAudioCodec
{
public:
  virtual ~IAudioCodec() {}
  virtual std::size_t decode(float* aSamples, std::size_t aSampleCount) const = 0;
  virtual std::size_t encode(const float* aSamples, std::size_t aSampleCount) const = 0;
  virtual const IAudioInfo& info() const = 0;
};

#endif // IAUDIO_CODEC_H
