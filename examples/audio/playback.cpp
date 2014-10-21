#include <audio/audio.h>
#include <audio/codec_repository.h>

#include <common/readerwriterqueue.h>

#include <algorithm>
#include <array>
#include <thread>
#include <iostream>

using Audio = invent::audio::AudioIO;

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.[mp3|ogg|flac|wav|aiff]" << std::endl;
    return -1;
  }

  CodecRepository<Audio::sample_type> codecs;
  auto codec = codecs.open(argv[1]);

  moodycamel::ReaderWriterQueue<std::array<Audio::sample_type, Audio::DefaultBufferSize * 2>> q;

  std::thread decoderThread = std::thread([&](){
    std::size_t decodedSamples = 0;

    do
    {
	  std::array<Audio::sample_type, Audio::DefaultBufferSize * 2> decoderBuffer;
	  decodedSamples = codec->decode(decoderBuffer.data(), decoderBuffer.size());

      bool elementEnqueued = false;
	  do
	  {
		  if (q.try_enqueue(decoderBuffer))
		  {
			  elementEnqueued = true;
		  }
		  else
		  {
			  const double sleepTime = ((1.0 / codec->info().samplerate()) * 1000) * Audio::DefaultBufferSize;
			  std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(std::round(sleepTime))));
		  }
	  } while (!elementEnqueued);
    } while (decodedSamples > 0);
  });

  Audio audio([&](Audio::sample_iterator aInBegin,
                  Audio::sample_iterator aInEnd,
                  Audio::sample_iterator aOutBegin,
                  Audio::sample_iterator aOutEnd)
              {
				auto data = q.peek();
                if (data)
                {
				  std::copy(data->cbegin(), data->cend(), aOutBegin);
				  q.pop();
                }
              });

  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl
            << "Playing back " << argv[1] << ", which is " << codec->info().toString() << std::endl;

  decoderThread.join();
}
