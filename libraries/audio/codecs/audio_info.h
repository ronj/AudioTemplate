#ifndef IAUDIO_INFO_H
#define IAUDIO_INFO_H

#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>

class IAudioInfo
{
public:
   using audio_duration = std::chrono::duration<float, std::chrono::seconds::period>;
   static_assert(std::chrono::treat_as_floating_point<audio_duration::rep>::value,
                 "IAudioInfo::audio_duration required to be floating point.");

public:
  virtual ~IAudioInfo() {}
  virtual unsigned int channels() const = 0;
  virtual unsigned int bitrate() const = 0;
  virtual unsigned int samplerate() const = 0;
  virtual audio_duration duration() const = 0;

  virtual std::string toString() const = 0;

protected:
  std::string channelsToString() const
  {
    switch (channels())
    {
      case 0: return "unknown";
      case 1: return "mono";
      case 2: return "stereo";
      default: return "multi-channel";
    }
  }

  std::string bitrateToString() const
  {
    return std::to_string(bitrate()) + " Kbps";
  }

  std::string samplerateToString() const
  {
	  return std::to_string(samplerate()) + " Hz";
  }

  std::string durationToString() const
  {
    int hrs = duration().count() / 3600.0;
    int min = (duration().count() - (hrs * 3600.0)) / 60.0;
    double sec = duration().count() - (hrs * 3600.0) - (min * 60.0);

    std::stringstream ss;
    ss.fill('0');
    ss << std::setw(2) << hrs << ":"
       << std::setw(2) << min << ":"
       << std::setprecision(4) << sec;

    return ss.str();
  }
};

#endif // IAUDIO_INFO_H
