#include <audio/audio.h>
#include <audio/codec_repository.h>

#include <dsp/analysis/fft.h>
#include <dsp/analysis/beat_detector.h>
#include <dsp/common/dsp_algorithm.h>
#include <dsp/generators/oscillator.h>
#include <dsp/generators/waveforms.h>
#include <dsp/effects/FFTConvolver.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <chrono>
#include <functional>

using Audio = invent::audio::AudioIO;

class Playback
{
public:
  Playback(const std::string& aFilename, unsigned int aSamplesPerChannel)
    : iMP3(iCodecs.open(aFilename))
    , fft(Fft::create())
    , beat_lo(48, 95)
    , beat_mid(85, 169, &beat_lo)
    , beat_high(150, 280, &beat_lo)
    , sine(440, 44100)
    , sine2(400, 44100)
    , iDecodedData(aSamplesPerChannel * iMP3->info().channels(), 0.0f)
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

    static auto start = std::chrono::steady_clock::now();
    std::size_t decodedSamples = iMP3->decode(iDecodedData.data(), iDecodedData.capacity());

    fft->setSignal(iDecodedData);
    float* amp = fft->getAmplitude();
    std::vector<float> amplitude(fft->getBinSize());
    std::copy(fft->getAmplitude(), fft->getAmplitude() + fft->getBinSize(), amplitude.begin());

    float timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();

    beat_lo.process(timestamp / 1000.f, amplitude);
    beat_mid.process(timestamp / 1000.f, amplitude);
    beat_high.process(timestamp / 1000.f, amplitude);

    system("clear");

    std::cout.fill('*');
    for (int i = 0; i < fft->getBinFromFrequency(1000); i++)
    {
      std::cout.width(amp[i] * 100);
      std::cout << "" << std::endl;
    }

    std::cout << "Time: " << timestamp / 1000.f << std::endl;
    if (!beat_lo.is_erratic)  std::cout << "Low: " << beat_lo.win_bpm_int / 10.f << " BPM / " << beat_lo.win_bpm_int_lo << " BPM" << std::endl;
    if (!beat_mid.is_erratic) std::cout << "Med: " << beat_mid.win_bpm_int / 10.f << " BPM / " << beat_mid.win_bpm_int_lo << " BPM" << std::endl;
    if (!beat_high.is_erratic) std::cout << "Hi : " << beat_high.win_bpm_int / 10.f << " BPM / " << beat_high.win_bpm_int_lo << " BPM" << std::endl;
    std::cout << "RMS: " << rms(iDecodedData.cbegin(), iDecodedData.cend()) << std::endl;
    std::cout << "AMP: " << max_amplitude(iDecodedData.cbegin(), iDecodedData.cend()) << std::endl;
    std::cout << "DEV: " << std_dev(iDecodedData.cbegin(), iDecodedData.cend()) << std::endl;

    std::vector<float> data(decodedSamples);
    float sample = sine() + sine2() * sine();

    if (decodedSamples > 0)
    {
      std::copy(iDecodedData.cbegin(), iDecodedData.cbegin() + decodedSamples, data.begin());
      //std::transform(data.begin(), data.end(), aOutBegin, std::bind2nd(std::multiplies<float>(), sample));
      std::copy(data.begin(), data.end(), aOutBegin);
    }
  }

private:
  invent::audio::CodecRepository iCodecs;
  std::shared_ptr<IAudioCodec> iMP3;
  std::vector<Audio::sample_type> iDecodedData;
  std::shared_ptr<Fft> fft;
  BeatDetektor beat_lo;
  BeatDetektor beat_mid;
  BeatDetektor beat_high;
  Oscillator<WaveformType::Sine<float>> sine;
  Oscillator<WaveformType::Sine<float>> sine2;
  fftconvolver::FFTConvolver iConvolver;
};

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " file.mp3" << std::endl;
    return -1;
  }

  Audio audio;
  Playback playback(argv[1], audio.bufferSize());
  audio.setCallback(Audio::binder(&Playback::playback, playback));
  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl;

  std::cin.get();
}
