#include <audio/audio_io.h>
#include <audio/codec_repository.h>

#include <common/readerwriterqueue.h>

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
  auto codec = codecs.open(argv[1]);

  moodycamel::ReaderWriterQueue<std::vector<Audio::sample_type>> q;

  std::thread decoderThread = std::thread([&](){
    std::size_t decodedSamples = 0;

    do
    {
      std::vector<Audio::sample_type> audioData(Audio::DefaultBufferSize * codec->info().channels());
      decodedSamples = codec->decode(audioData.data(), audioData.capacity());

      bool queue_full = true;
      while (queue_full)
      {
        queue_full = !q.try_enqueue(audioData);
        std::chrono::milliseconds dura( 10 );
        std::this_thread::sleep_for( dura );
      }
    } while (decodedSamples > 0);
  });

  Audio audio([&](Audio::sample_iterator aInBegin,
                  Audio::sample_iterator aInEnd,
                  Audio::sample_iterator aOutBegin,
                  Audio::sample_iterator aOutEnd)
              {
                std::vector<Audio::sample_type> audioData(Audio::DefaultBufferSize * codec->info().channels());

                if (q.try_dequeue(audioData))
                {
                  std::copy(audioData.cbegin(), audioData.cend(), aOutBegin);
                }
              });

  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl
            << "Playing back " << argv[1] << ", which is " << codec->info().toString() << std::endl;

  decoderThread.join();
}
