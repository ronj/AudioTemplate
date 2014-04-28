#include <audio/io.h>
#include <dsp/generators/oscillator.h>
#include <dsp/generators/waveforms.h>

#include <nodes/models/connection.h>
#include <nodes/nodes/common.h>
#include <nodes/nodes/math.h>

#include <algorithm>
#include <iostream>

using Audio = AudioIO<float>;

class SineOscillator : public NodeNumberGenerator
{
public:
  SineOscillator(float aFreq)
    : NodeNumberGenerator("Oscillator", "Math")
    , iSine(aFreq, 44100.0)
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
  Waveform()
    : sine(440.0f)
    , sine2(400.0f)
  {
    c1 = std::make_shared<Connection>(
      sine.fields().getField("out", Field::Direction::Output),
      mult.fields().getField("x", Field::Direction::Input));
    c2 = std::make_shared<Connection>(
      sine2.fields().getField("out", Field::Direction::Output),
      mult.fields().getField("y", Field::Direction::Input));
  }

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
      sine2.compute();
      mult.compute();
      float sample = mult.fields().getField("out", Field::Direction::Output).getValue();

      *it = sample;
      *(it + 1) = sample;
    }
  }

private:
  SineOscillator sine;
  SineOscillator sine2;
  Multiply mult;
  std::shared_ptr<Connection> c1;
  std::shared_ptr<Connection> c2;
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
