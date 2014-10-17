#include <audio/audio.h>
#include <audio/codec_repository.h>

#include <dsp/effects/FFTConvolver.h>

#include <algorithm>
#include <iostream>

// See IR's from: http://fokkie.home.xs4all.nl/IR.htm

using Audio = invent::audio::AudioIO;

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " input_file.[mp3|ogg|flac|wav|aiff] convolution_file.[mp3|ogg|flac|wav|aiff]" << std::endl;
    return -1;
  }

  fftconvolver::FFTConvolver convolver;
  CodecRepository<Audio::sample_type> codecs;

  std::shared_ptr<IAudioCodec<Audio::sample_type>> input = codecs.open(argv[1]);
  std::shared_ptr<IAudioCodec<Audio::sample_type>> convolve = codecs.open(argv[2]);

  std::vector<Audio::sample_type> convolveData;
  std::vector<Audio::sample_type> inputData(Audio::DefaultBufferSize * input->info().channels());
  std::vector<Audio::sample_type> output(Audio::DefaultBufferSize * input->info().channels());

  std::size_t samplesDecoded = 0;
  std::vector<Audio::sample_type> temp(Audio::DefaultBufferSize * convolve->info().channels());

  do
  {
    samplesDecoded = convolve->decode(temp.data(), temp.capacity());
    std::copy(temp.cbegin(), temp.cbegin() + samplesDecoded, std::back_inserter(convolveData));
  } while (samplesDecoded > 0);

  if (!convolver.init(Audio::DefaultBufferSize, convolveData.data(), convolveData.size()))
  {
    std::cout << "Convolution initialization failed" << std::endl;
  }

  Audio audio([&](Audio::sample_iterator aInBegin,
                  Audio::sample_iterator aInEnd,
                  Audio::sample_iterator aOutBegin,
                  Audio::sample_iterator aOutEnd)
              {
                std::size_t decodedSamples = input->decode(inputData.data(), inputData.capacity());

                if (decodedSamples > 0)
                {
                  convolver.process(inputData.data(), output.data(), decodedSamples);
                  std::copy(output.cbegin(), output.cbegin() + decodedSamples, aOutBegin);
                }
              });

  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl
            << "Convolving " << argv[1] << ", which is " << input->info().toString() << " with " << argv[2] << ", which is " << convolve->info().toString() << std::endl;

  std::cin.get();
}
