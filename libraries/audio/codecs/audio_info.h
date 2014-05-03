#ifndef IAUDIO_INFO_H
#define IAUDIO_INFO_H

#include <string>
#include <sstream>

class IAudioInfo
{
public:
  virtual ~IAudioInfo() {}
  virtual unsigned int channels() const = 0;
  virtual unsigned int bitrate() const = 0;
  virtual unsigned int samplerate() const = 0;
  virtual unsigned int duration() const = 0;

  virtual std::string toString() const = 0;

public:
  template <typename T>
  std::string Stringify(T aConvert) const
  {
    std::stringstream ss;
    ss << aConvert;
    return ss.str();
  }
};

#endif // IAUDIO_INFO_H
