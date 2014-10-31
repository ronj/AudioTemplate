#include <audio/audio.h>
#include <audio/codec_repository.h>

#include <common/stride_iterator.h>
#include <common/readerwriterqueue.h>

#include <dsp/filters/Dsp.h>

#include <algorithm>
#include <array>
#include <thread>
#include <memory>
#include <iostream>

using Audio = invent::audio::AudioIO;

class ParametricEqualizer
{
public:
	void addFilter(std::shared_ptr<Dsp::Filter> aFilter)
	{
		m_Bands.push_back(std::move(aFilter));
	}

	void process(unsigned int aSampleCount, Audio::sample_type* const* aArrayOfChannels)
	{
		for (auto band : m_Bands)
		{
			band->process(aSampleCount, aArrayOfChannels);
		}
	}

	void adjustBandGain(int band, float adjust)
	{
		m_Bands[band]->setParam(2, m_Bands[band]->getParam(2) + adjust);
		std::cout << "New value: " << m_Bands[band]->getParam(2) << std::endl;
	}

private:
	std::vector<std::shared_ptr<Dsp::Filter>> m_Bands;
};

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.[mp3|ogg|flac|wav|aiff]" << std::endl;
    return -1;
  }

  invent::audio::CodecRepository::registerCommonCodecs();

  auto codec = invent::audio::CodecRepository::open(argv[1]);
  moodycamel::ReaderWriterQueue<std::vector<Audio::sample_type>> q;

  ParametricEqualizer eq;
  
  std::array<int, 10> bands = { 32, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
  Dsp::Params params;


  for (auto band : bands)
  {
	  params.clear();

	  std::shared_ptr<Dsp::Filter> f0 = std::make_shared<Dsp::SmoothedFilterDesign<Dsp::RBJ::Design::BandShelf, 2>>(1024);
	  
	  params[0] = 44100;
	  params[1] = band;
	  params[2] = Dsp::ParamInfo::defaultGainParam().getDefaultValue();
	  params[3] = Dsp::ParamInfo::defaultBandwidthParam().getDefaultValue();

	  f0->setParams(params);
	  eq.addFilter(f0);
  }

  std::thread decoderThread = std::thread([&]() {
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

  std::vector<Audio::sample_type> audioData(Audio::DefaultBufferSize * codec->info().channels());
  std::vector<Audio::sample_type> leftChannelData(Audio::DefaultBufferSize);
  std::vector<Audio::sample_type> rightChannelData(Audio::DefaultBufferSize);
  float* arrayOfChannels[2] = { leftChannelData.data(), rightChannelData.data() };

  Audio audio([&](Audio::sample_iterator aInBegin,
                  Audio::sample_iterator aInEnd,
                  Audio::sample_iterator aOutBegin,
                  Audio::sample_iterator aOutEnd)
              {
                if (q.try_dequeue(audioData))
                {
					Dsp::deinterleave(Audio::DefaultBufferSize, leftChannelData.data(), rightChannelData.data(), audioData.data());

					eq.process(Audio::DefaultBufferSize, arrayOfChannels);

					Dsp::interleave(Audio::DefaultBufferSize, aOutBegin, leftChannelData.data(), rightChannelData.data());
                }
              });

  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl
            << "Playing back " << argv[1] << ", which is " << codec->info().toString() << std::endl;

  char x;
  while (std::cin.get(x))
  {
	  switch (x)
	  {
	  case 'w': eq.adjustBandGain(0, 1.f); break;
	  case 's': eq.adjustBandGain(0, -1.f); break;

	  case 'e': eq.adjustBandGain(1, 1.0f); break;
	  case 'd': eq.adjustBandGain(1, -1.0f); break;

	  case 'r': eq.adjustBandGain(2, 1.0f); break;
	  case 'f': eq.adjustBandGain(2, -1.0f); break;

	  case 'o': eq.adjustBandGain(8, 1.0f); break;
	  case 'l': eq.adjustBandGain(8, -1.0f); break;
	  }
  }

  decoderThread.join();
}
