#include "SimpleAudio.h"
#include "SimpleMP3.h"

#include <algorithm>
#include <iostream>

using Audio = SimpleAudio<signed long>;
using MP3 = SimpleMP3<signed long>;

class Playback
{
public:
  explicit Playback(MP3& aMP3)
    : iMP3(aMP3)
  {
  }

  void playback(Audio::sample_iterator aInBegin,
                Audio::sample_iterator aInEnd,
                Audio::sample_iterator aOutBegin,
                Audio::sample_iterator aOutEnd)
  {
    (void)aInBegin;
    (void)aInEnd;
    (void)aOutEnd;

    unsigned int decodedSamples = iMP3.decode();

    if (decodedSamples > 0)
    {
      std::transform(iMP3.data().cbegin(), iMP3.data().cbegin() + decodedSamples, aOutBegin, [](signed long sample) { return sample; });
      //std::copy(iMP3.data().cbegin(), iMP3.data().cbegin() + decodedSamples, aOutBegin);
    }
  }

private:
  MP3& iMP3;
};

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.mp3" << std::endl;
    return -1;
  }

  MP3 mp3(argv[1], 512);
  Playback playback(mp3);
  Audio audio(Audio::binder(&Playback::playback, playback));
  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl;

  std::cin.get();
}
