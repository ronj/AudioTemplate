#include <audio/audio.h>

#include <algorithm>
#include <iostream>

using Audio = invent::audio::AudioIO;

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.[mp3|ogg|flac|wav|aiff]" << std::endl;
    return -1;
  }

  invent::audio::CodecRepository::registerCommonCodecs();
  auto codec = invent::audio::CodecRepository::open(argv[1]);
  invent::audio::blocks::DefaultDecoder decoder(codec);

  Audio audio([&](Audio::sample_iterator aInBegin,
                  Audio::sample_iterator aInEnd,
                  Audio::sample_iterator aOutBegin,
                  Audio::sample_iterator aOutEnd)
              {
				        auto data = decoder.peek();

                if (data)
                {
				          std::copy(data->begin(), data->end(), aOutBegin);
				          decoder.pop();
                }
                else
                {
                  std::fill(aOutBegin, aOutEnd, 0.0f);
                }
              });

  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl
            << "Playing back " << argv[1] << ", which is " << codec->info().toString() << std::endl;

  decoder.join();
}
