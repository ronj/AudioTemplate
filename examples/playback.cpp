#include <audio/audio_io.h>
#include <audio/codec_repository.h>

#include <algorithm>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

using Audio = AudioIO<float>;

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.[mp3|ogg|flac|wav|aiff]" << std::endl;
    return -1;
  }

  CodecRepository<Audio::sample_type> codecs;
  std::shared_ptr<IAudioCodec<Audio::sample_type>> codec = codecs.open(argv[1]);
  std::vector<Audio::sample_type> audioData(Audio::DefaultBufferSize * codec->info().channels());

  std::thread decoderThread = std::thread([&](){ std::cout << "Decoding" << std::endl; });

  Audio audio([&](Audio::sample_iterator aInBegin,
                  Audio::sample_iterator aInEnd,
                  Audio::sample_iterator aOutBegin,
                  Audio::sample_iterator aOutEnd)
              {
                std::size_t decodedSamples = codec->decode(audioData.data(), audioData.capacity());

                if (decodedSamples > 0)
                {
                  std::copy(audioData.cbegin(), audioData.cbegin() + decodedSamples, aOutBegin);
                }
              });

  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl
            << "Playing back " << argv[1] << ", which is " << codec->info().toString() << std::endl;

  std::cin.get();
}
