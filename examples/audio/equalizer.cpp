#include <audio/audio.h>

#include <dsp/filters/Dsp.h>

#include <algorithm>
#include <memory>
#include <iostream>

using Audio = invent::audio::AudioIO;

class ParametricEqualizer
{
public:
	ParametricEqualizer(const std::vector<unsigned int>& aBands, unsigned int aSamplerate)
	{
		Dsp::Params params;

		for (auto band : aBands)
		{
			m_Bands.emplace_back(std::make_shared<Dsp::SmoothedFilterDesign<Dsp::RBJ::Design::BandShelf, 2>>(1024));

			params.clear();
			params[0] = aSamplerate;
			params[1] = band;
			params[2] = Dsp::ParamInfo::defaultGainParam().getDefaultValue();
			params[3] = Dsp::ParamInfo::defaultBandwidthParam().getDefaultValue();

			m_Bands.back()->setParams(params);
		}
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
	}

	void adjustBandBandwidth(int band, float adjust)
	{
		m_Bands[band]->setParam(3, m_Bands[band]->getParam(3) + adjust);
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

  CodecRepository<Audio::sample_type> codecs;
  auto codec = codecs.open(argv[1]);
  invent::audio::blocks::DefaultDecoder decoder(*codec);

  ParametricEqualizer eq({ 32, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 }, 44100);

  std::vector<Audio::sample_type> leftChannelData(Audio::DefaultBufferSize);
  std::vector<Audio::sample_type> rightChannelData(Audio::DefaultBufferSize);
  float* arrayOfChannels[2] = { leftChannelData.data(), rightChannelData.data() };

  Audio audio([&](Audio::sample_iterator aInBegin,
                  Audio::sample_iterator aInEnd,
                  Audio::sample_iterator aOutBegin,
                  Audio::sample_iterator aOutEnd)
              {
				auto audio = decoder.peek();
                if (audio)
                {
					Dsp::deinterleave(Audio::DefaultBufferSize, leftChannelData.data(), rightChannelData.data(), audio->data());

					eq.process(Audio::DefaultBufferSize, arrayOfChannels);

					Dsp::interleave(Audio::DefaultBufferSize, aOutBegin, leftChannelData.data(), rightChannelData.data());

					decoder.pop();
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
	  case 'w': eq.adjustBandGain(0, 1.0f); break;
	  case 's': eq.adjustBandGain(0, -1.0f); break;

	  case 'e': eq.adjustBandGain(1, 1.0f); break;
	  case 'd': eq.adjustBandGain(1, -1.0f); break;

	  case 'r': eq.adjustBandGain(2, 1.0f); break;
	  case 'f': eq.adjustBandGain(2, -1.0f); break;

	  case 'o': eq.adjustBandGain(8, 1.0f); break;
	  case 'l': eq.adjustBandGain(8, -1.0f); break;
	  }
  }

  decoder.join();
}
