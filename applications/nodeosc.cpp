#include <audio/io.h>
#include <dsp/generators/oscillator.h>
#include <dsp/generators/waveforms.h>

#include <nodes/nodes/common.h>

#include <algorithm>
#include <iostream>

using Audio = AudioIO<float>;

class SineOscillator : public NodeNumberGenerator
{
public:
  SineOscillator()
    : NodeNumberGenerator("Oscillator", "Math")
    , iSine(440.0, 44100.0)
  {
  }

  float process()
  {
    return iSine();
  }

private:
  Oscillator<WaveformType::Sine<float>> iSine;
};

class Waveform
{
public:
  void generate(Audio::sample_iterator aInBegin,
                Audio::sample_iterator aInEnd,
                Audio::sample_iterator aOutBegin,
                Audio::sample_iterator aOutEnd)
  {
    (void)aInBegin;
    (void)aInEnd;
    (void)aOutEnd;

    for (Audio::sample_iterator it = aOutBegin; it != aOutEnd; it += 2)
    {
      sine.compute();
      float sample = sine.fields().getField("out", Field::Direction::Output).getValue();

      *it = sample;
      *(it + 1) = sample;
    }
  }

private:
  SineOscillator sine;
};

int main(int argc, char* argv[])
{
  Waveform waveform;
  Audio audio(Audio::binder(&Waveform::generate, waveform));
  audio.play();

  std::cout << "Stream opened with a buffer size of " << audio.bufferSize()
            << " samples, and a stream latency of " << audio.streamLatency()
            << " samples (" << (audio.streamLatency() / audio.samplerateControl().getSamplerate()) * 1000. << " ms.)" << std::endl;

  std::cin.get();
}
