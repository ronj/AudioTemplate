#ifndef IAUDIO_CODEC_H
#define IAUDIO_CODEC_H

#include <cstddef>
#include <exception>

class IAudioInfo;

using namespace std;

class FormatNotSupportedException : public exception
{
  virtual const char* what() const throw()
  {
    return "Format is not supported by this codec.";
  }
};

template <typename TSample>
class IAudioCodec
{
public:
  virtual ~IAudioCodec() {}
  virtual std::size_t decode(TSample* aSamples, std::size_t aSampleCount) = 0;
  virtual std::size_t encode(const TSample* aSamples, std::size_t aSampleCount) = 0;
  virtual const IAudioInfo& info() const = 0;
};

#endif // IAUDIO_CODEC_H
